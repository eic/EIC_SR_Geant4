#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "Constants.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "CADMesh.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4ChordFinder.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), fDetectorMessenger(0)
{
	fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
    	delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	G4NistManager* nistManager = G4NistManager::Instance();
	//------------------------------------------------------------------------
	//- Materials
	//------------------------------------------------------------------------
	//- Vacuum
	G4Material* mat_vac = nistManager->FindOrBuildMaterial("G4_Galactic");
	//------------------------------------------------------------------------
    	//- Build world
   	G4Box* world_box = new G4Box("world",
                                 Constants::world_dim_x/2,
                                 Constants::world_dim_y/2,
                                 Constants::world_dim_z/2);
    	G4LogicalVolume* world_log = new G4LogicalVolume(world_box, mat_vac, "world");
    	G4VPhysicalVolume* world_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),world_log,"world",0,false,0);

    	//- Build beam pipe vacuum
	auto mesh = CADMesh::TessellatedMesh::FromOBJ("../geometry/beam_pipe_20231011_Zup.obj");
	G4VSolid* beampipe_solid = mesh->GetSolid();
	G4LogicalVolume* beampipe_log = new G4LogicalVolume(beampipe_solid, mat_vac,"beampipe");
//	new G4PVPlacement(0,G4ThreeVector(0,0,0),beampipe_log,"beampipe",world_log,false,0);
	beampipe_log->SetVisAttributes(G4VisAttributes(G4Color::White()));
/*
	G4Box* mag_dipl = new G4Box("mag_dipl",1*m/2,1*m/2,1*m/2);
	G4LogicalVolume* mag_dipl_log = new G4LogicalVolume(mag_dipl,mat_vac, "mag_dipl");
	G4VPhysicalVolume* mag_dipl_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),
						mag_dipl_log,"mag_dipl",world_log,false,0); 
	mag_dipl_log->SetVisAttributes(G4VisAttributes(G4Color::White()));  

	G4MagneticField* magField = new G4UniformMagField(G4ThreeVector(0, -2.*tesla,0.));
	G4FieldManager* localFieldMgr = new G4FieldManager(magField);
	localFieldMgr->SetDetectorField(magField);
	localFieldMgr->CreateChordFinder(magField);
	mag_dipl_log->SetFieldManager(localFieldMgr, true);
*/
	G4double D2EF2_length = 2.726015431*m;
	G4Box* D2EF2_box = new G4Box("D2EF2_box",1*m/2,1*m/2,D2EF2_length/2); 
	G4LogicalVolume* D2EF2_log = new G4LogicalVolume(D2EF2_box,mat_vac, "D2EF2_log");

	G4double startPosX = 7.61776450600000E+00*cm;
	G4double startPosZ = -3.71513783800000E+03*cm;
	G4double startDirX = -2.48090543441244E-02*cm;
	G4double startDirZ = 9.99692208043331E-01*cm;
	G4double rotY_angle = atan(startDirX/startDirZ)*rad;
	G4RotationMatrix* D2EF2_rot = new G4RotationMatrix();
	D2EF2_rot->rotateY(-rotY_angle);
	G4VPhysicalVolume* D2EF2_phys = new G4PVPlacement(D2EF2_rot, G4ThreeVector(startPosX+D2EF2_length*sin(rotY_angle)/2.,0.,startPosZ+D2EF2_length*cos(rotY_angle)/2.),D2EF2_log,"D2EF2",world_log,false,0);
	D2EF2_log->SetVisAttributes(G4VisAttributes(G4Color::White()));  

	G4MagneticField* D2EF2_mag = new G4UniformMagField(G4ThreeVector(0, -2.54524403400000E-01*tesla,0.));
	G4FieldManager* localFieldMgr = new G4FieldManager(D2EF2_mag);
	localFieldMgr->SetDetectorField(D2EF2_mag);
	localFieldMgr->CreateChordFinder(D2EF2_mag);
	D2EF2_log->SetFieldManager(localFieldMgr, true);

/*
startPos_cm: 7.61776450600000E+00 0.00000000000000E+00 -3.71513783800000E+03
startDir_cm: -2.48090543441244E-02 0.00000000000000E+00 9.99692208043331E-01
G4double By = -2.54524403400000E-01*tesla;
*/
	return world_phys;
}
