#ifndef PrimaryMessenger_h
#define PrimaryMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;
class G4UIcmdWithADouble;

class PrimaryMessenger: public G4UImessenger
{
public:
  
    PrimaryMessenger(PrimaryGeneratorAction*);
   ~PrimaryMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
  
    PrimaryGeneratorAction* fPrimaryGeneratorAction;

    G4UIdirectory*              _directory;
    G4UIcmdWithADouble*  _deltaP_Cmd;
    G4UIcmdWithADoubleAndUnit*  _P_Cmd;
    G4UIcmdWithADouble*  _PdirX_Cmd;
    G4UIcmdWithADouble*  _PdirY_Cmd;
    G4UIcmdWithADouble*  _PdirZ_Cmd;
    G4UIcmdWithAString*  _pName_Cmd;
};

#endif

