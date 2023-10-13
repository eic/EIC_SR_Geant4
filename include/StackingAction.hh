#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "StackingMessenger.hh"
#include "globals.hh"

class G4Track;
class StackingMessenger;

class StackingAction : public G4UserStackingAction
{
public:

  StackingAction();
  virtual ~StackingAction();

  G4int GetKillStatus() { return _kill_secondaries;}
  void SetKillStatus(G4int status){_kill_secondaries = status;}
    
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
    
private:

  G4int _kill_secondaries;
  StackingMessenger* fStackingMessenger;

};

#endif
