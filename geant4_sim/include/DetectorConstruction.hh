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

class SimParameters;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    	DetectorConstruction(G4String);
    	~DetectorConstruction();

    	G4VPhysicalVolume* Construct();

private:
	SimParameters* simPar;
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
	void BuildFarBackwardBeamPipe();
	void BuildFarBackwardBeamPipe2();
	void BuildAbsorbers();
	void BuildTaggers();

	G4MagneticField* GetMagField(G4int,G4LogicalVolume*);
};

#endif

