#include "GammaReflectionProcess.hh"
#include "G4VParticleChange.hh"
#include "G4VSolid.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParallelWorldProcess.hh"
#include "G4TransportationManager.hh"
#include "G4Exp.hh"

#define NMAXLOOPS 100 // the maximum number of loops to find acceptable angular offsets

G4double GammaReflectionProcess::fSurfaceRoughness = 50e-9 * m;

GammaReflectionProcess::GammaReflectionProcess() : G4VProcess("GammaReflectionProcess")
{
	//------------------- material properties -------------------//
	_reflectFileNameMap["Cu"] = "../materials/02-Copper.csv";
	_reflectFileNameMap["Au"] = "../materials/04-Gold.csv";
	_sigmaRatio = 0.005;
	//-----------------------------------------------------------//

	_reflectDataMap["Cu"] = ReadReflectData(_reflectFileNameMap["Cu"],_grazingAngleMap["Cu"],_photonEnergyMap["Cu"]);
	_reflectDataMap["Au"] = ReadReflectData(_reflectFileNameMap["Au"],_grazingAngleMap["Au"],_photonEnergyMap["Au"]);

	PrintReflectData("Cu",_reflectDataMap["Cu"],_grazingAngleMap["Cu"],_photonEnergyMap["Cu"]);
	PrintReflectData("Au",_reflectDataMap["Au"],_grazingAngleMap["Au"],_photonEnergyMap["Au"]);


	fNistManager = G4NistManager::Instance();
	theMatIP = fNistManager->FindOrBuildMaterial("G4_Au");
	theMatIR = fNistManager->FindOrBuildMaterial("G4_Cu");
	SaveHenkeDataAsMaterialProperty();
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

	// only when leaving the magnet or vacuum and entering the world
	if(	postStep->GetStepStatus() == fGeomBoundary && postPV->GetName() == "world" &&
		(G4StrUtil::contains(prePV->GetName(),"mag_") || G4StrUtil::contains(prePV->GetName(),"vac_")))
	{
		const G4DynamicParticle* aParticle = track.GetDynamicParticle(); // get the photon

		G4ThreeVector fOldMomentum = aParticle->GetMomentumDirection(); // photon momentum direction
		G4ThreeVector fNewMomentum = fOldMomentum; // new momentum direction after reflection
		G4double GamEner = aParticle->GetTotalEnergy(); // photon energy
		G4ThreeVector theGlobalPoint = step.GetPostStepPoint()->GetPosition(); // reflection point
		G4double prob = 0; // reflection probability


		// Based on new Geant4 implementation of 'G4XrayReflection'
		// Ref.[https://geant4.web.cern.ch/download/11.2.0.html]
		// Uses data from Ref.[https://doi.org/10.1006/adnd.1993.1013]
		DoXrayReflection(GamEner,prob,fNewMomentum);

		// Based on Synrad+ implementation
		// Ref.[https://molflow.web.cern.ch/node/116]
		// Uses data from Ref.[https://doi.org/10.1006/adnd.1993.1013]
		DoGammaReflection(GamEner,prob,fNewMomentum);

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

void GammaReflectionProcess::DoXrayReflection(const G4double GamEner, G4double &prob, G4ThreeVector &fNewMomentum)
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
	if(prePV->GetName() == "vac_IP" || prePV->GetName() == "vac_ip") // Au ip beam pipe
		prob = Reflectivity(GamEner, SinIncidentAngle, theMatIP);
	else // Cu beam pipe
		prob = Reflectivity(GamEner, SinIncidentAngle, theMatIR);

	return;
}

void GammaReflectionProcess::DoGammaReflection(const G4double GamEner, G4double &prob, G4ThreeVector &fNewMomentum)
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
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection01", 
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
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection02",
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
	G4ThreeVector fNewMomentum = fOldMomentum - 2. * fOldMomentum * fGlobalNormal * fGlobalNormal;
	fNewMomentum = fNewMomentum.unit(); // normalize: vector parallel to this, but of length 1.

	// Calculate Angle between Normal and Photon Momentum
	G4double anglePhotonToNormal = fOldMomentum.angle(-fGlobalNormal);
	G4double grazingPhotonAngle = M_PI_2 - anglePhotonToNormal; 

	// get probability
	if(prePV->GetName() == "vac_IP" || prePV->GetName() == "vac_ip") // Au ip beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Au"],
			_grazingAngleMap["Au"],_photonEnergyMap["Au"],grazingPhotonAngle/rad,GamEner/eV);
	else // Cu beam pipe
		prob = FindReflectionProbability(_reflectDataMap["Cu"],
			_grazingAngleMap["Cu"],_photonEnergyMap["Cu"],grazingPhotonAngle/rad,GamEner/eV);

	//- Roughtness
	G4ThreeVector fNewMomentum_tmp = fNewMomentum;

	// get angular offsets and
	// check if the new reflected gamma direction is within physical ranges;
	// since the norm vector looks inside the vacuum, the angle between them should be (0;pi/2) --> dot > 0; 
	G4int i = 0;
	do
	{
		// In the old (default) Synrad+ reflection model, reflection is specular (mirror-like), 
		// but as surface roughness is taken into account; the theta and phi angles (between 
		// the reflected direction and the surface normal) are perturbated with a gaussian 
		// distributied offset
		G4double thetaOffset 	= atan(_sigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
		G4double phiOffset 	= atan(_sigmaRatio*tan(M_PI*(G4UniformRand() - 0.5)));
		// where thetaOffset and phiOffset are the random angles added to the calculated ones
		// (those of mirror reflection), and _sigmaRatio is the value the user inputs in the 
		// Synrad+ Material Roughness field (the ratio of the sigmaH and sigmaX values, where 
		// sigmaX is the standard deviation of the size of cavities on the surface, and sigmaH 
		// is the st.dev. of the depth of those). Essentially, the larger the "roughness" value 
		// (theoretically the ratio of RMS roughness and autocorrelation length), the more the 
		// reflection direction is perturbated around the perfect mirror direction.

		fNewMomentum_tmp.setTheta(fNewMomentum.getTheta() + thetaOffset);
		fNewMomentum_tmp.setPhi(fNewMomentum.getPhi() + phiOffset);
		i++;
	}
	while(fNewMomentum_tmp*fGlobalNormal <= 0.0 && i < NMAXLOOPS);

	// set the new offset direction
	if(i < NMAXLOOPS)
	{
		fNewMomentum = fNewMomentum_tmp;
	}
	else
	{
		G4ExceptionDescription ed;
		ed 	<< " GammaReflectionProcess/PostStepDoIt(): "
			<< " Failed to find acceptable angular offsets after "<<NMAXLOOPS<<" trials."<< G4endl
			<< " (fNewMomentum_tmp*fGlobalNormal) = "<<fNewMomentum_tmp*fGlobalNormal<<"." << G4endl
			<< " while the dot product should be > 0." << G4endl;
		G4Exception("GammaReflectionProcess::PostStepDoIt", "GammaReflection01", 
			JustWarning, // or EventMustBeAborted 
			ed,
			"Bad angular offsets - use unperturbed reflection photon direction");
	}

	return;
}

G4double GammaReflectionProcess::Reflectivity(const G4double GamEner, const G4double SinIncidentAngle, const G4Material* theMat) const
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
    		const G4double sin2 = std::pow(SinIncidentAngle, 2);
    		const G4double rho2 = 0.5 * (sin2 - 2 * delta + std::sqrt(std::pow(sin2 - 2 * delta, 2) + 4 * beta * beta));
    		const G4double rho = std::sqrt(rho2);
    		const G4double Refl_sigma = (rho2 * std::pow(SinIncidentAngle - rho, 2) + std::pow(beta, 2)) / (rho2 * std::pow(SinIncidentAngle + rho, 2) + std::pow(beta, 2));
    		const G4double coscot = std::sqrt(1 - sin2) / SinIncidentAngle;
    		const G4double pi_over_sigma = (rho2 * std::pow(rho - coscot, 2) + std::pow(beta, 2)) / (rho2 * std::pow(rho + coscot, 2) + std::pow(beta, 2));
    		const G4double Refl_pi = Refl_sigma * pi_over_sigma;
    		theReflectivity = 0.5 * (Refl_sigma + Refl_pi);  // unpolarized
    		G4double RoughAtten = 1;
    		if (fSurfaceRoughness > 0) 
		{
      			G4double kiz = SinIncidentAngle * GamEner / CLHEP::hbarc;
      			G4double kjz = SinIncidentAngle * (1 - delta) * GamEner / CLHEP::hbarc;
      			RoughAtten = G4Exp(-2 * kiz * kjz * fSurfaceRoughness * fSurfaceRoughness);  // Nevot–Croce
      			theReflectivity *= RoughAtten;
    		}
  	}
  	return theReflectivity;
}

void GammaReflectionProcess::SaveHenkeDataAsMaterialProperty()
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
      			std::vector<G4double> Ephot, f1, f2;
      			const G4Element* theElement = a_material->GetElement(0);
      			G4int iret = ReadHenkeXrayData(theElement->GetName(), Ephot, f1, f2);
      			if (iret) 
			{
          			G4cout << std::left << std::setw(12) << __FILE__ << " " << __FUNCTION__ << " line "
                 		<< std::right << std::setw(4) << __LINE__ << " no Henke data found for "
                 		<< a_material->GetName() << " " << theElement->GetName() << G4endl;
      			}
      			else 
			{
        			std::vector<G4double> RealIndex(Ephot.size()), ImagIndex(Ephot.size());
        			for (std::size_t i = 0; i < Ephot.size(); ++i) 
				{
          				G4double lambda = CLHEP::twopi * CLHEP::hbarc / Ephot[i];
          				G4double lambda_sqr = lambda * lambda;
          				RealIndex[i] = fmax(0, factor * lambda_sqr * f1[i]);  // delta or 1-RealIndex
          				ImagIndex[i] = factor * lambda_sqr * f2[i];  // beta  or  -ImagIndex
            			
					G4cout << "Ephot=" << std::setw(10) << Ephot[i] / eV << " eV delta=" << std::setw(10)
                   			<< RealIndex[i] << " beta=" << std::setw(10) << ImagIndex[i] << G4endl;
        			}  // photon energy
        			G4MaterialPropertiesTable* proptab = a_material->GetMaterialPropertiesTable();
        			if(proptab == nullptr) 
				{
          				proptab = new G4MaterialPropertiesTable();
          				a_material->SetMaterialPropertiesTable(proptab);
        			}
        			proptab->AddProperty("REALRINDEX", Ephot, RealIndex);  // 1-RealIndex
        			proptab->AddProperty("IMAGINARYRINDEX", Ephot, ImagIndex);
          			
				G4cout << std::left << std::setw(12) << __FILE__ << " " << __FUNCTION__ << " line "
                 		<< std::right << std::setw(4) << __LINE__ << " " << a_material->GetName()
                 		<< " " << theElement->GetName()
                 		<< " reflection data saved in PropertiesTable" << G4endl;
      			}  // data found
    		}
  	}

	return;
}

G4int GammaReflectionProcess::ReadHenkeXrayData(std::string ElName, std::vector<G4double>& Ephot, std::vector<G4double>& f1, std::vector<G4double>& f2)
{
  	std::transform(ElName.begin(), ElName.end(), ElName.begin(),::tolower);  // henke_physical_reference uses lower case filanames
  	const G4String DataDir = "../materials/henke_physical_reference/";
  	const G4String InpFname = DataDir + ElName + ".nff";
  	std::ifstream infile(InpFname);
  	if (!infile.is_open()) 
	{
    		G4cout << "ReadHenkeXrayReflData " << InpFname << " not found" << G4endl;
    		return 1;  // failure
  	}
  	std::vector<std::string> VarName(3);
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


G4double GammaReflectionProcess::AlongStepGetPhysicalInteractionLength(const G4Track&,G4double,G4double,G4double&,G4GPILSelection*) 
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

void GammaReflectionProcess::PrintReflectData(string name, vector<pair<double, vector<double>>> dataset,
		vector<double> angle, vector<double> energy)
{
	G4cout<<"\n\n=================================================================="<<G4endl;
	cout<<"[INFO] GammaReflectionProcess::GammaReflectionProcess ==> "<<name<<" data"<<endl;

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

vector<pair<double, vector<double>>> GammaReflectionProcess::ReadReflectData(string filename,
	vector<double> &angle, vector<double> &energy)
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

double GammaReflectionProcess::FindReflectionProbability(vector<pair<double, vector<double>>> dataset,
								vector<double> angle, vector<double> energy, 
								double angle_val, double energy_val)
{
	int ang_i, ang_j, ene_i, ene_j;

	FindClosestIndexesInVec(angle,angle_val,ang_i,ang_j);
	FindClosestIndexesInVec(energy,energy_val,ene_i,ene_j);

	double prob_ii = dataset.at(ang_i).second.at(ene_i);
	double prob_ji = dataset.at(ang_j).second.at(ene_i);
	double prob_ij = dataset.at(ang_i).second.at(ene_j);
	double prob_jj = dataset.at(ang_j).second.at(ene_j);

	// linear interpolation between two points
	double prob_ii_ji = (ang_i == ang_j) ? prob_ii : 
		(prob_ji - prob_ii)*(angle_val - angle.at(ang_i))/(angle.at(ang_j) - angle.at(ang_i)) + prob_ii;
	double prob_ij_jj = (ang_i == ang_j) ? prob_jj : 
		(prob_jj - prob_ij)*(angle_val - angle.at(ang_j))/(angle.at(ang_i) - angle.at(ang_j)) + prob_ij;

	double prob = (ene_i == ene_j) ? prob_ii_ji : 
		(prob_ij_jj - prob_ii_ji)*(energy_val - energy.at(ene_i))/(energy.at(ene_j) - energy.at(ene_i)) + 
		prob_ii_ji;

	return prob;
}
