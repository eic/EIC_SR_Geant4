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
  
    	DetectorConstruction* fDetectorConstruction;

	G4UIcmdWithADoubleAndUnit* 	_dist_tag_Cmd;
	G4UIcmdWithADoubleAndUnit* 	_bwd_achmbH_Cmd;
	G4UIcmdWithoutParameter*	_modify_Cmd;
};

#endif

