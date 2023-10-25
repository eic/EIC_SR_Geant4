#include "StackingAction.hh"
#include "G4Track.hh"

StackingAction::StackingAction()
{
    	_kill_secondaries  = 0;
    	fStackingMessenger = new StackingMessenger(this);
        simPar = new SimParameters();
        simPar->InitDefault();
        simPar->PrintGammaEcut();
}

StackingAction::~StackingAction()
{
    	delete fStackingMessenger;
	delete simPar;
}

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{
    	G4ClassificationOfNewTrack status = fUrgent;
    
	// kill secondaries
    	if(_kill_secondaries == 1 && track->GetTrackID() > 1){status = fKill;}

	// kill gammas
	if(track->GetTotalEnergy() < simPar->GetGammaEcut()){status = fKill;}

    	return status;
}
