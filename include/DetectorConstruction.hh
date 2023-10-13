#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4PhysicalConstants.hh"
#include "DetectorMessenger.hh"
#include "globals.hh"

class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();

    void SetTargetLength(G4double value){_target_length = value;}
    void SetTargetWidth(G4double value){_target_width = value;}
    void SetTargetHeight(G4double value){_target_height = value;}

    G4double GetTargetLength() {return _target_length;}
    G4double GetTargetWidth() {return _target_width;}
    G4double GetTargetHeight() {return _target_height;}

private:
    G4double _target_length;
    G4double _target_width;
    G4double _target_height;

    DetectorMessenger* fDetectorMessenger;
};

#endif

