#ifndef RunMessenger_h
#define RunMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class RunAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

class RunMessenger: public G4UImessenger
{
public:
  
    RunMessenger(RunAction*);
   ~RunMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
  
    RunAction*       fRunAction;

    G4UIdirectory*              _directory;
    G4UIcmdWithAString*  _output_filename_Cmd;
};

#endif

