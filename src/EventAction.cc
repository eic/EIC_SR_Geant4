#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

EventAction::EventAction(RunAction* runAct) : runAction(runAct)
{
	_timer = new G4Timer;
}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* event)
{
	_timer->Start();
    	runAction->_event = event->GetEventID();

	runAction->_gammaPosX.clear();
	runAction->_gammaPosY.clear();
	runAction->_gammaPosZ.clear();
	runAction->_gammaMomX.clear();
	runAction->_gammaMomY.clear();
	runAction->_gammaMomZ.clear();
	runAction->_gammaEnergy.clear();

    	if((runAction->_event)%100 == 0)
        	printf("[INFO] EventAction::BeginOfEventAction ==> BeginOfEvent: %12d\n",runAction->_event);

	return;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    	runAction->_tree->Fill();

	runAction->_beamPdg = -999;
	runAction->_beamPosX = -999;
	runAction->_beamPosY = -999;
	runAction->_beamPosZ = -999;
	runAction->_beamMomX = -999;
	runAction->_beamMomY = -999;
	runAction->_beamMomZ = -999;
	runAction->_beamEnergy = -999;

    	_timer->Stop();
	// 'real' is wall clock time - time from start to finish of the call. This is all elapsed time 
	// including time slices used by other processes and time the process spends blocked (for example 
	// if it is waiting for I/O to complete).
	runAction->_avgEventTime += _timer->GetRealElapsed();

	return;
}
