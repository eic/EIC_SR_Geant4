#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

class DetectorMessenger: public G4UImessenger
{
public:
  
    DetectorMessenger(DetectorConstruction*);
   ~DetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
    DetectorConstruction*       fDetector;
    G4UIdirectory*              _directory;
    G4UIcmdWithADoubleAndUnit*  _target_length_Cmd;
    G4UIcmdWithADoubleAndUnit*  _target_width_Cmd;
    G4UIcmdWithADoubleAndUnit*  _target_height_Cmd;
};

#endif

