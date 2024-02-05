#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "RunAction.hh"
#include "SimParameters.hh"
#include "G4Gamma.hh"

class RunAction;
class SimParameters;

class TrackingAction : public G4UserTrackingAction
{
public:  
	TrackingAction(RunAction*,G4String);
	~TrackingAction();
	
	void PreUserTrackingAction(const G4Track*);   
	void PostUserTrackingAction(const G4Track*);
    
private:
	RunAction* runAction;
	SimParameters* simPar;
};

#endif
