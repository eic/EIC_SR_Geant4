#include "GammaReflectionProcess.hh"
#include "G4VParticleChange.hh"
#include "G4VSolid.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParallelWorldProcess.hh"
#include "G4TransportationManager.hh"
#include "G4Exp.hh"

#define NMAXLOOPS 100 // the maximum number of loops to find acceptable angular offsets

GammaReflectionProcess::GammaReflectionProcess(	G4int fType, G4bool print, 
						G4double fRoughness, G4double fCorrLength, G4double fSigma) : 
						fReflectionType(fType), fSurfaceRoughness(fRoughness), 
						fAutoCorrLength(fCorrLength), fSigmaRatio(fSigma), 
						G4VProcess("GammaReflectionProcess")
{
	Init(print);
}

GammaReflectionProcess::GammaReflectionProcess() : G4VProcess("GammaReflectionProcess")
{
	// default
	fReflectionType = 3;
	fSurfaceRoughness = 50e-9 * m; // RMS roughness
	fAutoCorrLength = 10000e-9 * m; // autocorrelation length
	fSigmaRatio = 0.005; 	// the ratio of the sigmaH and sigmaX values, where sigmaX is the standard deviation 
				// of the size of cavities on the surface, and sigmaH is the st.dev. of the depth of 
				// those

	Init(true);
}

void GammaReflectionProcess::Init(G4bool print)
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] GammaReflectionProcess::GammaReflectionProcess"<<G4endl;
	G4cout<<"X-ray reflection process is:"<<G4endl;
	switch(fReflectionType)
	{
		case 0:
			G4cout<<" - DoXrayReflection:";
			G4cout<<" specular reflection with Henke data [30;30000] eV used to calculate";
			G4cout<<" the reflection and Névot-Croce factor attenuation";
			G4cout<<G4endl;
			break;
		case 1:
			G4cout<<" - DoGammaReflection1:";
			G4cout<<" specular reflection with Synrad+ data [1;50000] eV used to calculate";
			G4cout<<" the reflection and Debye-Waller factor attenuation";
			G4cout<<G4endl;
			break;
		case 2:
			G4cout<<" - DoGammaReflection2:";
			G4cout<<" diffuse reflection with Synrad+ data [1;50000] eV used to calculate";
			G4cout<<" the offsets for the surface normal -- old model";
			G4cout<<G4endl;
			break;
		default:
			G4cout<<" - DoGammaReflection3:";
			G4cout<<" diffuse reflection with Synrad+ data [1;50000] eV used to calculate";
			G4cout<<" the reflection, Debye-Waller factor attenuation, and perturbed reflection direction -- new model";
			G4cout<<G4endl;
	}
	G4cout<<"==================================================================\n\n"<<G4endl;

	if(fReflectionType == 0)
	{
		G4cout<<"[INFO] fSurfaceRoughness = "<<fSurfaceRoughness/m<<" [m]"<<G4endl;

		fNistManager = G4NistManager::Instance();
		theMatIP = fNistManager->FindOrBuildMaterial("G4_Au");
		theMatIR = fNistManager->FindOrBuildMaterial("G4_Cu");
		theMatTaggerFoil = fNistManager->FindOrBuildMaterial("G4_Cu");
		theMatLumiExitWindow = fNistManager->FindOrBuildMaterial("G4_Al");
		SaveHenkeDataAsMaterialProperty(print);
	}
	else
	{
		G4cout<<"[INFO] fSigmaRatio = "<<fSigmaRatio<<G4endl;
		G4cout<<"[INFO] fSurfaceRoughness = "<<fSurfaceRoughness/m<<" [m]"<<G4endl;
		G4cout<<"[INFO] fAutoCorrLength = "<<fAutoCorrLength/m<<" [m]"<<G4endl;

		_reflectFileNameMap["Cu"] = "../materials/02-Copper.csv";
		_reflectFileNameMap["Au"] = "../materials/04-Gold.csv";
		_reflectFileNameMap["Al"] = "../materials/01-Aluminium.csv";

		_reflectDataMap["Cu"] = ReadReflectData(	_reflectFileNameMap["Cu"],
								_grazingAngleMap["Cu"],
								_photonEnergyMap["Cu"]);
		_reflectDataMap["Au"] = ReadReflectData(	_reflectFileNameMap["Au"],
								_grazingAngleMap["Au"],
								_photonEnergyMap["Au"]);
		_reflectDataMap["Al"] = ReadReflectData(	_reflectFileNameMap["Al"],
								_grazingAngleMap["Al"],
								_photonEnergyMap["Al"]);

		if(print)
		{
			PrintReflectData("Cu",_reflectDataMap["Cu"],_grazingAngleMap["Cu"],_photonEnergyMap["Cu"]);
			PrintReflectData("Au",_reflectDataMap["Au"],_grazingAngleMap["Au"],_photonEnergyMap["Au"]);
			PrintReflectData("Al",_reflectDataMap["Al"],_grazingAngleMap["Al"],_photonEnergyMap["Al"]);
		}
	}

	return;
}


GammaReflectionProcess::~GammaReflectionProcess()
{}

G4double GammaReflectionProcess::PostStepGetPhysicalInteractionLength(const G4Track&,G4double,G4ForceCondition* condition)
{
	// return "infinite" distance for interaction, but asks for PostStepDoIt
	// being called anyway at the end of the step, by returning the "Forced" condition flag
  	*condition = Forced;
  	return DBL_MAX;
}
  
G4VParticleChange * 
GammaReflectionProcess::PostStepDoIt(const G4Track& track, const G4Step & step)
{
  	aParticleChange.Initialize(track);
	aParticleChange.ProposeVelocity(track.GetVelocity());

    	G4StepPoint*          	preStep		= step.GetPreStepPoint();
    	G4StepPoint*          	postStep	= step.GetPostStepPoint();
    	G4VPhysicalVolume*    	prePV		= preStep->GetPhysicalVolume();
    	G4VPhysicalVolume*	postPV		= postStep->GetPhysicalVolume();

	// check if the next step is not outside the world and skip everything except for gamma
	if(!postPV || track.GetParticleDefinition()->GetPDGEncoding() != G4Gamma::Gamma()->GetPDGEncoding())
		return &aParticleChange;


	// get tracks at the edge of the vacuum beam pipe and world
	//..............-----------------------------------------
	//..............|..................Volume...............|
	//.pre-step.....0....<--.step.status.on.boundary.-->....0.post-step.point
	//.point........|\...................................../|.steps.ends.on.boundary
	//..............|.\.................................../.|
	//..............|..\................................./..|
	//..............|...\.............................../...|
	//..............|....0.post-step.point....pre-step.0....|
	//..............|.........................point.........|
	//..............-----------------------------------------

	// only when leaving the magnet or vacuum and entering the world, tagger, or lumi exit window
	if(	postStep->GetStepStatus() == fGeomBoundary && 
		(postPV->GetName() == "world" || G4StrUtil::contains(postPV->GetName(),"vac_TaggerFoil_") ||
		postPV->GetName() == "lumiWind") &&
		(G4StrUtil::contains(prePV->GetName(),"mag_") || G4StrUtil::contains(prePV->GetName(),"vac_")))
	{
		const G4DynamicParticle* aParticle = track.GetDynamicParticle(); // get the photon

		G4ThreeVector fOldMomentum = aParticle->GetMomentumDirection(); // photon momentum direction
		G4ThreeVector fNewMomentum = fOldMomentum; // new momentum direction after reflection
		G4double GamEner = aParticle->GetTotalEnergy(); // photon energy
		G4ThreeVector theGlobalPoint = step.GetPostStepPoint()->GetPosition(); // reflection point
		G4double prob = 0; // reflection probability
		G4String volName = prePV->GetName(); // volume name

		switch(fReflectionType)
		{
			case 0:
				// Based on new Geant4 implementation of 'G4XrayReflection' - specular reflection
				// Ref.[https://geant4.web.cern.ch/download/11.2.0.html]
				// Uses data from Ref.[https://doi.org/10.1006/adnd.1993.1013] for [30;30000] eV
				DoXrayReflection(GamEner,theGlobalPoint,fOldMomentum,volName,prob,fNewMomentum);
				break;
			case 1:
				// Based on Synrad+ implementation - specular reflection
				// Ref.[https://molflow.web.cern.ch/node/116]
				// Uses data from Ref.[https://doi.org/10.1006/adnd.1993.1013] for [30;30000] eV
				// Uses data from Ref.[https://lib-extopc.kek.jp/preprints/PDF/1981/8102/8102231.pdf] for < 30 eV
				DoGammaReflection1(GamEner,theGlobalPoint,fOldMomentum,volName,prob,fNewMomentum);
				break;
			case 2:
				// Based on Synrad+ implementation (old model) - diffuse reflection
				// Ref.[https://molflow.web.cern.ch/node/116]
				// Uses data from Ref.[https://doi.org/10.1006/adnd.1993.1013] for [30;30000] eV
				// Uses data from Ref.[https://lib-extopc.kek.jp/preprints/PDF/1981/8102/8102231.pdf] for < 30 eV
				DoGammaReflection2(GamEner,theGlobalPoint,fOldMomentum,volName,prob,fNewMomentum);
				break;
			default:
				// Based on Synrad+ implementation (new model) - diffuse reflection
				// Ref.[https://molflow.web.cern.ch/node/116]
				// Uses data from Ref.[https://doi.org/10.1006/adnd.1993.1013] for [30;30000] eV
				// Uses data from Ref.[https://lib-extopc.kek.jp/preprints/PDF/1981/8102/8102231.pdf] for < 30 eV
				DoGammaReflection3(GamEner,theGlobalPoint,fOldMomentum,volName,prob,fNewMomentum);
		}

		// play Monte-Carlo
		if(G4UniformRand() > prob)
    			aParticleChange.ProposeTrackStatus(fStopAndKill); // kill the track
		else
			aParticleChange.ProposeMomentumDirection(fNewMomentum); // propose momentum change
	}

	// get tracks at the edge of the absorber
	if(postStep->GetStepStatus() == fGeomBoundary && G4StrUtil::contains(postPV->GetName(),"abs_"))
	{
		aParticleChange.ProposeTrackStatus(fStopAndKill);
	}

  	return &aParticleChange;
}

void GammaReflectionProcess::DoXrayReflection(	const G4double GamEner, 
						const G4ThreeVector theGlobalPoint, 
						const G4ThreeVector fOldMomentum,
						const G4String volName,
						G4double &prob, 
						G4ThreeVector &fNewMomentum)
{
	G4bool valid;

	// ID of Navigator which limits step
	G4int hNavId = G4ParallelWorldProcess::GetHypNavigatorID();
	auto iNav = G4TransportationManager::GetTransportationManager()->GetActiveNavigatorsIterator();
	//- GetGlobalExitNormal [https://bugzilla-geant4.kek.jp/show_bug.cgi?id=1300]
	// Obtains the Normal vector to a surface (in global coordinates)
	// pointing out of previous volume and into current volume
	G4ThreeVector fGlobalNormal = (iNav[hNavId])->GetGlobalExitNormal(theGlobalPoint, &valid);
	G4double SinIncidentAngle = fOldMomentum * fGlobalNormal;

	if(!valid)
	{
		G4ExceptionDescription ed;
		ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
			<< " The Navigator reports that it returned an invalid normal" << G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection01", 
			JustWarning, // or EventMustBeAborted 
			ed,
			"Invalid Surface Normal - Geometry must return valid surface normal");
	}

	// Total internal reflection
	// https://link.springer.com/chapter/10.1007/978-1-4842-7185-8_8
	// For a mirror reflective surface, the reflection formula is r = i–2(i·n)n,
	// where i is the incoming ray direction, n the surface normal, and r the
	// reflection direction. The normal direction is expected to be
	// normalized (equal to a length of one) in this form. The normal can point in 
	// either direction (e.g., upward or downward) and the formula still works. The 
	// incoming ray direction does not have to be normalized, and the outgoing 
	// direction vector will be computed having the same length as the incoming one.
	fNewMomentum = fOldMomentum - 2. * fOldMomentum * fGlobalNormal * fGlobalNormal;
	fNewMomentum = fNewMomentum.unit(); // normalize: vector parallel to this, but of length 1.

	// get probability
	if(volName == "vac_IP" || volName == "vac_ip") // Au ip beam pipe
		prob = Reflectivity(GamEner, SinIncidentAngle, theMatIP);
	else if(G4StrUtil::contains(volName,"vac_TaggerFoil_")) // Cu tagger foil
		prob = Reflectivity(GamEner, SinIncidentAngle, theMatTaggerFoil);
	else if(volName == "lumiWind") // Al lumi exit window
		prob = Reflectivity(GamEner, SinIncidentAngle, theMatLumiExitWindow);
	else // Cu beam pipe
		prob = Reflectivity(GamEner, SinIncidentAngle, theMatIR);

	return;
}

void GammaReflectionProcess::DoGammaReflection1(const G4double GamEner, 
						const G4ThreeVector theGlobalPoint,
						const G4ThreeVector fOldMomentum,
						const G4String volName, 
						G4double &prob, 
						G4ThreeVector &fNewMomentum)
{
	G4bool valid;

	// ID of Navigator which limits step
	G4int hNavId = G4ParallelWorldProcess::GetHypNavigatorID();
	auto iNav = G4TransportationManager::GetTransportationManager()->GetActiveNavigatorsIterator();
	//- GetGlobalExitNormal [https://bugzilla-geant4.kek.jp/show_bug.cgi?id=1300]
	// Obtains the Normal vector to a surface (in global coordinates)
	// pointing out of previous volume and into current volume
	G4ThreeVector fGlobalNormal = (iNav[hNavId])->GetGlobalExitNormal(theGlobalPoint, &valid);
	G4double SinIncidentAngle = fOldMomentum * fGlobalNormal;

	// flip the normal vector -- now, pointing inside the vacuum
	if(valid)
	{
		fGlobalNormal = -fGlobalNormal;
	}
	else
	{
		G4ExceptionDescription ed;
		ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
			<< " The Navigator reports that it returned an invalid normal" << G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection02", 
			JustWarning, // or EventMustBeAborted 
			ed,
			"Invalid Surface Normal - Geometry must return valid surface normal");
	}

	if(fOldMomentum * fGlobalNormal > 0.0)
	{
		G4ExceptionDescription ed;
		ed 	<< 	" GammaReflectionProcess/PostStepDoIt(): fGlobalNormal points in a "
				"wrong direction. "
			<< G4endl
			<< " The momentum of the photon arriving at interface (oldMomentum)"
			<< " must exit the volume cross in the step. " << G4endl
			<< " So it MUST have dot < 0 with the normal that Exits the new "
			"volume (globalNormal)."
			<< G4endl << "   >> The dot product of oldMomentum and global Normal is "
			<< fOldMomentum * fGlobalNormal << G4endl
			<< "     Old Momentum  (during step)     = " << fOldMomentum << G4endl
			<< "     Global Normal (Exiting New Vol) = " << fGlobalNormal << G4endl
			<< G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection03",
			JustWarning, // or EventMustBeAborted
			ed,
			"Invalid Surface Normal - Geometry must return valid surface "
			"normal pointing in the right direction");
		fGlobalNormal = -fGlobalNormal;
	}

	// Total internal reflection
	// https://link.springer.com/chapter/10.1007/978-1-4842-7185-8_8
	// For a mirror reflective surface, the reflection formula is r = i–2(i·n)n,
	// where i is the incoming ray direction, n the surface normal, and r the
	// reflection direction. The normal direction is expected to be
	// normalized (equal to a length of one) in this form. The normal can point in 
	// either direction (e.g., upward or downward) and the formula still works. The 
	// incoming ray direction does not have to be normalized, and the outgoing 
	// direction vector will be computed having the same length as the incoming one.
	fNewMomentum = fOldMomentum - 2. * fOldMomentum * fGlobalNormal * fGlobalNormal;
	fNewMomentum = fNewMomentum.unit(); // normalize: vector parallel to this, but of length 1.

	// Calculate Angle between Normal and Photon Momentum
	G4double anglePhotonToNormal = fOldMomentum.angle(-fGlobalNormal);
	G4double grazingPhotonAngle = M_PI_2 - anglePhotonToNormal; 

	// get reflection probability from the database
	if(volName == "vac_IP" || volName == "vac_ip") // Au ip beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Au"],
			_grazingAngleMap["Au"],_photonEnergyMap["Au"],grazingPhotonAngle/rad,GamEner/eV);
	else if(G4StrUtil::contains(volName,"vac_TaggerFoil_")) // Cu tagger foil
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);
	else if(volName == "lumiWind") // Al lumi exit window
		prob = FindReflectionProbability(_reflectDataMap["Al"],
			_grazingAngleMap["Al"],_photonEnergyMap["Al"],grazingPhotonAngle/rad,GamEner/eV);
	else // Cu beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);

	//- Roughtness
	// Debye–Waller factor [https://doi.org/10.1364/OSAC.422924]	
	G4double kiz = SinIncidentAngle * GamEner / CLHEP::hbarc;
	prob *= G4Exp(-2.0 * pow(kiz * fSurfaceRoughness,2));

	return;
}

void GammaReflectionProcess::DoGammaReflection2(const G4double GamEner, 
						const G4ThreeVector theGlobalPoint,
						const G4ThreeVector fOldMomentum,
						const G4String volName, 
						G4double &prob, 
						G4ThreeVector &fNewMomentum)
{
	G4bool valid;

	// ID of Navigator which limits step
	G4int hNavId = G4ParallelWorldProcess::GetHypNavigatorID();
	auto iNav = G4TransportationManager::GetTransportationManager()->GetActiveNavigatorsIterator();
	//- GetGlobalExitNormal [https://bugzilla-geant4.kek.jp/show_bug.cgi?id=1300]
	// Obtains the Normal vector to a surface (in global coordinates)
	// pointing out of previous volume and into current volume
	G4ThreeVector fGlobalNormal = (iNav[hNavId])->GetGlobalExitNormal(theGlobalPoint, &valid);

	// flip the normal vector -- now, pointing inside the vacuum
	if(valid)
	{
		fGlobalNormal = -fGlobalNormal;
	}
	else
	{
		G4ExceptionDescription ed;
		ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
			<< " The Navigator reports that it returned an invalid normal" << G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection02", 
			JustWarning, // or EventMustBeAborted 
			ed,
			"Invalid Surface Normal - Geometry must return valid surface normal");
	}

	if(fOldMomentum * fGlobalNormal > 0.0)
	{
		G4ExceptionDescription ed;
		ed 	<< 	" GammaReflectionProcess/PostStepDoIt(): fGlobalNormal points in a "
				"wrong direction. "
			<< G4endl
			<< " The momentum of the photon arriving at interface (oldMomentum)"
			<< " must exit the volume cross in the step. " << G4endl
			<< " So it MUST have dot < 0 with the normal that Exits the new "
			"volume (globalNormal)."
			<< G4endl << "   >> The dot product of oldMomentum and global Normal is "
			<< fOldMomentum * fGlobalNormal << G4endl
			<< "     Old Momentum  (during step)     = " << fOldMomentum << G4endl
			<< "     Global Normal (Exiting New Vol) = " << fGlobalNormal << G4endl
			<< G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection03",
			JustWarning, // or EventMustBeAborted
			ed,
			"Invalid Surface Normal - Geometry must return valid surface "
			"normal pointing in the right direction");
		fGlobalNormal = -fGlobalNormal;
	}

	//- Total internal reflection
	// https://link.springer.com/chapter/10.1007/978-1-4842-7185-8_8
	// For a mirror reflective surface, the reflection formula is r = i–2(i·n)n,
	// where i is the incoming ray direction, n the surface normal, and r the
	// reflection direction. The normal direction is expected to be
	// normalized (equal to a length of one) in this form. The normal can point in 
	// either direction (e.g., upward or downward) and the formula still works. The 
	// incoming ray direction does not have to be normalized, and the outgoing 
	// direction vector will be computed having the same length as the incoming one.
	fNewMomentum = fOldMomentum - 2. * fOldMomentum * fGlobalNormal * fGlobalNormal;

	//- Roughtness
	// old (default) Synrad+ model [https://molflow.web.cern.ch/node/116]
	// From: https://molflow-forum.web.cern.ch/t/unexpected-absorbed-sr-photon-distribution-in-synrad/584/19
	//  First perturbate surface normal according to “roughness ratio”
	//   - If incident shadowing (ray would come from behind), repeat by generating a new random tilt angle
	//  Treat everything else as if the surface was locally flat
	//   - Calculate incident theta with perturbated surface
	//   - Look up reflection probability (function of theta and energy)
	//   - Specularly reflect on perturbated surface
	//   - If outgoing shadowing (going against original surface), 
	//     generate new perturbated surface and do everything again
	G4ThreeVector fGlobalNormal_tmp = fGlobalNormal;
	G4ThreeVector fNewMomentum_tmp = fNewMomentum;

	// get angular offsets and check if the new surface normal and gamma direction are within physical ranges;
	G4int i = 0;
	do
	{
		// In the old (default) Synrad+ reflection model, reflection is specular (mirror-like), 
		// but as surface roughness is taken into account; the theta and phi angles (between 
		// the reflected direction and the surface normal) are perturbated with a gaussian 
		// distributied offset
		G4double thetaOffset 	= atan(fSigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
		G4double phiOffset 	= atan(fSigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
		// where thetaOffset and phiOffset are the random angles added to the calculated ones
		// (those of mirror reflection), and fSigmaRatio is the value the user inputs in the 
		// Synrad+ Material Roughness field (the ratio of the sigmaH and sigmaX values, where 
		// sigmaX is the standard deviation of the size of cavities on the surface, and sigmaH 
		// is the st.dev. of the depth of those). Essentially, the larger the "roughness" value 
		// (theoretically the ratio of RMS roughness and autocorrelation length), the more the 
		// reflection direction is perturbated around the perfect mirror direction.

		fGlobalNormal_tmp.setTheta(fGlobalNormal.getTheta() + thetaOffset);
		fGlobalNormal_tmp.setPhi(fGlobalNormal.getPhi() + phiOffset);
		fGlobalNormal_tmp = fGlobalNormal_tmp.unit(); // normalize: vector parallel to this, but of length 1.

		// Total internal reflection
		fNewMomentum_tmp = fOldMomentum - 2. * fOldMomentum * fGlobalNormal_tmp * fGlobalNormal_tmp;
		fNewMomentum_tmp = fNewMomentum_tmp.unit();

		i++;
	}
	while(	(fGlobalNormal*fGlobalNormal_tmp <= 0.0 || // old and new normals: angle between them should be (0;pi/2) --> dot > 0; 
		fNewMomentum_tmp*fGlobalNormal <= 0.0) && // new outgoing gamma and old normal: angle between them should be (0;pi/2) --> dot > 0;
		i < NMAXLOOPS); // loops

	// set the new offset direction
	if(i < NMAXLOOPS)
	{
		fGlobalNormal = fGlobalNormal_tmp;
		fNewMomentum = fNewMomentum_tmp;
	}
	else
	{
		G4ExceptionDescription ed;
		ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
			<< " Failed to find acceptable angular offsets after "<<NMAXLOOPS<<" trials."<< G4endl
			<< " (fGlobalNormal*fGlobalNormal_tmp) = "<<fGlobalNormal*fGlobalNormal_tmp<<" and " << G4endl
			<< " (fNewMomentum_tmp*fGlobalNormal) = "<<fNewMomentum_tmp*fGlobalNormal<<"," << G4endl
			<< " while the dot products should be > 0." << G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection04", 
			JustWarning, // or EventMustBeAborted 
			ed,
			"Bad angular offsets - use unperturbed surface normal and momentum direction");
	}

	// Calculate Angle between Normal and Photon Momentum
	G4double anglePhotonToNormal = fOldMomentum.angle(-fGlobalNormal);
	G4double grazingPhotonAngle = M_PI_2 - anglePhotonToNormal; 

	// get reflection probability from the database
	if(volName == "vac_IP" || volName == "vac_ip") // Au ip beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Au"],
			_grazingAngleMap["Au"],_photonEnergyMap["Au"],grazingPhotonAngle/rad,GamEner/eV);
	else if(G4StrUtil::contains(volName,"vac_TaggerFoil_")) // Cu tagger foil
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);
	else if(volName == "lumiWind") // Al lumi exit window
		prob = FindReflectionProbability(_reflectDataMap["Al"],
			_grazingAngleMap["Al"],_photonEnergyMap["Al"],grazingPhotonAngle/rad,GamEner/eV);
	else // Cu beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);

	return;
}

void GammaReflectionProcess::DoGammaReflection3(const G4double GamEner, 
						const G4ThreeVector theGlobalPoint,
						const G4ThreeVector fOldMomentum,
						const G4String volName, 
						G4double &prob, 
						G4ThreeVector &fNewMomentum)
{
	G4bool valid;

	// ID of Navigator which limits step
	G4int hNavId = G4ParallelWorldProcess::GetHypNavigatorID();
	auto iNav = G4TransportationManager::GetTransportationManager()->GetActiveNavigatorsIterator();
	//- GetGlobalExitNormal [https://bugzilla-geant4.kek.jp/show_bug.cgi?id=1300]
	// Obtains the Normal vector to a surface (in global coordinates)
	// pointing out of previous volume and into current volume
	G4ThreeVector fGlobalNormal = (iNav[hNavId])->GetGlobalExitNormal(theGlobalPoint, &valid);
	G4double SinIncidentAngle = fOldMomentum * fGlobalNormal;

	// flip the normal vector -- now, pointing inside the vacuum
	if(valid)
	{
		fGlobalNormal = -fGlobalNormal;
	}
	else
	{
		G4ExceptionDescription ed;
		ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
			<< " The Navigator reports that it returned an invalid normal" << G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection02", 
			JustWarning, // or EventMustBeAborted 
			ed,
			"Invalid Surface Normal - Geometry must return valid surface normal");
	}

	if(fOldMomentum * fGlobalNormal > 0.0)
	{
		G4ExceptionDescription ed;
		ed 	<< 	" GammaReflectionProcess/PostStepDoIt(): fGlobalNormal points in a "
				"wrong direction. "
			<< G4endl
			<< " The momentum of the photon arriving at interface (oldMomentum)"
			<< " must exit the volume cross in the step. " << G4endl
			<< " So it MUST have dot < 0 with the normal that Exits the new "
			"volume (globalNormal)."
			<< G4endl << "   >> The dot product of oldMomentum and global Normal is "
			<< fOldMomentum * fGlobalNormal << G4endl
			<< "     Old Momentum  (during step)     = " << fOldMomentum << G4endl
			<< "     Global Normal (Exiting New Vol) = " << fGlobalNormal << G4endl
			<< G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection03",
			JustWarning, // or EventMustBeAborted
			ed,
			"Invalid Surface Normal - Geometry must return valid surface "
			"normal pointing in the right direction");
		fGlobalNormal = -fGlobalNormal;
	}

	// Total internal reflection
	// https://link.springer.com/chapter/10.1007/978-1-4842-7185-8_8
	// For a mirror reflective surface, the reflection formula is r = i–2(i·n)n,
	// where i is the incoming ray direction, n the surface normal, and r the
	// reflection direction. The normal direction is expected to be
	// normalized (equal to a length of one) in this form. The normal can point in 
	// either direction (e.g., upward or downward) and the formula still works. The 
	// incoming ray direction does not have to be normalized, and the outgoing 
	// direction vector will be computed having the same length as the incoming one.
	fNewMomentum = fOldMomentum - 2. * fOldMomentum * fGlobalNormal * fGlobalNormal;
	fNewMomentum = fNewMomentum.unit(); // normalize: vector parallel to this, but of length 1.

	// Calculate Angle between Normal and Photon Momentum
	G4double anglePhotonToNormal = fOldMomentum.angle(-fGlobalNormal);
	G4double grazingPhotonAngle = M_PI_2 - anglePhotonToNormal; 

	// get reflection probability from the database
	if(volName == "vac_IP" || volName == "vac_ip") // Au ip beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Au"],
			_grazingAngleMap["Au"],_photonEnergyMap["Au"],grazingPhotonAngle/rad,GamEner/eV);
	else if(G4StrUtil::contains(volName,"vac_TaggerFoil_")) // Cu tagger foil
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);
	else if(volName == "lumiWind") // Al lumi exit window
		prob = FindReflectionProbability(_reflectDataMap["Al"],
			_grazingAngleMap["Al"],_photonEnergyMap["Al"],grazingPhotonAngle/rad,GamEner/eV);
	else // Cu beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);

	//- Roughtness
	// Debye–Waller factor [https://doi.org/10.1364/OSAC.422924]	
	G4double kiz = SinIncidentAngle * GamEner / CLHEP::hbarc;
	G4double probSpec = G4Exp(-2.0 * pow(kiz * fSurfaceRoughness,2));

	// if not specular
	// https://cds.cern.ch/record/2157666/files/CERN-THESIS-2016-047.pdf
	// https://gitlab.cern.ch/molflow_synrad/synrad/-/blob/master_cli/src/Simulation/Particle.cpp?ref_type=heads
	if(G4UniformRand() > probSpec)
	{
		G4double tau = fAutoCorrLength / fSurfaceRoughness;
		// see Table 2.4 in Ref.[https://cds.cern.ch/record/2157666/files/CERN-THESIS-2016-047.pdf]
		G4double sigmaTheta = (2.9267 / tau) * rad;
		G4double sigmaPhi = ((	2.80657 * pow(anglePhotonToNormal, -1.00238) - 
					1.00293 * pow(anglePhotonToNormal, 1.2266)) / 
					tau) * rad;
		
		G4ThreeVector fNewMomentum_tmp = fNewMomentum;
		// perturb reflected gamma direction and check if it is within physical ranges;
		G4int i = 0;
		do
		{
			// scattering angle = theta
			fNewMomentum_tmp.setTheta(G4RandGauss::shoot(fNewMomentum.getTheta(),sigmaTheta));
			// out-of-plane angle = phi
			fNewMomentum_tmp.setPhi(G4RandGauss::shoot(fNewMomentum.getPhi(),sigmaPhi));	

			i++;
		}
		while(	fNewMomentum_tmp*fGlobalNormal <= 0.0 && // new outgoing gamma and normal: angle between them should be (0;pi/2) --> dot > 0; 
			i < NMAXLOOPS); // loops

		// set the new perturbed direction
		if(i < NMAXLOOPS)
		{
			fNewMomentum = fNewMomentum_tmp;
		}
		else
		{
			G4ExceptionDescription ed;
			ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
				<< " Failed to find acceptable angular offsets after "<<NMAXLOOPS<<" trials."<< G4endl
				<< " (fNewMomentum_tmp*fGlobalNormal) = "<<fNewMomentum_tmp*fGlobalNormal<<"," << G4endl
				<< " while the dot products should be > 0." << G4endl;
			G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection04", 
				JustWarning, // or EventMustBeAborted 
				ed,
				"Bad angular perturbation - use unperturbed momentum direction");
		}
	}

	return;
}

G4double GammaReflectionProcess::Reflectivity(	const G4double GamEner, 
						const G4double SinIncidentAngle, 
						const G4Material* theMat) const
{
	if(GamEner < 30. * eV || GamEner > 30. * keV) return 0;

	G4double theReflectivity = 0;
	const G4MaterialPropertiesTable* theMatProp = theMat->GetMaterialPropertiesTable();
	if (SinIncidentAngle < 0.9 && theMatProp != nullptr)
  	{  	
		// avoid perpendicular refl. at straight entry  and require
    		// data available
    		G4MaterialPropertyVector* RealIndex = theMatProp->GetProperty(kREALRINDEX);
    		G4MaterialPropertyVector* ImagIndex = theMatProp->GetProperty(kIMAGINARYRINDEX);
    		if (nullptr == RealIndex || nullptr == ImagIndex) { return theReflectivity; }
    		const G4double delta = RealIndex->Value(GamEner);
    		const G4double beta = ImagIndex->Value(GamEner);
    		const G4double sin2 = pow(SinIncidentAngle, 2);
    		const G4double rho2 = 0.5 * (sin2 - 2 * delta + sqrt(pow(sin2 - 2 * delta, 2) + 4 * beta * beta));
    		const G4double rho = sqrt(rho2);
    		const G4double Refl_sigma = 	(rho2 * pow(SinIncidentAngle - rho, 2) + pow(beta, 2)) / 
						(rho2 * pow(SinIncidentAngle + rho, 2) + pow(beta, 2));
    		const G4double coscot = sqrt(1 - sin2) / SinIncidentAngle;
    		const G4double pi_over_sigma = 	(rho2 * pow(rho - coscot, 2) + pow(beta, 2)) / 
						(rho2 * pow(rho + coscot, 2) + pow(beta, 2));
    		const G4double Refl_pi = Refl_sigma * pi_over_sigma;
    		theReflectivity = 0.5 * (Refl_sigma + Refl_pi);  // unpolarized
    		G4double RoughAtten = 1;
    		if (fSurfaceRoughness > 0) 
		{
      			G4double kiz = SinIncidentAngle * GamEner / CLHEP::hbarc;
      			G4double kjz = SinIncidentAngle * (1.0 - delta) * GamEner / CLHEP::hbarc;

			// Nevot–Croce [https://doi.org/10.1016/S0076-695X(08)60047-2]
      			RoughAtten = G4Exp(-2.0 * kiz * kjz * fSurfaceRoughness * fSurfaceRoughness); 
      			theReflectivity *= RoughAtten;
    		}
  	}

  	return theReflectivity;
}

void GammaReflectionProcess::SaveHenkeDataAsMaterialProperty(G4bool print)
{
  	// loop through the material table and load set up MaterialPropertiesTable
  	// with Henke data used to calculate the reflection
  	auto materialTable = G4Material::GetMaterialTable();
  	for (auto a_material : *materialTable) 
	{
    		auto N = a_material->GetTotNbOfAtomsPerVolume();
    		// calculate the reflectivity from input data. Implemented for dense
    		// materials of a single element
    		if (a_material->GetNumberOfElements() == 1 && a_material->GetDensity() > 1) 
		{
      			G4double factor = N * CLHEP::classic_electr_radius / CLHEP::twopi;
      			vector<G4double> Ephot, f1, f2;
      			const G4Element* theElement = a_material->GetElement(0);
      			G4int iret = ReadHenkeXrayData(theElement->GetName(), Ephot, f1, f2);
      			if (iret) 
			{
          			G4cout << left << setw(12) << __FILE__ << " " << __FUNCTION__ << " line "
                 		<< right << setw(4) << __LINE__ << " no Henke data found for "
                 		<< a_material->GetName() << " " << theElement->GetName() << G4endl;
      			}
      			else 
			{
        			vector<G4double> RealIndex(Ephot.size()), ImagIndex(Ephot.size());
        			for (size_t i = 0; i < Ephot.size(); ++i) 
				{
          				G4double lambda = CLHEP::twopi * CLHEP::hbarc / Ephot[i];
          				G4double lambda_sqr = lambda * lambda;
          				RealIndex[i] = fmax(0, factor * lambda_sqr * f1[i]);  // delta or 1-RealIndex
          				ImagIndex[i] = factor * lambda_sqr * f2[i];  // beta  or  -ImagIndex
            			
					if(print)
						G4cout << "Ephot=" << setw(10) << Ephot[i] / eV << " eV delta=" 
						<< setw(10) << RealIndex[i] << " beta=" << setw(10) 
						<< ImagIndex[i] << G4endl;
        			}  // photon energy

        			G4MaterialPropertiesTable* proptab = a_material->GetMaterialPropertiesTable();
        			if(proptab == nullptr) 
				{
          				proptab = new G4MaterialPropertiesTable();
          				a_material->SetMaterialPropertiesTable(proptab);
        			}
        			proptab->AddProperty("REALRINDEX", Ephot, RealIndex);  // 1-RealIndex
        			proptab->AddProperty("IMAGINARYRINDEX", Ephot, ImagIndex);
          			
				G4cout << left << setw(12) << __FILE__ << " " << __FUNCTION__ << " line "
                 		<< right << setw(4) << __LINE__ << " " << a_material->GetName()
                 		<< " " << theElement->GetName()
                 		<< " reflection data saved in PropertiesTable\n" << G4endl;
      			}  // data found
    		}
  	}

	return;
}

G4int GammaReflectionProcess::ReadHenkeXrayData(string ElName, 
						vector<G4double>& Ephot, 
						vector<G4double>& f1, 
						vector<G4double>& f2)
{
	// henke_physical_reference uses lower case filenames
  	transform(ElName.begin(), ElName.end(), ElName.begin(),::tolower);

  	const G4String DataDir = "../materials/henke_physical_reference/";
  	const G4String InpFname = DataDir + ElName + ".nff";
  	ifstream infile(InpFname);
  	if (!infile.is_open()) 
	{
    		G4cout << "ReadHenkeXrayReflData " << InpFname << " not found" << G4endl;
    		return 1;  // failure
  	}
  	vector<string> VarName(3);
  	infile >> VarName[0] >> VarName[1] >> VarName[2];
    
	G4cout << "ReadHenkeXrayData variable names " << VarName[0] << " " << VarName[1] << " "
 	<< VarName[2] << G4endl;
  
	G4double E_eV_i, f1_i, f2_i;
  	Ephot.resize(0);
  	f1.resize(0);
  	f2.resize(0);
  	for (;;) 
	{
    		infile >> E_eV_i >> f1_i >> f2_i;
    		if (infile.eof()) break;
    		Ephot.push_back(E_eV_i * eV);
    		f1.push_back(f1_i);
    		f2.push_back(f2_i);
  	}

  	return 0;  // success
}

const G4String &GammaReflectionProcess::GetName() const 
{
	return theProcessName;
}


G4double GammaReflectionProcess::AlongStepGetPhysicalInteractionLength(	const G4Track&,
									G4double,
									G4double,
									G4double&,
									G4GPILSelection*) 
{
  	return -1.0;
}

G4double GammaReflectionProcess::AtRestGetPhysicalInteractionLength(const G4Track&,G4ForceCondition*) 
{
  	return -1.0;
}

G4VParticleChange* GammaReflectionProcess::AtRestDoIt(const G4Track&,const G4Step&) 
{
  	return 0;
}

G4VParticleChange* GammaReflectionProcess::AlongStepDoIt(const G4Track&,const G4Step&) 
{
  	return 0;
}

void GammaReflectionProcess::PrintReflectData(	string name, 
						vector<pair<double, 
						vector<double>>> dataset,
						vector<double> angle, 
						vector<double> energy)
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	G4cout<<"[INFO] GammaReflectionProcess::GammaReflectionProcess ==> "<<name<<" data"<<G4endl;

	// print column names
	printf("Angl/Ene : ");
	for(int j = 0; j < angle.size(); ++j){printf("%.2e  ",angle.at(j));}
	printf("\n");

	printf("Angl/Ene : ");
    	for(int j = 0; j < dataset.size(); ++j)
    	{
        	printf("%.2e  ",dataset.at(j).first);
    	}
	printf("\n");

	// print data
    	for(int i = 0; i < dataset.at(0).second.size(); ++i)
    	{
		printf("%.2e  : ",energy.at(i));
        	for(int j = 0; j < dataset.size(); ++j)
        	{
           		printf("%.2e  ",dataset.at(j).second.at(i));
        	}
		printf("\n");
    	}
	G4cout<<"==================================================================\n\n"<<G4endl;

	return;
}

vector<pair<double, vector<double>>> GammaReflectionProcess::ReadReflectData(	string filename,
										vector<double> &angle, 
										vector<double> &energy)
{
	// Reads a CSV file into a vector of <string, vector<double>> pairs where
	// each pair represents <column name, column values>

	vector<pair<double, vector<double>>> dataset;

	// Create an input filestream
    	ifstream myFile(filename);

    	// make sure the file is open
    	if(!myFile.is_open()) 
	{
		G4cout<<"\n\n=================================================================="<<G4endl;
		G4cout<<"[INFO] GammaReflectionProcess::ReadReflectData"<<G4endl;
		G4cout<<"material reflection data file name = "<<filename<<G4endl;
		G4cout<<"==================================================================\n\n"<<G4endl;
		throw runtime_error("[ERROR] GammaReflectionProcess::ReadReflectData ==> Cannot open or find the file\n");
	}

    	// Helper vars
    	string line, colname;
    	double val;

    	// Read the column names
    	if(myFile.good())
    	{
        	// Extract the first line in the file
        	getline(myFile, line);

        	// Create a stringstream from line
        	stringstream ss(line);

        	// Extract each column name
		getline(ss, colname, ','); // skip the first column
        	while(getline(ss, colname, ','))
		{
			val = stod(colname);
			// Initialize and add <colname, double vector> pairs to dataset
            		dataset.push_back({val, vector<double> {}});
			angle.push_back(val);
        	}
    	}

    	// Read data, line by line
    	while(getline(myFile, line))
    	{
        	// Create a stringstream of the current line
        	stringstream ss(line);
        
        	// Keep track of the current column index
        	int colIdx = 0;
        
        	// Extract each value
        	while(ss >> val)
		{
            		// Add the current integer to the 'colIdx' column's values vector
            		if(colIdx == 0) energy.push_back(val);
			else dataset.at(colIdx-1).second.push_back(val);
            
            		// If the next token is a comma, ignore it and move on
            		if(ss.peek() == ',') ss.ignore();
            
            		// Increment the column index
            		colIdx++;
        	}
    	}

    	// Close the file
    	myFile.close();

	return dataset;
}

void GammaReflectionProcess::FindClosestIndexesInVec(vector<double> vec, double val, int &id1, int &id2)
{
	id1 = id2 = 0;
	// find the index of the element of the vecotr that is nearest-above to val
	auto i = lower_bound(vec.begin(), vec.end(), val); 
	// if the vector values are in decreasing order use:
	//auto i = lower_bound(vec.rbegin(), vec.rend(), val);
	id2 = i - vec.begin(); // nearest index

	if (i == vec.end())
  	{
		--id2;  // extrapolating above
	}
	else if (*i == val)
	{
		id1 = id2;
  		return;
	}

	id1 = id2 ? id2 - 1 : id2; //nearest-below index, except when extrapolating downward
	id1 = (val > vec.at(id2)) ? id2 : id1; //outside the data range

	return;
}

double GammaReflectionProcess::FindReflectionProbability(	vector<pair<double, vector<double>>> dataset,
								vector<double> angle, vector<double> energy, 
								double angle_val, double energy_val)
{
	int ang_i, ang_j, ene_i, ene_j;

	// find the closest data in the table
	FindClosestIndexesInVec(angle,angle_val,ang_i,ang_j);
	FindClosestIndexesInVec(energy,energy_val,ene_i,ene_j);

	// get probabilities
	double prob_ii = dataset.at(ang_i).second.at(ene_i);
	double prob_ji = dataset.at(ang_j).second.at(ene_i);
	double prob_ij = dataset.at(ang_i).second.at(ene_j);
	double prob_jj = dataset.at(ang_j).second.at(ene_j);

	// linear interpolation between two points

	// interbolate between ene_i and ene_j; at ang_i
	double k1 = (prob_ij - prob_ii)/(energy.at(ene_j) - energy.at(ene_i));
	double b1 = prob_ii - k1*energy.at(ene_i);
	double prob1 = (energy.at(ene_j) == energy.at(ene_i)) ? prob_ii : k1*energy_val + b1;

	// interbolate between ene_i and ene_j; at ang_j
	double k2 = (prob_jj - prob_ji)/(energy.at(ene_j) - energy.at(ene_i));
	double b2 = prob_ji - k2*energy.at(ene_i);
	double prob2 = (energy.at(ene_j) == energy.at(ene_i)) ? prob_ji : k2*energy_val + b2;

	// interpolate between ang_i and ang_j
	double k = (prob2 - prob1)/(angle.at(ang_j) - angle.at(ang_i));
	double b = prob1 - k*angle.at(ang_i);
	double prob = (angle.at(ang_j) == angle.at(ang_i)) ? prob1 : k*angle_val + b;

	return prob;
}
