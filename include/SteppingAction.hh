#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "RunAction.hh"

class G4LogicalVolume;
class RunAction;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(RunAction*);
    virtual ~SteppingAction();
    virtual void UserSteppingAction(const G4Step *);

private:
    RunAction* runAction;
};

#endif
