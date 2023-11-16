#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VParticleChange.hh"
#include "G4ParticleChange.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4Transform3D.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

SteppingAction::SteppingAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintTrackTcut();
}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
	const G4Track* track = step->GetTrack();

	//--------------------------------------------------------------------------//
	//- Beam monitor at the last magnet
	G4StepPoint*            preStep         = step->GetPreStepPoint();
	G4StepPoint*            postStep        = step->GetPostStepPoint();
	G4VPhysicalVolume*      prePV           = preStep->GetPhysicalVolume();
	G4VPhysicalVolume*      postPV          = postStep->GetPhysicalVolume();

	// the monitor is associated with the magnet at the given ID [0;GetMagNum()-1]
	G4int iMag = simPar->GetMagNum() - 1;
	// check the track
	if(     track->GetTrackID() == 1 && // primary particle 
		postStep->GetStepStatus() == fGeomBoundary && // at boundary
		G4StrUtil::contains(postPV->GetName(),"vac_") && // entering vacuum
		prePV->GetName() == "mag_"+simPar->GetMagName(iMag)) // leaving the magnet
	{
		// get particle coordinates
		runAction->_monName = prePV->GetName();
		runAction->_monPosX = postStep->GetPosition().getX()/cm;
		runAction->_monPosY = postStep->GetPosition().getY()/cm;
		runAction->_monPosZ = postStep->GetPosition().getZ()/cm;
		runAction->_monMomX = postStep->GetMomentum().getX()/eV;
		runAction->_monMomY = postStep->GetMomentum().getY()/eV;
		runAction->_monMomZ = postStep->GetMomentum().getZ()/eV;

		// start of the beam vector between the two magnets
		G4ThreeVector magVector1(	simPar->GetMagEndPosX(iMag) - simPar->GetIpPosX(),
						simPar->GetMagEndPosY(iMag) - simPar->GetIpPosY(),
						simPar->GetMagEndPosZ(iMag) - simPar->GetIpPosZ());

		// end of the beam vector between the two magnets 
		iMag = (iMag < simPar->GetMagNum() - 1) ? iMag + 1 : iMag;
		G4ThreeVector magVector2(	simPar->GetMagStartPosX(iMag) - simPar->GetIpPosX(),
						simPar->GetMagStartPosY(iMag) - simPar->GetIpPosY(),
						simPar->GetMagStartPosZ(iMag) - simPar->GetIpPosZ());

		// rotate coordinate vectors from global to local/ip system
		magVector1.rotateY(-simPar->GetIpTheta());
		magVector2.rotateY(-simPar->GetIpTheta());
		
		// get monitor coordinates
		runAction->_monRefPosX = magVector2.x()/cm;
        	runAction->_monRefPosY = magVector2.y()/cm;
        	runAction->_monRefPosZ = magVector2.z()/cm;

		// orientation angle of the beam/monitor
		runAction->_monRefAngle = (magVector2.z() != magVector1.z()) ? 
			atan((magVector2.x()-magVector1.x())/(magVector2.z()-magVector1.z())) : 0;
	}
	//--------------------------------------------------------------------------//

	// check if the track (=gamma) is killed
	if(track->GetTrackStatus() == fStopAndKill && 
		track->GetParticleDefinition()->GetPDGEncoding() == G4Gamma::Gamma()->GetPDGEncoding())
	{	
		// do not store in absorbers
		if(G4StrUtil::contains(step->GetPostStepPoint()->GetPhysicalVolume()->GetName(),"abs_")){return;}

		runAction->_gammaVtxPosX.push_back(track->GetVertexPosition().getX()/cm);
		runAction->_gammaVtxPosY.push_back(track->GetVertexPosition().getY()/cm);
		runAction->_gammaVtxPosZ.push_back(track->GetVertexPosition().getZ()/cm);
		runAction->_gammaVtxDirX.push_back(track->GetVertexMomentumDirection().getX());
		runAction->_gammaVtxDirY.push_back(track->GetVertexMomentumDirection().getY());
		runAction->_gammaVtxDirZ.push_back(track->GetVertexMomentumDirection().getZ());

		runAction->_gammaPosX.push_back(step->GetPostStepPoint()->GetPosition().getX()/cm);
		runAction->_gammaPosY.push_back(step->GetPostStepPoint()->GetPosition().getY()/cm);
		runAction->_gammaPosZ.push_back(step->GetPostStepPoint()->GetPosition().getZ()/cm);
		runAction->_gammaMomX.push_back(step->GetPostStepPoint()->GetMomentum().getX()/eV);
		runAction->_gammaMomY.push_back(step->GetPostStepPoint()->GetMomentum().getY()/eV);
		runAction->_gammaMomZ.push_back(step->GetPostStepPoint()->GetMomentum().getZ()/eV);
		runAction->_gammaEnergy.push_back(track->GetTotalEnergy()/eV);
		runAction->_gammaTime.push_back(track->GetGlobalTime()/ms); // time from the start of the event
	}

	return;
}
