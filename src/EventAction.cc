#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

EventAction::EventAction(RunAction* runAct) : runAction(runAct)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    G4int eventID = event->GetEventID();

    runAction->_Event_ID = eventID;

    runAction->_X0      = -999.0;
    runAction->_Y0      = -999.0;
    runAction->_Z0      = -999.0;
    runAction->_Mom_X0  = -999.0;
    runAction->_Mom_Y0  = -999.0;
    runAction->_Mom_Z0  = -999.0;
    runAction->_Ekin0   = -999.0;
    runAction->_Det0    = 0;

    runAction->_X1      = -999.0;
    runAction->_Y1      = -999.0;
    runAction->_Z1      = -999.0;
    runAction->_Mom_X1  = -999.0;
    runAction->_Mom_Y1  = -999.0;
    runAction->_Mom_Z1  = -999.0;
    runAction->_Ekin1   = -999.0;
    runAction->_Det1    = 0;

    if(eventID%10000 == 0)
    {
        printf("\r--> BeginOfEvent: %12d",eventID);
        fflush(stdout);
    }
}

void EventAction::EndOfEventAction(const G4Event*)
{
    runAction->_tree->Fill();
}
