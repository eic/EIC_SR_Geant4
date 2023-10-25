#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "SimParameters.hh"

class SimParameters;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    	DetectorConstruction();
    	~DetectorConstruction();

    	G4VPhysicalVolume* Construct();
	SimParameters* simPar;
};

#endif

