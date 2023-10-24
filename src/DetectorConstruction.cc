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
#include "SimParameters.hh"
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
	SimParameters simPar;
	simPar.InitDefault();
	simPar.PrintGeoParameters();

	G4NistManager* nistManager = G4NistManager::Instance();
	//------------------------------------------------------------------------
	//- Materials
	//------------------------------------------------------------------------
	//- Vacuum
	G4Material* mat_vac = nistManager->FindOrBuildMaterial("G4_Galactic");
	//------------------------------------------------------------------------
    	//- Build world
   	G4Box* world_box = new G4Box("world",
                                 simPar.GetWorldSizeX()/2,
                                 simPar.GetWorldSizeY()/2,
                                 simPar.GetWorldSizeZ()/2);
    	G4LogicalVolume* world_log = new G4LogicalVolume(world_box, mat_vac, "world");
    	G4VPhysicalVolume* world_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),world_log,"world",0,false,0,true);
	world_log->SetVisAttributes(G4VisAttributes::GetInvisible());

    	//- Build beam pipe vacuum

	//- Original
	auto mesh = CADMesh::TessellatedMesh::FromOBJ("../geometry/beam_pipe_20231011_Zup.obj");
	G4VSolid* beampipeOrig_solid = mesh->GetSolid();
	G4LogicalVolume* beampipeOrig_log = new G4LogicalVolume(beampipeOrig_solid, mat_vac,"beampipeOrig");

	//- Extention
	G4VSolid* beampipeExt_solid = new G4Tubs("beampipeExt_solid", 0, 
		simPar.GetExtR(), simPar.GetExtL()/2, 0, 2.0 * M_PI);
	G4LogicalVolume* beampipeExt_log = new G4LogicalVolume(beampipeExt_solid, mat_vac,"beampipeExt");

	//- Union
	G4VSolid* beampipeUni_solid = new G4UnionSolid("beampipeUni_solid", 
		beampipeOrig_solid, beampipeExt_solid, 0, G4ThreeVector(0, 0, simPar.GetExtE()-simPar.GetExtL()/2));
	G4LogicalVolume* beampipeUni_log = new G4LogicalVolume(beampipeUni_solid, mat_vac,"beampipeUni");

	G4VSolid* beampipeVac_solid = beampipeUni_solid;

	for(int iMag = 0; iMag < simPar.GetMagNum(); iMag++)
	{
		G4double rotY_angle = (simPar.GetMagStartDirZ(iMag) == 0) ? 0.0*rad : 
			atan(simPar.GetMagStartDirX(iMag)/simPar.GetMagStartDirZ(iMag))*rad;
		G4RotationMatrix* mag_rot = new G4RotationMatrix();
		mag_rot->rotateY(-rotY_angle);

		G4Box* mag_box = new G4Box(simPar.GetMagName(iMag)+"_box",
					1*m/2,1*m/2,simPar.GetMagLength(iMag)/2); 
		G4VSolid* mag_solid = new G4IntersectionSolid(simPar.GetMagName(iMag)+"_solid", 
					beampipeUni_solid, mag_box,
		mag_rot,G4ThreeVector(simPar.GetMagStartPosX(iMag)+simPar.GetMagLength(iMag)*sin(rotY_angle)/2.,
					simPar.GetMagStartPosY(iMag),
					simPar.GetMagStartPosZ(iMag)+simPar.GetMagLength(iMag)*cos(rotY_angle)/2.));
		G4LogicalVolume* mag_log = new G4LogicalVolume(mag_solid,mat_vac, simPar.GetMagName(iMag)+"_log");
		G4VPhysicalVolume* mag_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),mag_log,
								"mag_"+simPar.GetMagName(iMag),world_log,false,0,true);

		G4MagneticField* mag_field; 
		if(simPar.GetMagType(iMag) == "D-pole")
		{
			mag_field = new G4UniformMagField(G4ThreeVector(simPar.GetMagBx(iMag),
			simPar.GetMagBy(iMag),simPar.GetMagBz(iMag)));
			mag_log->SetVisAttributes(G4VisAttributes(G4Color::Blue()));  
		}
		else if(simPar.GetMagType(iMag) == "Q-pole")
		{
			mag_field = new G4QuadrupoleMagField(simPar.GetMagK1(iMag));
			mag_log->SetVisAttributes(G4VisAttributes(G4Color::Yellow()));  
		}
		else
		{
			// Error
		}

		G4FieldManager* localFieldMgr = new G4FieldManager(mag_field);
		localFieldMgr->SetDetectorField(mag_field);
		localFieldMgr->CreateChordFinder(mag_field);
		mag_log->SetFieldManager(localFieldMgr, true);

		//- Vacuum
		beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",beampipeVac_solid,
		mag_box,mag_rot,G4ThreeVector(simPar.GetMagStartPosX(iMag)+simPar.GetMagLength(iMag)*sin(rotY_angle)/2.,
		simPar.GetMagStartPosY(iMag),
		simPar.GetMagStartPosZ(iMag)+simPar.GetMagLength(iMag)*cos(rotY_angle)/2.));
	}

	G4LogicalVolume* beampipeVac_log = new G4LogicalVolume(beampipeVac_solid, mat_vac,"beampipeVac");
	new G4PVPlacement(0,G4ThreeVector(0,0,0),beampipeVac_log,"vac_beampipe",world_log,false,0,true);
	beampipeVac_log->SetVisAttributes(G4VisAttributes(G4Color::Grey()));

	// absorbers
	for(int iAbs = 0; iAbs < simPar.GetAbsNum(); iAbs++)
	{
		G4Box* abs_box = new G4Box("abs_box_"+to_string(iAbs),
		simPar.GetAbsSizeX(iAbs)/2,simPar.GetAbsSizeY(iAbs)/2,simPar.GetAbsSizeZ(iAbs)/2); 

		G4VSolid* abs_solid = new G4SubtractionSolid("abs_solid_"+to_string(iAbs),abs_box,beampipeVac_solid,0,
		G4ThreeVector(-simPar.GetAbsPosX(iAbs),-simPar.GetAbsPosY(iAbs),-simPar.GetAbsPosZ(iAbs)));

		G4LogicalVolume* abs_log = new G4LogicalVolume(abs_solid,mat_vac, "abs_log_"+to_string(iAbs));

		G4VPhysicalVolume* abs_phys = new G4PVPlacement(0, 
		G4ThreeVector(simPar.GetAbsPosX(iAbs),simPar.GetAbsPosY(iAbs),simPar.GetAbsPosZ(iAbs)),
		abs_log,"abs_"+to_string(iAbs),world_log,false,0,true);

		abs_log->SetVisAttributes(G4VisAttributes(G4Color::Magenta()));  
	}

	return world_phys;
}
