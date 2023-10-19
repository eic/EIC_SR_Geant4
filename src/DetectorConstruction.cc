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
#include "G4QuadrupoleMagField.hh"
#include "G4UserLimits.hh"
#include "G4ChordFinder.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

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
	world_log->SetVisAttributes(G4VisAttributes::GetInvisible());

    	//- Build beam pipe vacuum

	//- Original
	auto mesh = CADMesh::TessellatedMesh::FromOBJ("../geometry/beam_pipe_20231011_Zup.obj");
	G4VSolid* beampipeOrig_solid = mesh->GetSolid();
	G4LogicalVolume* beampipeOrig_log = new G4LogicalVolume(beampipeOrig_solid, mat_vac,"beampipeOrig");

	//- Extention
	G4double ext_length = 370*cm;
	G4double ext_radius = 20.0*cm;
	G4double ext_endpos = -3350.0*cm;
	G4VSolid* beampipeExt_solid = new G4Tubs("beampipeExt_solid", 0, ext_radius, ext_length/2, 0, 2.0 * M_PI);
	G4LogicalVolume* beampipeExt_log = new G4LogicalVolume(beampipeExt_solid, mat_vac,"beampipeExt");

	//- Union
	G4VSolid* beampipeUni_solid = new G4UnionSolid("beampipeUni_solid", beampipeOrig_solid, beampipeExt_solid, 0, G4ThreeVector(0, 0, ext_endpos-ext_length/2));
	G4LogicalVolume* beampipeUni_log = new G4LogicalVolume(beampipeUni_solid, mat_vac,"beampipeUni");

	// D2EF2-box
	G4double length_D2EF2 = 2.726015431*m;
	G4double startPosX_D2EF2 = 7.61776450600000E+00*cm;
	G4double startPosZ_D2EF2 = -3.71513783800000E+03*cm;
	G4double startDirX_D2EF2 = -2.48090543441244E-02*cm;
	G4double startDirZ_D2EF2 = 9.99692208043331E-01*cm;
	G4double rotY_angle_D2EF2 = (startDirZ_D2EF2 == 0) ? 0.0*rad : atan(startDirX_D2EF2/startDirZ_D2EF2)*rad;
	G4RotationMatrix* D2EF2_rot = new G4RotationMatrix();
	D2EF2_rot->rotateY(-rotY_angle_D2EF2);

	G4Box* D2EF2_box = new G4Box("D2EF2_box",1*m/2,1*m/2,length_D2EF2/2); 
	G4VSolid* D2EF2_solid_1 = new G4IntersectionSolid("D2EF2_solid", beampipeUni_solid, D2EF2_box,
	D2EF2_rot,G4ThreeVector(startPosX_D2EF2+length_D2EF2*sin(rotY_angle_D2EF2)/2.,0.,startPosZ_D2EF2+length_D2EF2*cos(rotY_angle_D2EF2)/2.));
	G4LogicalVolume* D2EF2_log = new G4LogicalVolume(D2EF2_solid_1,mat_vac, "D2EF2_log");
	G4VPhysicalVolume* D2EF2_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),D2EF2_log,"mag_D2EF2",world_log,false,0);
	D2EF2_log->SetVisAttributes(G4VisAttributes(G4Color::Blue()));  

	G4double field_D2EF2 = 2.54524403400000E-01*tesla;
	G4MagneticField* D2EF2_mag = new G4UniformMagField(G4ThreeVector(0, field_D2EF2,0.));
	G4FieldManager* localFieldMgr_D2EF2 = new G4FieldManager(D2EF2_mag);
	localFieldMgr_D2EF2->SetDetectorField(D2EF2_mag);
	localFieldMgr_D2EF2->CreateChordFinder(D2EF2_mag);
	D2EF2_log->SetFieldManager(localFieldMgr_D2EF2, true);

	// D2EF1-box
	G4double length_D2EF1 = 2.726015431*m;
	G4double startPosX_D2EF1 = 2.10318264100000E+00*cm;
	G4double startPosZ_D2EF1 = -3.41670739300000E+03*cm;
	G4double startDirX_D2EF1 = -1.31554205131084E-02*cm;
	G4double startDirZ_D2EF1 = 9.99913463711297E-01*cm;
	G4double rotY_angle_D2EF1 = (startDirZ_D2EF1 == 0) ? 0.0*rad : atan(startDirX_D2EF1/startDirZ_D2EF1)*rad;
	G4RotationMatrix* D2EF1_rot = new G4RotationMatrix();
	D2EF1_rot->rotateY(-rotY_angle_D2EF1);

	G4Box* D2EF1_box = new G4Box("D2EF1_box",1*m/2,1*m/2,length_D2EF1/2); 
	G4VSolid* D2EF1_solid_1 = new G4IntersectionSolid("D2EF1_solid", beampipeUni_solid, D2EF1_box,
	D2EF1_rot,G4ThreeVector(startPosX_D2EF1+length_D2EF1*sin(rotY_angle_D2EF1)/2.,0.,startPosZ_D2EF1+length_D2EF1*cos(rotY_angle_D2EF1)/2.));
	G4LogicalVolume* D2EF1_log = new G4LogicalVolume(D2EF1_solid_1,mat_vac, "D2EF1_log");
	G4VPhysicalVolume* D2EF1_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),D2EF1_log,"mag_D2EF1",world_log,false,0);
	D2EF1_log->SetVisAttributes(G4VisAttributes(G4Color::Blue()));  

	G4double field_D2EF1 = 2.54524403400000E-01*tesla;
	G4MagneticField* D2EF1_mag = new G4UniformMagField(G4ThreeVector(0, field_D2EF1,0.));
	G4FieldManager* localFieldMgr_D2EF1 = new G4FieldManager(D2EF1_mag);
	localFieldMgr_D2EF1->SetDetectorField(D2EF1_mag);
	localFieldMgr_D2EF1->CreateChordFinder(D2EF1_mag);
	D2EF1_log->SetFieldManager(localFieldMgr_D2EF1, true);

	// D1EF-box
	G4double length_D1EF = 0.8914000836*m;
	G4double startPosX_D1EF = 6.68549937300000E-02*cm;
	G4double startPosZ_D1EF = -3.11826997500000E+03*cm;
	G4double startDirX_D1EF = -1.49999943750006E-03*cm;
	G4double startDirZ_D1EF = 9.99998875000211E-01*cm;
	G4double rotY_angle_D1EF = (startDirZ_D1EF == 0) ? 0.0*rad : atan(startDirX_D1EF/startDirZ_D1EF)*rad;
	G4RotationMatrix* D1EF_rot = new G4RotationMatrix();
	D1EF_rot->rotateY(-rotY_angle_D1EF);

	G4Box* D1EF_box = new G4Box("D1EF_box",1*m/2,1*m/2,length_D1EF/2); 
	G4VSolid* D1EF_solid_1 = new G4IntersectionSolid("D1EF_solid", beampipeUni_solid, D1EF_box,
	D1EF_rot,G4ThreeVector(startPosX_D1EF+length_D1EF*sin(rotY_angle_D1EF)/2.,0.,startPosZ_D1EF+length_D1EF*cos(rotY_angle_D1EF)/2.));
	G4LogicalVolume* D1EF_log = new G4LogicalVolume(D1EF_solid_1,mat_vac, "D1EF_log");
	G4VPhysicalVolume* D1EF_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),D1EF_log,"mag_D1EF",world_log,false,0);
	D1EF_log->SetVisAttributes(G4VisAttributes(G4Color::Blue()));  

	G4double field_D1EF = 1.00169207900000E-01*tesla;
	G4MagneticField* D1EF_mag = new G4UniformMagField(G4ThreeVector(0, field_D1EF,0.));
	G4FieldManager* localFieldMgr_D1EF = new G4FieldManager(D1EF_mag);
	localFieldMgr_D1EF->SetDetectorField(D1EF_mag);
	localFieldMgr_D1EF->CreateChordFinder(D1EF_mag);
	D1EF_log->SetFieldManager(localFieldMgr_D1EF, true);


	// Q1EF-box 
	G4double length_Q1EF = 1.61*m;
	G4double startPosX_Q1EF = 0.0*cm;
	G4double startPosZ_Q1EF = -1.23700000000000E+03*cm;
	G4double startDirX_Q1EF = 0.0*cm;
	G4double startDirZ_Q1EF = 0.0*cm;
	G4double rotY_angle_Q1EF = (startDirZ_Q1EF == 0) ? 0.0*rad : atan(startDirX_Q1EF/startDirZ_Q1EF)*rad;
	G4RotationMatrix* Q1EF_rot = new G4RotationMatrix();
	Q1EF_rot->rotateY(-rotY_angle_Q1EF);

	G4Box* Q1EF_box = new G4Box("Q1EF_box",1*m/2,1*m/2,length_Q1EF/2); 
	G4VSolid* Q1EF_solid_1 = new G4IntersectionSolid("Q1EF_solid", beampipeUni_solid, Q1EF_box,
	Q1EF_rot,G4ThreeVector(startPosX_Q1EF+length_Q1EF*sin(rotY_angle_Q1EF)/2.,0.,startPosZ_Q1EF+length_Q1EF*cos(rotY_angle_Q1EF)/2.));
	G4LogicalVolume* Q1EF_log = new G4LogicalVolume(Q1EF_solid_1,mat_vac, "Q1EF_log");
	G4VPhysicalVolume* Q1EF_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),Q1EF_log,"mag_Q1EF",world_log,false,0);
	Q1EF_log->SetVisAttributes(G4VisAttributes(G4Color::Yellow()));  

	G4double field_Q1EF = 5.957841642*tesla/(1.*meter);
	G4MagneticField* Q1EF_mag = new G4QuadrupoleMagField(field_Q1EF);
	G4FieldManager* localFieldMgr_Q1EF = new G4FieldManager(Q1EF_mag);
	localFieldMgr_Q1EF->SetDetectorField(Q1EF_mag);
	localFieldMgr_Q1EF->CreateChordFinder(Q1EF_mag);
	Q1EF_log->SetFieldManager(localFieldMgr_Q1EF, true);

	// Q0EF-box 
	G4double length_Q0EF = 1.20*m;
	G4double startPosX_Q0EF = 0.0*cm;
	G4double startPosZ_Q0EF = -7.00000000000000E+02*cm;
	G4double startDirX_Q0EF = 0.0*cm;
	G4double startDirZ_Q0EF = 0.0*cm;
	G4double rotY_angle_Q0EF = (startDirZ_Q0EF == 0) ? 0.0*rad : atan(startDirX_Q0EF/startDirZ_Q0EF)*rad;
	G4RotationMatrix* Q0EF_rot = new G4RotationMatrix();
	Q0EF_rot->rotateY(-rotY_angle_Q0EF);

	G4Box* Q0EF_box = new G4Box("Q0EF_box",1*m/2,1*m/2,length_Q0EF/2); 
	G4VSolid* Q0EF_solid_1 = new G4IntersectionSolid("Q0EF_solid", beampipeUni_solid, Q0EF_box,
	Q0EF_rot,G4ThreeVector(startPosX_Q0EF+length_Q0EF*sin(rotY_angle_Q0EF)/2.,0.,startPosZ_Q0EF+length_Q0EF*cos(rotY_angle_Q0EF)/2.));
	G4LogicalVolume* Q0EF_log = new G4LogicalVolume(Q0EF_solid_1,mat_vac, "Q0EF_log");
	G4VPhysicalVolume* Q0EF_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),Q0EF_log,"mag_Q0EF",world_log,false,0);
	Q0EF_log->SetVisAttributes(G4VisAttributes(G4Color::Yellow()));  

	G4double field_Q0EF = -12.98838344*tesla/(1.*meter);
	G4MagneticField* Q0EF_mag = new G4QuadrupoleMagField(field_Q0EF);
	G4FieldManager* localFieldMgr_Q0EF = new G4FieldManager(Q0EF_mag);
	localFieldMgr_Q0EF->SetDetectorField(Q0EF_mag);
	localFieldMgr_Q0EF->CreateChordFinder(Q0EF_mag);
	Q0EF_log->SetFieldManager(localFieldMgr_Q0EF, true);

	//- Vacuum
	G4VSolid* beampipeVac_solid_1 = new G4SubtractionSolid("beampipeVac_solid_1", beampipeUni_solid, D2EF2_box,
	D2EF2_rot,G4ThreeVector(startPosX_D2EF2+length_D2EF2*sin(rotY_angle_D2EF2)/2.,0.,startPosZ_D2EF2+length_D2EF2*cos(rotY_angle_D2EF2)/2.));

	G4VSolid* beampipeVac_solid_2 = new G4SubtractionSolid("beampipeVac_solid_2", beampipeVac_solid_1, D2EF1_box,
	D2EF1_rot,G4ThreeVector(startPosX_D2EF1+length_D2EF1*sin(rotY_angle_D2EF1)/2.,0.,startPosZ_D2EF1+length_D2EF1*cos(rotY_angle_D2EF1)/2.));

	G4VSolid* beampipeVac_solid_3 = new G4SubtractionSolid("beampipeVac_solid_3", beampipeVac_solid_2, D1EF_box,
	D1EF_rot,G4ThreeVector(startPosX_D1EF+length_D1EF*sin(rotY_angle_D1EF)/2.,0.,startPosZ_D1EF+length_D1EF*cos(rotY_angle_D1EF)/2.));

	G4VSolid* beampipeVac_solid_4 = new G4SubtractionSolid("beampipeVac_solid_4", beampipeVac_solid_3, Q1EF_box,
	Q1EF_rot,G4ThreeVector(startPosX_Q1EF+length_Q1EF*sin(rotY_angle_Q1EF)/2.,0.,startPosZ_Q1EF+length_Q1EF*cos(rotY_angle_Q1EF)/2.));

	G4VSolid* beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid_5", beampipeVac_solid_4, Q0EF_box,
	Q0EF_rot,G4ThreeVector(startPosX_Q0EF+length_Q0EF*sin(rotY_angle_Q0EF)/2.,0.,startPosZ_Q0EF+length_Q0EF*cos(rotY_angle_Q0EF)/2.));

	G4LogicalVolume* beampipeVac_log = new G4LogicalVolume(beampipeVac_solid, mat_vac,"beampipeVac");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),beampipeVac_log,"vac_beampipe",world_log,false,0);
	beampipeVac_log->SetVisAttributes(G4VisAttributes(G4Color::Grey()));


	// absorbers
	G4Box* abs_box_1 = new G4Box("abs_box_1",30*cm/2,30*cm/2,5*cm/2); 
	G4Box* abs_box_2 = new G4Box("abs_box_2",10*cm/2,10*cm/2,5*cm/2); 
	G4Box* abs_box_3 = new G4Box("abs_box_3",10*cm/2,10*cm/2,5*cm/2); 
	G4Box* abs_box_4 = new G4Box("abs_box_4",50*cm/2,50*cm/2,5*cm/2); 

	G4VSolid* abs_solid_1 = new G4SubtractionSolid("abs_solid_1",abs_box_1,beampipeVac_solid,0,G4ThreeVector(0.,0.,-1527*cm));
	G4VSolid* abs_solid_2 = new G4SubtractionSolid("abs_solid_2",abs_box_2,beampipeVac_solid,0,G4ThreeVector(12*cm,0.,-475*cm));
	G4VSolid* abs_solid_3 = new G4SubtractionSolid("abs_solid_3",abs_box_3,beampipeVac_solid,0,G4ThreeVector(-14*cm,0.,550*cm));
	G4VSolid* abs_solid_4 = new G4SubtractionSolid("abs_solid_4",abs_box_4,beampipeVac_solid,0,G4ThreeVector(0.,0.,3720*cm));

	G4LogicalVolume* abs_log_1 = new G4LogicalVolume(abs_solid_1,mat_vac, "abs_log_1");
	G4LogicalVolume* abs_log_2 = new G4LogicalVolume(abs_solid_2,mat_vac, "abs_log_2");
	G4LogicalVolume* abs_log_3 = new G4LogicalVolume(abs_solid_3,mat_vac, "abs_log_3");
	G4LogicalVolume* abs_log_4 = new G4LogicalVolume(abs_solid_4,mat_vac, "abs_log_4");

	G4VPhysicalVolume* abs_phys_1 = new G4PVPlacement(0, G4ThreeVector(0.,0.,1527*cm),abs_log_1,"abs_1",world_log,false,0);
	G4VPhysicalVolume* abs_phys_2 = new G4PVPlacement(0, G4ThreeVector(-12*cm,0.,475*cm),abs_log_2,"abs_2",world_log,false,0);
	G4VPhysicalVolume* abs_phys_3 = new G4PVPlacement(0, G4ThreeVector(14*cm,0.,-550*cm),abs_log_3,"abs_3",world_log,false,0);
	G4VPhysicalVolume* abs_phys_4 = new G4PVPlacement(0, G4ThreeVector(0.,0.,-3720*cm),abs_log_4,"abs_4",world_log,false,0);

	abs_log_1->SetVisAttributes(G4VisAttributes(G4Color::Magenta()));  
	abs_log_2->SetVisAttributes(G4VisAttributes(G4Color::Magenta()));  
	abs_log_3->SetVisAttributes(G4VisAttributes(G4Color::Magenta()));  
	abs_log_4->SetVisAttributes(G4VisAttributes(G4Color::Magenta()));  

	return world_phys;
}
