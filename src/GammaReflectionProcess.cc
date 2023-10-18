#include "GammaReflectionProcess.hh"
#include "G4VParticleChange.hh"
#include "G4VSolid.hh"

GammaReflectionProcess::GammaReflectionProcess() : G4VProcess("GammaReflectionProcess")
{}

GammaReflectionProcess::~GammaReflectionProcess()
{}

G4double GammaReflectionProcess::PostStepGetPhysicalInteractionLength(const G4Track&,G4double,G4ForceCondition* condition)
{
  	*condition = Forced;
  	return DBL_MAX;
}
  
G4VParticleChange * 
GammaReflectionProcess::PostStepDoIt(const G4Track& track, const G4Step & step)
{
  	aParticleChange.Initialize(track);

    	G4StepPoint*          	preStep		= step.GetPreStepPoint();
    	G4StepPoint*          	postStep	= step.GetPostStepPoint();
    	G4VPhysicalVolume*    	prePV		= preStep->GetPhysicalVolume();
    	G4VPhysicalVolume*	postPV		= postStep->GetPhysicalVolume();

	// check if the next step is not outside the world and skip everything except for gamma
	if(!postPV || track.GetParticleDefinition()->GetPDGEncoding() != 22)
		return &aParticleChange;

	// get tracks at the edge of the vacuum beam pipe
	if(	postStep->GetStepStatus() == fGeomBoundary && postPV->GetName() == "world" &&
		(G4StrUtil::contains(prePV->GetName(),"mag_") || G4StrUtil::contains(prePV->GetName(),"vac_")))
	{
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

		G4double gammaEnergy_eV = track.GetTotalEnergy()/CLHEP::eV; // MeV --> eV
		G4ThreeVector gammaDir_i = postStep->GetMomentum()/postStep->GetMomentum().mag(); 
		G4ThreeVector stepPos = postStep->GetPosition();

		const G4VTouchable* touchable = preStep->GetTouchable();
		const G4RotationMatrix* rotation = touchable->GetRotation();
		G4RotationMatrix rotation_inv = rotation->inverse();
		G4ThreeVector translation = touchable->GetTranslation();
		G4VSolid* solid = touchable->GetSolid();
		G4String name = touchable->GetVolume()->GetName();
		G4ThreeVector posLoc = *rotation * (stepPos - translation);
		// get normal to the surface (gamma is passing outward from the volume of interest to the world)
		G4ThreeVector norm = solid->SurfaceNormal(posLoc); 
		G4ThreeVector gammDirLoc = *rotation * gammaDir_i;
		G4double incidentAngle = acos( norm.dot(gammDirLoc) );
		G4double grazingAngle = M_PI_2 - incidentAngle; 

		G4ThreeVector normGlob = rotation_inv*norm + translation;

		//- Reflection
		// https://link.springer.com/chapter/10.1007/978-1-4842-7185-8_8
		// For a mirror reflective surface, the reflection formula is r = i–2(i·n)n,
		// where i is the incoming ray direction, n the surface normal, and r the
		// reflection direction. The normal direction is expected to be
		// normalized (equal to a length of one) in this form. The normal can point in 
		// either direction (e.g., upward or downward) and the formula still works. The 
		// incoming ray direction does not have to be normalized, and the outgoing 
		// direction vector will be computed having the same length as the incoming one.

		G4ThreeVector gammaDir_r = gammaDir_i - 2.0* normGlob.dot(gammaDir_i) *normGlob;
		// check that the half-angle between reflected and incident angles is equal to the grazing angle
		if(abs(grazingAngle - acos(gammaDir_i.dot(gammaDir_r))/2.)/grazingAngle > 1e-9)
			throw std::runtime_error("\n\n[ERROR] SteppingAction::UserSteppingAction ==> Wrong reflection direction\n");


		G4cout<<"trackID = "<<track.GetTrackID()<<G4endl;
		G4cout<<"name = "<<name<<G4endl;
		G4cout<<"incidentAngle = "<<incidentAngle<<G4endl;
		G4cout<<"grazingAngle = "<<grazingAngle<<G4endl;
		G4cout<<"norm = "<<norm.x()<<";"<<norm.y()<<";"<<norm.z()<<G4endl;
		G4cout<<"normGlob = "<<normGlob.x()<<";"<<normGlob.y()<<";"<<normGlob.z()<<G4endl;
		G4cout<<"gammaDir_i = "<<gammaDir_i.x()<<";"<<gammaDir_i.y()<<";"<<gammaDir_i.z()<<G4endl;
		G4cout<<"gammaDir_r = "<<gammaDir_r.x()<<";"<<gammaDir_r.y()<<";"<<gammaDir_r.z()<<G4endl;
		G4cout<<"stepPos = "<<stepPos.x()<<";"<<stepPos.y()<<";"<<stepPos.z()<<G4endl;

		G4cout<<G4endl;
		
		if(G4UniformRand() > 0.5)
			aParticleChange.ProposeMomentumDirection(gammaDir_r);
		else
    			aParticleChange.ProposeTrackStatus(fStopAndKill);
	}

  	return &aParticleChange;
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

