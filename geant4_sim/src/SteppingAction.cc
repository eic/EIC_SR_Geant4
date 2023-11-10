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
	G4StepPoint*            preStep         = step->GetPreStepPoint();
	G4StepPoint*            postStep        = step->GetPostStepPoint();
	G4VPhysicalVolume*      prePV           = preStep->GetPhysicalVolume();
	G4VPhysicalVolume*      postPV          = postStep->GetPhysicalVolume();

	if(     postStep->GetStepStatus() == fGeomBoundary && 
		G4StrUtil::contains(prePV->GetName(),"vac_") &&
		postPV->GetName() == "mag_Q1EF")
	{
		runAction->_monPosX = postStep->GetPosition().getX()/cm;
		runAction->_monPosY = postStep->GetPosition().getY()/cm;
		runAction->_monPosZ = postStep->GetPosition().getZ()/cm;
		runAction->_monMomX = postStep->GetMomentum().getX()/eV;
		runAction->_monMomY = postStep->GetMomentum().getY()/eV;
		runAction->_monMomZ = postStep->GetMomentum().getZ()/eV;

		G4cout<<"x [cm] = "<<postStep->GetPosition().getX()/cm<<G4endl;
		G4cout<<"y [cm] = "<<postStep->GetPosition().getY()/cm<<G4endl;
		G4cout<<"z [cm] = "<<postStep->GetPosition().getZ()/cm<<G4endl;

		G4Track* aNonConstTrack = const_cast<G4Track*>(track);
		aNonConstTrack->SetTrackStatus(fStopAndKill);
	}
	//--------------------------------------------------------------------------//

	// kill long living tracks
	if(track->GetGlobalTime() > simPar->GetTrackTcut())
	{
		G4Track* aNonConstTrack = const_cast<G4Track*>(track);
		aNonConstTrack->SetTrackStatus(fStopAndKill);
	}

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
