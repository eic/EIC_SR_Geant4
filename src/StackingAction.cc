#include "StackingAction.hh"
#include "G4Track.hh"

StackingAction::StackingAction()
{
    _kill_secondaries  = 0;
    fStackingMessenger = new StackingMessenger(this);
}

StackingAction::~StackingAction()
{
    delete fStackingMessenger;
}

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
    G4ClassificationOfNewTrack status = fUrgent;
    
    if(_kill_secondaries == 1)
    {
        if(aTrack->GetTrackID() > 1)
        {
            status = fKill;
        }
    }
    return status;
}
