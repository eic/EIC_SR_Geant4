#include "TrackingAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

TrackingAction::TrackingAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintStoreTrajectories();
}

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
	// new track, reset trajectory vectors
	runAction->_trackPosX_cm.clear();
	runAction->_trackPosY_cm.clear();
	runAction->_trackPosZ_cm.clear();
	runAction->_trackEne_GeV.clear();
	runAction->_trackTrackID.clear();
	runAction->_trackEventID.clear();

	// set the vertex as the first point in the trajectory
	runAction->_trackPosX_cm.push_back(track->GetVertexPosition().x()/cm);
	runAction->_trackPosY_cm.push_back(track->GetVertexPosition().y()/cm);
	runAction->_trackPosZ_cm.push_back(track->GetVertexPosition().z()/cm);
	runAction->_trackEne_GeV.push_back(track->GetKineticEnergy()/GeV);
	runAction->_trackTrackID.push_back(track->GetTrackID());
	runAction->_trackEventID.push_back(runAction->_event);

	return;
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
	// filter particles before to store
	if(	simPar->GetStoreTrajectories() &&
		runAction->_trackPosZ_cm.back() > simPar->GetStoreTrajectoriesZmin()/cm && 
		runAction->_trackPosZ_cm.back() < simPar->GetStoreTrajectoriesZmax()/cm && 
		runAction->_trackEne_GeV.back() > simPar->GetStoreTrajectoriesEmin()/GeV)
	{
		runAction->_tracks->Fill();
	}

	return;
}
