#ifndef StackingMessenger_h
#define StackingMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class StackingAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

class StackingMessenger: public G4UImessenger
{
public:
  
    StackingMessenger(StackingAction*);
   ~StackingMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
  
    StackingAction*       fStackingAction;

    G4UIdirectory*              _directory;
    G4UIcmdWithAnInteger*  _kill_secondaries_Cmd;
};

#endif

