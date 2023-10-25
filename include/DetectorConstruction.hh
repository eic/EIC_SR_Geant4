#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "SimParameters.hh"
#include "RunAction.hh"

class SimParameters;
class RunAction; 

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    	DetectorConstruction(RunAction*);
    	~DetectorConstruction();

    	G4VPhysicalVolume* Construct();
	SimParameters* simPar;

private:
	RunAction* runAction;
};

#endif

