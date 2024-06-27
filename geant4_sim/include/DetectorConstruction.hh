#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TROOT.h"
#include "TGraph.h"
#pragma GCC diagnostic pop

#include "G4VUserDetectorConstruction.hh"
#include "SimParameters.hh"
#include "G4Material.hh"
#include "G4VSolid.hh"
#include "G4MagneticField.hh"
#include "G4RotationMatrix.hh"
#include "RunAction.hh"

class SimParameters;
class RunAction;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    	DetectorConstruction(RunAction*,G4String,G4double,G4double);
    	~DetectorConstruction();

    	G4VPhysicalVolume* Construct();

private:
	SimParameters* simPar;
	RunAction* runAction;

	G4bool visibility; // components visibility
	G4bool addAbsorber; // build SR photon absorber
	G4Material* mat_vac; // vacuum material
	G4VSolid* world_box; // world SV
	G4LogicalVolume* world_log; // world LV
	G4VPhysicalVolume* world_phys; // world PV
	G4VSolid* beampipeUni_solid; // beam pipe SV = original beam pipe + extension
	G4VSolid* beampipeVac_solid; // beam pipe SV = beampipeUni_solid - (all magnets + IP beam pipe)
	G4VSolid* beampipeUniBwd_solid; // bwd beam pipe SV
	G4LogicalVolume* beampipeVac_log; // beam pipe LV
	G4VSolid* ip_box; // IP beam pipe virtual volume
	G4LogicalVolume* ip_log; // IP beam pipe LV
	G4VSolid* bwd_beampipeVac_solid; // backward SV
	G4VSolid* farbwd_beampipeVac_solid; // far-backward SV
	G4LogicalVolume* farbwd_beampipeVac_log; // far-backward LV

	G4double mass;
	G4double gamma;

	void BuildMaterials();
	void BuildWorld();
	void CreateBeamPipeSolid();
	void CreateSrMask();
	void BuildMagnets();
	void BuildParticles();
	void BuildIrBeamPipe();
	void BuildIpBeamPipe();
	void BuildAbsorbers();
	void BuildTaggers();
	void BuildFwdAnteChamber();

	G4MagneticField* GetMagField(G4int,G4LogicalVolume*,G4ThreeVector,G4RotationMatrix*);
	G4double tag1_posZ;
	G4double ante_chmbr_H;
	G4ThreeVector far_bwd_center;
	G4RotationMatrix* farbwd_rot;
};

#endif

