#ifndef EventAction_h
#define EventAction_h 1

#include "G4ThreeVector.hh"
#include "G4Timer.hh"
#include "G4UserEventAction.hh"
#include "RunAction.hh"
#include "globals.hh"
#include "SimParameters.hh"

class RunAction;
class SimParameters;

class EventAction : public G4UserEventAction
{
public:
    	EventAction(RunAction*,G4String);
    	virtual ~EventAction();

    	virtual void BeginOfEventAction(const G4Event* event);
    	virtual void EndOfEventAction(const G4Event* event);

private:
    	RunAction* runAction;
	G4Timer* _timer;

private:
        SimParameters* simPar;
};

#endif

