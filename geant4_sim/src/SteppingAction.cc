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

SteppingAction::SteppingAction(RunAction* runAct) : runAction(runAct)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
	const G4Track* track = step->GetTrack();

	// check if the track (=gamma) is killed
	if(track->GetTrackStatus() == fStopAndKill && 
		track->GetParticleDefinition()->GetPDGEncoding() == G4Gamma::Gamma()->GetPDGEncoding())
	{	
		// do not store in absorbers
		if(G4StrUtil::contains(step->GetPostStepPoint()->GetPhysicalVolume()->GetName(),"abs_")){return;}

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
