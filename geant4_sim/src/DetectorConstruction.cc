#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"
#include "G4Polycone.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
#include "G4QuadrupoleMagField.hh"
#include "G4UserLimits.hh"
#include "G4ChordFinder.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "CADMesh.hh" // https://github.com/christopherpoole/CADMesh
#include "SolenoidMagField.hh"
#include "G4Torus.hh"
#include "G4EllipticalCone.hh"
#include "G4EllipticalTube.hh"
#include "G4ExtrudedSolid.hh"

const G4double farbwd_len = 20 * m;
const G4double B2eR_beampipe_radius = 5 * cm;

DetectorConstruction::DetectorConstruction(RunAction* runAct, G4String xmlFileName, G4double tag1_posZ_cm, G4double ante_chmbr_H_cm) : 
G4VUserDetectorConstruction(), runAction(runAct)
{
	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintBeampipeFileName();
	simPar->PrintIpBeampipeParameters();
	simPar->PrintGeoParameters();
	simPar->PrintMagParameters();
	simPar->PrintAbsParameters();

	addAbsorber = true;
	visibility = true;
	mat_vac = nullptr;
	world_box = nullptr;
	world_log = nullptr;
	world_phys = nullptr;
	beampipeUni_solid = nullptr;
	beampipeVac_solid = nullptr;
	bwd_beampipeVac_solid = nullptr;
	farbwd_beampipeVac_solid = nullptr;
	farbwd_beampipeVac_log = nullptr;
	beampipeVac_log = nullptr;
	ip_box = nullptr;
	ip_log = nullptr;
	mass = -1;
	gamma = -1;

	tag1_posZ = tag1_posZ_cm * cm;
	ante_chmbr_H = ante_chmbr_H_cm * cm;
	G4cout<<"[INFO] DetectorConstruction::DetectorConstruction ==> tag1_posZ = "
		<<tag1_posZ/cm<<" [cm]"<<G4endl;
	G4cout<<"[INFO] DetectorConstruction::DetectorConstruction ==> ante_chmbr_H = "
		<<ante_chmbr_H/cm<<" [cm]"<<G4endl;
	G4cout<<"[INFO] DetectorConstruction::DetectorConstruction ==> farbwd_len = "
		<<farbwd_len/m<<" [m]"<<G4endl;
	G4cout<<"[INFO] DetectorConstruction::DetectorConstruction ==> B2eR_beampipe_radius = "
		<<B2eR_beampipe_radius/cm<<" [cm]"<<G4endl;
}

DetectorConstruction::~DetectorConstruction()
{
	delete simPar;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	BuildMaterials();
	BuildWorld();
	CreateBeamPipeSolid();
	BuildFwdAnteChamber();
// 	CreateSrMask();
	BuildIpBeamPipe();
	BuildMagnets();
//	BuildFarBackwardBeamPipe();
//	BuildFarBackwardBeamPipe2();
	BuildTaggers();
	BuildIrBeamPipe();
	if(addAbsorber) BuildAbsorbers();

	return world_phys;
}

void DetectorConstruction::BuildFwdAnteChamber()
{
	if(!beampipeVac_solid || !beampipeUni_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildFwdAnteChamber ==> beam pipe solids are not created\n");
	}
	G4Box* fwd_cut = new G4Box("fwd_cut",1 * m, 1 * m, (30.0-12.5) * m/2.);
	// Cut the vacuum
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,fwd_cut,0,G4ThreeVector(0,0,-30*m + (30.0-12.5) * m/2.));
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,fwd_cut,0,G4ThreeVector(0,0,-30*m + (30.0-12.5) * m/2.));

	// fwd beam pipe	
	G4VSolid* fwd_beampipeVac_solid = new G4Tubs("fwd_beampipeVac_solid",
		0,10*cm/2,(30.0-12.5) * m/2.,0,2.0 * M_PI);
	
	G4EllipticalTube* ell_tube = new G4EllipticalTube("ell_tube",12.6*cm/2.,10*cm/2,(23.5-12.5) * m/2.);

	fwd_beampipeVac_solid = new G4UnionSolid("fwd_beampipeVac_solid",
		fwd_beampipeVac_solid,ell_tube,0,G4ThreeVector(0,0,(30.0-12.5) * m/2. - (23.5-12.5) * m/2.));

	// Ante-chamber
	const G4int nsect = 12;
	std::vector<G4TwoVector> polygon(nsect);
	// +X
	polygon[0].set(70*mm,-5*mm);
	polygon[1].set(140*mm,5*mm);
	polygon[2].set(150*mm,5*mm);
	polygon[3].set(150*mm,15*mm);
	polygon[4].set(140*mm,15*mm);
	polygon[5].set(70*mm,5*mm);
	// -X                                                                                          
	polygon[6].set(-70*mm,5*mm);
	polygon[7].set(-140*mm,15*mm);
	polygon[8].set(-150*mm,15*mm);
	polygon[9].set(-150*mm,5*mm);
	polygon[10].set(-140*mm,5*mm);
	polygon[11].set(-70*mm,-5*mm);

	G4TwoVector offsetA(0,0), offsetB(0,0);
	G4double scaleA = 1, scaleB = 1;
	G4double dz = (30.0-12.5) * m/2.;
	G4VSolid* ante_chamber_solid = new G4ExtrudedSolid("ante_chamber_solid", 
		polygon, dz, offsetA, scaleA, offsetB, scaleB);

	fwd_beampipeVac_solid = new G4UnionSolid("fwd_beampipeVac_solid",
		fwd_beampipeVac_solid,ante_chamber_solid,0,G4ThreeVector(0,0,0));

	G4LogicalVolume* fwd_beampipeVac_log = new G4LogicalVolume(
		fwd_beampipeVac_solid,mat_vac,"fwd_beampipeVac_log");
	fwd_beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut())); 
	new G4PVPlacement(0,G4ThreeVector(0,0,-30*m + dz),
		fwd_beampipeVac_log,"vac_fwd_beampipeVac",world_log,false,0,true);

	// Visualization
	G4VisAttributes* fwd_beampipeVac_vis = new G4VisAttributes(G4Color::White());
	fwd_beampipeVac_vis->SetVisibility(visibility);
	fwd_beampipeVac_log->SetVisAttributes(fwd_beampipeVac_vis);

	return;
}

void DetectorConstruction::BuildWorld()
{
	if(!mat_vac)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildWorld ==> Materials are not created\n");
	}

   	world_box = new G4Box("world",
                                 simPar->GetWorldSizeX()/2,
                                 simPar->GetWorldSizeY()/2,
                                 simPar->GetWorldSizeZ()/2);
    	world_log = new G4LogicalVolume(world_box,mat_vac,"world");
    	world_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),world_log,"world",0,false,0,true);
	//- Visualization
	world_log->SetVisAttributes(G4VisAttributes::GetInvisible());

	return;
}

void DetectorConstruction::CreateBeamPipeSolid()
{
	if(!mat_vac)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::CreateBeamPipeSolid ==> Materials are not created\n");
	}

	//- Original (beam pipe)
	auto mesh = CADMesh::TessellatedMesh::FromOBJ(simPar->GetBeampipeFileName().c_str());
	G4VSolid* beampipeOrig_solid = mesh->GetSolid();

	//- Extention
	G4VSolid* beampipeExtFwd_solid = new G4Tubs("beampipeExtFwd_solid", 0, 
		simPar->GetExtR(), simPar->GetExtL()/2, 0, 2.0 * M_PI);

	//- Union (beam pipe with an extention)
	beampipeUni_solid = new G4UnionSolid("beampipeUni_solid", 
		beampipeOrig_solid, beampipeExtFwd_solid, 0, 
		G4ThreeVector(0, 0, simPar->GetExtE()-simPar->GetExtL()/2));

	G4Box* beampipe_cut = new G4Box("beampipe_cut",2*m, 2*m, 10*m);

	//- Cut the vacuum on the BWD side 
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,beampipe_cut,0,G4ThreeVector(0,0,9280.0 * mm + 10*m));

	//- Copy
	beampipeVac_solid = beampipeUni_solid;

	//---------------------------------------------------------------------------------------------------------------//
	//- Build an extended bwd beam pipe
	const G4double B2eR_bend = 0.020 * rad;
	const G4double B2eR_bend_eps = 0.0001 * rad; // a small eps for the torus, otherwise the geo builder fails
	const G4double B2eR_len = 5.500091667736 * m;
	const G4double B2eR_cut_offset = 1 * cm;
	G4ThreeVector B2eR_start(0*cm,0*cm,1675*cm);
	G4double torus_radius = (0.5*B2eR_len/sin(B2eR_bend/2.)); 
	G4ThreeVector B2eR_end = B2eR_start - G4ThreeVector(torus_radius,0,B2eR_start.z());	
	B2eR_end.rotateY(B2eR_bend);
	B2eR_end += G4ThreeVector(torus_radius,0,B2eR_start.z());  

	const G4int numZPlanes = 8;
	const G4double zPlane[numZPlanes] = {
		9280.0 * mm, 9288.0 * mm, 9364.0 * mm, 11364.0 * mm, 11414.0 * mm, 
		15000.0 * mm, 16000.0 * mm, 25000.0 * mm};
	const G4double rInner[numZPlanes] = {0,0,0,0,0,0,0,0};
	const G4double rOuter[numZPlanes] = {
		128.6/2 * mm, 128.6/2 * mm, 154.0/2 * mm, 154.0/2 * mm, 196.0/2 * mm, 
		196.0/2 * mm, B2eR_beampipe_radius, B2eR_beampipe_radius};
	G4Polycone* polycone = new G4Polycone("polycone",
		0, 2.0 * M_PI, numZPlanes, zPlane, rInner, rOuter);

	// Torus - bent beam pipe
	G4RotationMatrix* torus_rot = new G4RotationMatrix();
	torus_rot->rotateX(-M_PI * 0.5 * rad);
	torus_rot->rotateY(M_PI * rad);
	
	G4VSolid* torus_solid = new G4Torus("torus_solid",0,B2eR_beampipe_radius,
		torus_radius,0,B2eR_bend + B2eR_bend_eps);
	bwd_beampipeVac_solid = new G4UnionSolid("bwd_beampipeVac_solid",
		polycone,torus_solid,
		torus_rot,G4ThreeVector(torus_radius, 0, B2eR_start.z()));

	//---------------------------------------------------------------------------------------------------------------//
/*
	// B2eR beam pipe extension
	G4RotationMatrix* torus_ext_rot = new G4RotationMatrix();
	torus_ext_rot->rotateY(-B2eR_bend);

	G4double torus_ext_len = 2 * cm;
	G4VSolid* torus_ext_solid = new G4Tubs("torus_ext_solid",0,B2eR_beampipe_radius, torus_ext_len/2.0, 0, 2.0 * M_PI * rad);
	bwd_beampipeVac_solid = new G4UnionSolid("bwd_beampipeVac_solid",
		bwd_beampipeVac_solid,torus_ext_solid,
		torus_ext_rot,G4ThreeVector(	B2eR_end.x() + 0.5*torus_ext_len*sin(B2eR_bend),
						B2eR_end.y(),
						B2eR_end.z() + 0.5*torus_ext_len*cos(B2eR_bend)));

*/
	//---------------------------------------------------------------------------------------------------------------//
	// Cut the BWD beam pipe at the end of B2eR
	bwd_beampipeVac_solid = new G4SubtractionSolid("bwd_beampipeVac_solid",
		bwd_beampipeVac_solid,beampipe_cut,0,G4ThreeVector(0,0,B2eR_end.z() + B2eR_cut_offset + 10*m));
	//---------------------------------------------------------------------------------------------------------------//
	// FarBWD beam pipe
	G4double farbwd_ext = 5 * m;
	G4double lumi_halflen = 2.25 * m;
	G4RotationMatrix* farbwd_rot = new G4RotationMatrix();
	farbwd_rot->rotateY(-B2eR_bend);
	G4RotationMatrix* lumi_rot = new G4RotationMatrix();
	lumi_rot->rotateY(B2eR_bend);
	G4RotationMatrix* cut_rot = new G4RotationMatrix();
	cut_rot->rotateY(B2eR_bend);

	// main beam pipe
	G4VSolid* farbwd_tube = new G4Tubs("farbwd_tube", 0,B2eR_beampipe_radius, (farbwd_len+farbwd_ext)/2., 0, 2.0 * M_PI * rad);
	// luminosity beam pipe
	G4VSolid* lumi_tube = new G4Tubs("farbwd_tube", 0,B2eR_beampipe_radius, lumi_halflen, 0, 2.0 * M_PI * rad);
	// ante-chamber
	G4Box* ante_chmbr_box = new G4Box("ante_chmbr_box",25*cm/2.,ante_chmbr_H/2.,farbwd_len/2.);
	// unite main and ante-chamber
	farbwd_tube = new G4UnionSolid("farbwd_tube",
		farbwd_tube,ante_chmbr_box,0,G4ThreeVector(25*cm/2.,0,0));
	// distance between the main beam pipe center (placement coordinates) and the cut
	G4double L = (B2eR_end.z() + 0.5*farbwd_len*cos(B2eR_bend)-(B2eR_end.z() + B2eR_cut_offset))/cos(B2eR_bend);
	// half-width of the cut in the main beam pipe
	G4double M = B2eR_end.x() + 0.5*farbwd_len*sin(B2eR_bend) - L*sin(B2eR_bend);
	// displacement of the lumi pipe w.r.t. the main beam pipe
	G4double dL = M * sin(B2eR_bend); // along main beam pipe Z-axis
	G4double dK = M * cos(B2eR_bend); // along main beam pipe X-axis
	// unite lumi and main+ante-chamber in the main beam pipe coordinate system
	farbwd_tube = new G4UnionSolid("farbwd_tube",farbwd_tube,lumi_tube,lumi_rot,G4ThreeVector(-dK,0,-L-dL));

	// Cut the FarBWD beam pipe at the end of B2eR
	farbwd_beampipeVac_solid = new G4SubtractionSolid("farbwd_beampipeVac_solid",
		farbwd_tube,beampipe_cut,cut_rot,G4ThreeVector(0,0,-(L+(10*m)/cos(B2eR_bend))));

	// Place the assembly
	farbwd_beampipeVac_log = new G4LogicalVolume(farbwd_beampipeVac_solid,mat_vac,"farbwd_beampipeVac_log");
	farbwd_beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut())); 
	G4ThreeVector far_bwd_center(	B2eR_end.x() + 0.5*farbwd_len*sin(B2eR_bend),
					B2eR_end.y(),
					B2eR_end.z() + 0.5*farbwd_len*cos(B2eR_bend));
	new G4PVPlacement(farbwd_rot, 
		G4ThreeVector(far_bwd_center.x(),far_bwd_center.y(),far_bwd_center.z()),
		farbwd_beampipeVac_log,"vac_farbwd_beampipeVac",world_log,false,0,true);
	// print far-bwd center coordinates
	printf("[INFO] Far-BWD center: (%.10f,%.10f,%.10f) [cm]",
		far_bwd_center.x()/cm,
		far_bwd_center.y()/cm,
		far_bwd_center.z()/cm);
	// store far-bwd coordinates for tagger reference
	runAction->_sensRefPosX = far_bwd_center.x()/cm;
	runAction->_sensRefPosY = far_bwd_center.y()/cm;
	runAction->_sensRefPosZ = far_bwd_center.z()/cm;
	runAction->_sensRefAngl = B2eR_bend/rad;

	// Visualization
	G4VisAttributes* farbwd_beampipeVac_vis = new G4VisAttributes(G4Color::Gray());
	farbwd_beampipeVac_vis->SetVisibility(visibility);
	farbwd_beampipeVac_log->SetVisAttributes(farbwd_beampipeVac_vis);
	//---------------------------------------------------------------------------------------------------------------//
	// Place an absorber inside the FarBWD vacuum
	if(addAbsorber && farbwd_beampipeVac_log)
	{
		G4Tubs* abs_tube = new G4Tubs("abs_tube_farbwd",0,B2eR_beampipe_radius,1*cm,0, 2.0 * M_PI * rad);
		G4LogicalVolume* abs_log = new G4LogicalVolume(abs_tube,mat_vac, "abs_log_farbwd");
		new G4PVPlacement(0,G4ThreeVector(0,0,(farbwd_len+farbwd_ext)/2. - 1 * cm),
			abs_log,"abs_farbwd",farbwd_beampipeVac_log,false,0,true);

		//- Visualization
		G4VisAttributes* abs_vis = new G4VisAttributes(G4Color::Magenta());
		abs_vis->SetVisibility(visibility);
		abs_log->SetVisAttributes(abs_vis);
	}
	//---------------------------------------------------------------------------------------------------------------//
	// Place a lumi window
	G4double lumiWindStart_posz = B2eR_end.z() + B2eR_cut_offset + lumi_halflen;
	G4cout<<"[INFO] lumiWindStart_posz = "<<lumiWindStart_posz/cm<<" [cm]"<<G4endl;
	G4Box* lumiWind_box = new G4Box("lumiWind_box",B2eR_beampipe_radius,B2eR_beampipe_radius,1 * cm);
	G4LogicalVolume* lumiWind_log = new G4LogicalVolume(lumiWind_box,mat_vac,"lumiWind_log");
	new G4PVPlacement(0,G4ThreeVector(0,0,lumiWindStart_posz+ 1*cm),lumiWind_log,"lumiWind",world_log,false,0,true);
	
	return;
}

void DetectorConstruction::BuildTaggers()
{
	if(!farbwd_beampipeVac_log)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildTaggers ==> far-bwd beam pipe LV is not created\n");
	}

	//- Visualization
	G4VisAttributes* tag_vis = new G4VisAttributes(G4Color::Yellow());
	tag_vis->SetVisibility(visibility);
	G4Box* tag_box = new G4Box("tag_box",18.0 * cm /2, ante_chmbr_H/2., 1 * mm);
	G4LogicalVolume* tag_log = new G4LogicalVolume(tag_box,mat_vac,"tag_log");
	tag_log->SetVisAttributes(tag_vis);

	// Tagger 1
	new G4PVPlacement(0,G4ThreeVector(B2eR_beampipe_radius+18.0 * cm /2,0,tag1_posZ),
		tag_log,"sens_tag1",farbwd_beampipeVac_log,false,0,true);

	// Tagger 2
	new G4PVPlacement(0,G4ThreeVector(B2eR_beampipe_radius+18.0 * cm /2,0,farbwd_len/2. - 1*mm),
		tag_log,"sens_tag2",farbwd_beampipeVac_log,false,0,true);

	return;
}

void DetectorConstruction::CreateSrMask()
{
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid || !world_log)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::CreateSrMask ==> Materials, beam pipe solids, or world LV are not created\n");
	}
/*
	//- mask-0
	G4VSolid* mask0_vol = new G4Box("mask0_vol",1.0 * m/2,1.0 * m/2,1.0 * cm /2);
	G4VSolid* mask0_ring = new G4Tubs("mask0_ring", 0, 5.0 * cm, 1.0 * cm/2, 0, 2.0 * M_PI);

	G4double dz = 50.0 * cm;
	G4double posZ = -2300.0 * cm + dz;
	while(posZ < -1300.0 * cm - dz)
	{
		//- Vacuum (subtract the mask volume from the beam pipe volume) and add a new pipe with the mask
		beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
			beampipeVac_solid,mask0_vol,0,G4ThreeVector(0,0,posZ));

		//- Mask logic volume
		G4LogicalVolume* mask0_log = new G4LogicalVolume(mask0_ring,mat_vac, "mask0_log");
		mask0_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut()));

		//- Set tracking time limit in the volume
		new G4PVPlacement(
			0,G4ThreeVector(0.,0.,posZ),mask0_log,"vac_mask0",world_log,false,0,true);
		G4VisAttributes* mask0_vis = new G4VisAttributes(G4Color::Red());
		mask0_vis->SetVisibility(visibility);
		mask0_log->SetVisAttributes(mask0_vis);

		posZ += dz;
	}	

	//- mask-1
	G4VSolid* mask1_ring = new G4Tubs("mask1_ring", 2.0 * cm, 4.0 * cm, 1.0 * cm/2, 0, 2.0 * M_PI);

	posZ = -700.0 * cm;
	while(posZ <= -550.0 * cm)
	{
		//- Vacuum (subtract the mask ring from the beam pipe)
		beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
			beampipeVac_solid,mask1_ring,0,G4ThreeVector(0,0,posZ));
		beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
			beampipeUni_solid,mask1_ring,0,G4ThreeVector(0,0,posZ));

		posZ += dz;
	}
*/
/*
	//- mask-3
	const G4double  zPlane[] = {-720.0 * cm, -710.0 * cm};
	const G4double  rInner[] = {2.4 * cm, 2.7 * cm};
	const G4double  rOuter[] = {3.0 * cm, 3.0 * cm};
	G4VSolid* mask3_polycone = new G4Polycone("mask3_polycone", 0, 2.0 * M_PI, 2, zPlane, rInner, rOuter); 

	//- Vacuum (subtract the mask ring from the beam pipe)
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,mask3_polycone,0,G4ThreeVector(0,0,0));
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,mask3_polycone,0,G4ThreeVector(0,0,0));

	//- Visualization
//	G4LogicalVolume* mask3_log = new G4LogicalVolume(mask3_polycone, mat_vac, "mask3_log");
//	new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),mask3_log,"mask3",world_log,false,0,true);
//	G4VisAttributes* mask3_vis = new G4VisAttributes(G4Colour::Red());
//	mask3_log->SetVisAttributes(mask3_vis);
*/

	//- mask-5
	const G4double  zPlane[] = {-4660.0 * mm, -4453.0 * mm, -4300.0 * mm};
	const G4double  rInner[] = {54/2. * mm, 43/2. * mm, 62/2. * mm};
	const G4double  rOuter[] = {(62+1)/2. * mm, (62+1)/2. * mm, (62+1)/2. * mm};
	G4VSolid* mask5_polycone = new G4Polycone("mask5_polycone", 0, 2.0 * M_PI, 3, zPlane, rInner, rOuter); 

	//- Vacuum (subtract the mask ring from the beam pipe)
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,mask5_polycone,0,G4ThreeVector(0,0,0));
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,mask5_polycone,0,G4ThreeVector(0,0,0));

	//- Visualization
//	G4LogicalVolume* mask5_log = new G4LogicalVolume(mask5_polycone, mat_vac, "mask5_log");
//	new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),mask5_log,"mask5",world_log,false,0,true);
//	G4VisAttributes* mask5_vis = new G4VisAttributes(G4Colour::Red());
//	mask5_log->SetVisAttributes(mask5_vis);



	return;
}

void DetectorConstruction::BuildMagnets()
{
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid || !world_log || !bwd_beampipeVac_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildMagnets ==> Materials, beam pipe solids, or world LV are not created\n");
	}
	// Particles
	BuildParticles();

	//- Magnets (fill the beam pipe volume with the magnetic field)
	for(int iMag = 0; iMag < simPar->GetMagNum(); iMag++)
	{
		// magnet virtual volume
		G4VSolid* mag_vol;
		// magnet center coordinates
		G4ThreeVector mag_center;
		// rotation matrix
		G4RotationMatrix* mag_rot = new G4RotationMatrix();
		// magnet solid object
		G4VSolid* mag_solid;

		if(simPar->GetMagType(iMag) == "D-pole" || simPar->GetMagType(iMag) == "Q-pole")
		{
			mag_vol = new G4Box(simPar->GetMagName(iMag)+"_vol",
				1*m/2,1*m/2,simPar->GetMagLength(iMag)/2); 

			// vector of start coordinates in the global ref. system shifted w.r.t. the ip
			G4ThreeVector magVector1(	simPar->GetMagStartPosX(iMag) - simPar->GetIpPosX(),
							simPar->GetMagStartPosY(iMag) - simPar->GetIpPosY(),
							simPar->GetMagStartPosZ(iMag) - simPar->GetIpPosZ());
			// vector of end coordinates in the global ref. system shifted w.r.t. the ip
			G4ThreeVector magVector2(	simPar->GetMagEndPosX(iMag) - simPar->GetIpPosX(),
							simPar->GetMagEndPosY(iMag) - simPar->GetIpPosY(),
							simPar->GetMagEndPosZ(iMag) - simPar->GetIpPosZ());
			// rotate coordinate vectors from global to local/ip system
			magVector1.rotateY(-simPar->GetIpTheta());
			magVector2.rotateY(-simPar->GetIpTheta());
			// check if the magnet length is the same as in the XML
			G4double L = sqrt(pow(magVector2.x()-magVector1.x(),2)+pow(magVector2.z()-magVector1.z(),2));
			if((L - simPar->GetMagLength(iMag))/simPar->GetMagLength(iMag) > 1e-4)
			{
				G4cout<<"\n\n=================================================================="<<G4endl;
				G4cout<<"[WARNING] DetectorConstruction::BuildMagnets"<<G4endl;
				G4cout<<"Not accurate magnet length calculation"<<G4endl;
				G4cout<<"L (calc) = "<<L/m<<" [m]"<<G4endl;
				G4cout<<"L (xml)  = "<<simPar->GetMagLength(iMag)/m<<" [m]"<<G4endl;
				G4cout<<"==================================================================\n"<<G4endl;
			}
				
			// coordinates of the magnet center
			mag_center = G4ThreeVector(	magVector1.x() + (magVector2.x()-magVector1.x())/2,
							magVector1.y() + (magVector2.y()-magVector1.y())/2,
							magVector1.z() + (magVector2.z()-magVector1.z())/2);

			// orientation angle of the magnet
			G4double rotY_angle = atan((magVector2.x()-magVector1.x())/(magVector2.z()-magVector1.z()))*rad;
			mag_rot->rotateY(-rotY_angle);

			if(simPar->GetMagName(iMag) == "D2ER")
			{
				mag_solid = new G4IntersectionSolid(simPar->GetMagName(iMag)+"_solid",
					bwd_beampipeVac_solid, mag_vol,mag_rot,mag_center);
			}
			else if(simPar->GetMagName(iMag) == "Q3ER")
			{
				mag_solid = mag_vol;
				G4Tubs* mag_tube = new G4Tubs("tube",0.0,20*cm,10*m,0.,2*M_PI*rad); 
			
				mag_solid = new G4SubtractionSolid(simPar->GetMagName(iMag)+"_solid",
					mag_solid,mag_tube,0,G4ThreeVector(0,0,0));
			}
			else
			{
				mag_solid = new G4IntersectionSolid(simPar->GetMagName(iMag)+"_solid",
					beampipeUni_solid, mag_vol,mag_rot,mag_center);
			}
		}
		else if(simPar->GetMagType(iMag) == "SOL")
		{
			if(!simPar->IsBuildSolenoid()){continue;}

			mag_vol = new G4Tubs(simPar->GetMagName(iMag)+"_vol",
				0.0,simPar->GetSolRmax(),(simPar->GetSolZmax()-simPar->GetSolZmin())/2.0,0.,2*M_PI*rad); 
			// coordinates of the magnet center
			mag_center = simPar->GetSolOrigVec(); 

			if(!ip_box)
			{
				throw runtime_error(
				"[ERROR] DetectorConstruction::BuildMagnets ==> IP beam pipe or magnet are not created\n");
			}

			G4VSolid* vac_wo_ip_solid = new G4SubtractionSolid("vac_wo_ip_solid", 
				beampipeUni_solid, ip_box,0,
			G4ThreeVector(	0,
					0,
					simPar->GetIpBeampipeZmin()+
						(simPar->GetIpBeampipeZmax()-simPar->GetIpBeampipeZmin())/2));
			mag_solid = new G4IntersectionSolid(simPar->GetMagName(iMag)+"_solid",
				vac_wo_ip_solid, mag_vol,mag_rot,mag_center);
		}
		else
		{
			throw runtime_error(
			"[ERROR] DetectorConstruction::BuildMagnets ==> Wrong magnet type\n");
		}

		// magnet logic volume
		G4LogicalVolume* mag_log = new G4LogicalVolume(mag_solid,mat_vac, simPar->GetMagName(iMag)+"_log");
		// set tracking time limit in the volume
		mag_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut()));
		// magnet placement
		if(simPar->GetMagName(iMag) == "Q3ER")
		{
			new G4PVPlacement(mag_rot,mag_center,
				mag_log,"mag_"+simPar->GetMagName(iMag),world_log,false,0,true);
		}
		else
		{
			new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),
				mag_log,"mag_"+simPar->GetMagName(iMag),world_log,false,0,true);
		}

		//- Create magnetic field
		G4MagneticField* mag_field = GetMagField(iMag,mag_log); 

		// apply the field to logic volume
		G4FieldManager* localFieldMgr = new G4FieldManager(mag_field);
		localFieldMgr->SetDetectorField(mag_field);
		localFieldMgr->CreateChordFinder(mag_field);
		mag_log->SetFieldManager(localFieldMgr, true);

		// apply the solenoid field to the IP beam pipe
		if(simPar->GetMagType(iMag) == "SOL" && simPar->IsBuildSolenoid())
		{
			if(!ip_log)
			{
				throw runtime_error(
				"[ERROR] DetectorConstruction::BuildMagnets ==> IP beam pipe is not built\n");
			}
			ip_log->SetFieldManager(localFieldMgr, true); 
		}

		//- Vacuum (subtract the magnet volume from the beam pipe volume)
		if(mag_center.z() > 15 * m) // Far-BWD side 
		{
			if(simPar->GetMagName(iMag) == "D2ER")
			{
				bwd_beampipeVac_solid = new G4SubtractionSolid("bwd_beampipeVac_solid",
					bwd_beampipeVac_solid,mag_vol,mag_rot,mag_center);

				// Place B2eR yoke
				G4VSolid* yoke_solid = new G4Box("yoke_solid",
					80 * cm / 2., 40 * cm / 2., simPar->GetMagLength(iMag)/2);
				G4Box* yoke_cut = new G4Box("yoke_cut",
					70 * cm / 2., 10 * cm / 2. + 3 * mm, 100 * m);
				yoke_solid = new G4SubtractionSolid(simPar->GetMagName(iMag)+"_yoke",
					yoke_solid, yoke_cut, 0, G4ThreeVector(0,0,0)); 

				yoke_cut = new G4Box("yoke_cut",
					20 * cm / 2., 30 * cm / 2., 100 * m);
				yoke_solid = new G4SubtractionSolid(simPar->GetMagName(iMag)+"_yoke",
					yoke_solid, yoke_cut, 0, 
					G4ThreeVector(30 * cm / 2. + 20 * cm / 2.,0,0)); 
				yoke_solid = new G4SubtractionSolid(simPar->GetMagName(iMag)+"_yoke",
					yoke_solid, yoke_cut, 0, 
					G4ThreeVector(-30 * cm / 2. - 20 * cm / 2.,0,0)); 

				G4LogicalVolume* yoke_log = new G4LogicalVolume(yoke_solid,mat_vac,
					simPar->GetMagName(iMag)+"_yoke_log");
				new G4PVPlacement(mag_rot,mag_center,yoke_log,"yoke_"+simPar->GetMagName(iMag),
					world_log,false,0,true);
				G4VisAttributes* yoke_vis = new G4VisAttributes(G4Color::Magenta());
				yoke_vis->SetVisibility(visibility);
				yoke_log->SetVisAttributes(yoke_vis);
			}
		}
		else
		{
			beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
				beampipeVac_solid,mag_vol,mag_rot,mag_center);
		}
	}

	return;
}

G4MagneticField* DetectorConstruction::GetMagField(G4int iMag, G4LogicalVolume* mag_log)
{
	if(mass < 0 || gamma < 0)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::GetMagField ==> Particles are not built\n");
	}

	G4MagneticField* mag_field;
	if(simPar->GetMagType(iMag) == "D-pole")
	{
		// By[T]*rho[m] = 3.33564*pc[GeV]
		// rho[m] = l[m]/(2*sin(theta[rad]/2)) (RBEND)
		// pc = mc^2*sqrt(gamma^2 - 1)
		G4double By = ((3.33564*(mass/GeV)*sqrt(pow(gamma,2)-1.0))/
			((simPar->GetMagLength(iMag)/m)/(2.*sin(simPar->GetMagAngle(iMag)/2.))))*tesla;
		mag_field = new G4UniformMagField(G4ThreeVector(0,By,0));
		//- Visualization
		G4VisAttributes* mag_vis = new G4VisAttributes(G4Color::Blue());
		mag_vis->SetVisibility(visibility);
		mag_log->SetVisAttributes(mag_vis);
	}
	else if(simPar->GetMagType(iMag) == "Q-pole")
	{
		// k1[1/m^2] = 0.2998*grad[T/m]/pc[GeV], k1[1/m^2] = k1l[1/m]/l[m]
		G4double grad = (3.33564*(simPar->GetMagK1L(iMag)/(1/m))/(simPar->GetMagLength(iMag)/m)*
			(mass/GeV)*sqrt(pow(gamma,2)-1.0))*(tesla/m);
		mag_field = new G4QuadrupoleMagField(grad);
		//- Visualization
		G4VisAttributes* mag_vis = new G4VisAttributes(G4Color::Yellow());
		mag_vis->SetVisibility(visibility);
		mag_log->SetVisAttributes(mag_vis);
	}
	else if(simPar->GetMagType(iMag) == "SOL" && simPar->IsBuildSolenoid())
	{
		// rotation matrix
		G4RotationMatrix* sol_rot = new G4RotationMatrix();
		// flip XZ coordinates from EIC to Geant4
		if(simPar->IsFlipXZSolenoid()){sol_rot->rotateY(M_PI*rad);}

		mag_field = new SolenoidMagField(	simPar->GetSolFileName(),
							simPar->GetSolRmax(),
							simPar->GetSolZmin(),
							simPar->GetSolZmax(),
							simPar->IsPrintSolenoid(),
							simPar->GetSolOrigVec(),
							sol_rot); 
		//- Visualization
		G4VisAttributes* mag_vis = new G4VisAttributes(G4Colour::White());
		mag_vis->SetVisibility(visibility);
		mag_log->SetVisAttributes(mag_vis);
	}
	else
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::GetMagField ==> Wrong magnet type\n");
	}

	return mag_field;
}

void DetectorConstruction::BuildParticles()
{
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particleDef;
        particleDef = particleTable->FindParticle(simPar->GetBeamName().c_str());
        mass = particleDef->GetPDGMass();
        gamma = simPar->GetBeamGamma();

	return;
}

void DetectorConstruction::BuildIpBeamPipe()
{
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid || !world_log)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildIpBeamPipe ==> Materials, beam pipe solids, or world LV are not created\n");
	}

	//- IP beam pipe
	G4double ip_len = abs(simPar->GetIpBeampipeZmax()-simPar->GetIpBeampipeZmin());
	ip_box = new G4Box("ip_box",1*m/2,1*m/2,ip_len/2.); 
	G4VSolid* ip_solid = new G4Tubs("ip_solid",0, 62.0/2 * mm, ip_len/2., 0, 2.0 * M_PI);
	ip_log = new G4LogicalVolume(ip_solid,mat_vac,"ip_log");
	new G4PVPlacement(0, 
		G4ThreeVector(0.,0.,simPar->GetIpBeampipeZmin()+ip_len/2.),
		ip_log,"vac_IP",world_log,false,0,true);
	//- Visualization
	G4VisAttributes* ip_vis = new G4VisAttributes(G4Colour::Cyan());
	ip_vis->SetVisibility(visibility);
	ip_log->SetVisAttributes(ip_vis);

	//- Vacuum (subtract the ip beam pipe volume from the beam pipe volume)
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",beampipeVac_solid,ip_box,
		0,G4ThreeVector(0.,0.,simPar->GetIpBeampipeZmin()+ip_len/2));

	return;
}

void DetectorConstruction::BuildIrBeamPipe()
{
	if(!mat_vac || !beampipeVac_solid || !world_log || !bwd_beampipeVac_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildIrBeamPipe ==> Materials, beam pipe solids, or world LV are not created\n");
	}

	//- Vacuum (place the beam pipe without magnet and IP volumes) 
	beampipeVac_log = new G4LogicalVolume(beampipeVac_solid, mat_vac,"beampipeVac");
	// set tracking time limit in the volume
	beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut()));
	new G4PVPlacement(0,G4ThreeVector(0,0,0),
		beampipeVac_log,"vac_beampipe",world_log,false,0,true);
	//- Visualization
	G4VisAttributes* beampipeVac_vis = new G4VisAttributes(G4Color::Grey());
	beampipeVac_vis->SetVisibility(visibility);
	beampipeVac_log->SetVisAttributes(beampipeVac_vis);

	//- Vacuum BWD beam pipe vacuum
	G4LogicalVolume* bwd_beampipeVac_log = new G4LogicalVolume(
		bwd_beampipeVac_solid, mat_vac, "bwd_beampipeVac");
	bwd_beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut())); 
	new G4PVPlacement(0,G4ThreeVector(0,0,0),
		bwd_beampipeVac_log,"vac_bwd_beampipeVac",world_log,false,0,true);
	//- Visualization
	G4VisAttributes* bwd_beampipeVac_vis = new G4VisAttributes(G4Color::Gray());
	bwd_beampipeVac_vis->SetVisibility(visibility);
	bwd_beampipeVac_log->SetVisAttributes(bwd_beampipeVac_vis);

	return;
}

void DetectorConstruction::BuildFarBackwardBeamPipe2()
{

	G4double Q3eR_Theta = 0.018137692 * rad;
	G4double Q3eR_StartX = 45.451 * cm;
	G4double Q3eR_StartZ = 3756.99 * cm;
	G4double B2eR_FocX = 0.0 * cm;
	G4double B2eR_FocZ = Q3eR_StartZ - (Q3eR_StartX - B2eR_FocX)/tan(Q3eR_Theta);
	G4double Drift_Len = sqrt(pow((Q3eR_StartX - B2eR_FocX),2) + pow(Q3eR_StartZ - B2eR_FocZ,2));
	G4double Drift_Rad = 196.0/2 * mm;
	G4double Drift_CenterX = Q3eR_StartX - Drift_Len * sin(Q3eR_Theta)/2;
	G4double Drift_CenterZ = Q3eR_StartZ - Drift_Len * cos(Q3eR_Theta)/2;  
	G4double Drift_CenterY = 0.0 * mm;
	G4double Vac_EndZ = 1527.0 * cm;
	G4double B2eR_EndZ = 1500.0 * cm;
	G4double B2eR_FocL = B2eR_EndZ-B2eR_FocZ;
	G4double Tag1_ThetaMax = atan(Drift_Rad/B2eR_FocL);
	G4double Tag2_ThetaMax = 0;
	G4double TaggerSens_Depth = 13.0 * cm;
	G4double TaggerBox_Width = 16.0 * cm;
	G4double TaggerBox_Height = 18.0 * cm;
	G4double TaggerBox_Length = 10.0 * 3 * cm;
	G4double BeamPipe_Thickness = 3.0 * mm;
	G4double TaggerBox_Limit = Drift_Rad + BeamPipe_Thickness;
	
	// to found the tagger 2 max theta we have to solve the equition that does not have an analytical solution
	// so, to solve it, we can follow a numerical method: find the function's value atthe given point
	TGraph* gr_Tag2_ThetaMax = new TGraph();
	for(G4double theta = Q3eR_Theta; theta < Tag1_ThetaMax; theta += 1e-6)
	{
		gr_Tag2_ThetaMax->SetPoint(	gr_Tag2_ThetaMax->GetN(),
						(TaggerSens_Depth/tan(Tag1_ThetaMax - theta) - TaggerBox_Length) * 
							sin(theta-Q3eR_Theta)/TaggerBox_Limit,
						theta);
	}
	Tag2_ThetaMax = gr_Tag2_ThetaMax->Eval(1);
	delete gr_Tag2_ThetaMax;
	// the tagger 2 min theta
	G4double Tag2_ThetaMin = atan(sin(Tag2_ThetaMax-Q3eR_Theta)/(TaggerSens_Depth/TaggerBox_Limit + cos(Tag2_ThetaMax-Q3eR_Theta))) + Q3eR_Theta;

	//-------------------------------------------------------------------------------------------------//
	//- Tagger 1
	// coordinates of the tagger 1 box center 
	G4double Tag1_BoxCenterL = 
		sqrt(	
			pow(TaggerBox_Length/2. + TaggerBox_Limit/sin(Tag2_ThetaMax-Q3eR_Theta),2) + 
			pow(TaggerBox_Width/2.,2)
			);
	G4double Tag1_BoxCenterTheta = 
		atan(TaggerBox_Width/2./(TaggerBox_Length/2. + TaggerBox_Limit/sin(Tag2_ThetaMax-Q3eR_Theta))) + 
		Tag2_ThetaMax;
	G4double Tag1_BoxCenterX = B2eR_FocX + Tag1_BoxCenterL * sin(Tag1_BoxCenterTheta); 
	G4double Tag1_BoxCenterZ = B2eR_FocZ + Tag1_BoxCenterL * cos(Tag1_BoxCenterTheta); 
	//-------------------------------------------------------------------------------------------------//
	//- Tagger 2
	// coordinates of the tagger 2 box center
	G4double Tag2_BoxNearL = TaggerBox_Limit/sin(Tag2_ThetaMin-Q3eR_Theta);
	G4double Tag2_BoxDiagL = sqrt(pow(TaggerBox_Length/2.,2) + pow(TaggerBox_Width/2.,2));
	// c^2 = a^2 + b^2 - 2*a*b*cos(c)
	G4double Tag2_BoxCenterL = sqrt(
					pow(Tag2_BoxNearL,2) + 
					pow(Tag2_BoxDiagL,2) -
					2. * Tag2_BoxNearL*Tag2_BoxDiagL * 
						cos(
							M_PI_2 - (Tag2_ThetaMax-Tag2_ThetaMin) + 
							atan(TaggerBox_Length/TaggerBox_Width)
							)
					);
	// sin(b)/b = sin(c)/c
	G4double Tag2_BoxCenterTheta = 
		asin(
			(Tag2_BoxDiagL/Tag2_BoxCenterL) * 
			sin(M_PI_2 - (Tag2_ThetaMax-Tag2_ThetaMin) + atan(TaggerBox_Length/TaggerBox_Width))
		) + Tag2_ThetaMin;
	G4double Tag2_BoxCenterX = B2eR_FocX + Tag2_BoxCenterL * sin(Tag2_BoxCenterTheta); 
	G4double Tag2_BoxCenterZ = B2eR_FocZ + Tag2_BoxCenterL * cos(Tag2_BoxCenterTheta); 
	//-------------------------------------------------------------------------------------------------//
	// Place taggers
	G4Box* Tag_box = new G4Box("Tag_box",TaggerBox_Width/2.,TaggerBox_Height/2.,TaggerBox_Length/2.);
	G4LogicalVolume* Tag1_log = new G4LogicalVolume(Tag_box,mat_vac,"Tag1_log");
	G4LogicalVolume* Tag2_log = new G4LogicalVolume(Tag_box,mat_vac,"Tag2_log");
	G4RotationMatrix* Tag_rot = new G4RotationMatrix();
	Tag_rot->rotateY(-Tag2_ThetaMax);
	new G4PVPlacement(Tag_rot,G4ThreeVector(Tag1_BoxCenterX,0,Tag1_BoxCenterZ),
		Tag1_log,"Tag1",world_log,false,0,true);
	new G4PVPlacement(Tag_rot,G4ThreeVector(Tag2_BoxCenterX,0,Tag2_BoxCenterZ),
		Tag2_log,"Tag2",world_log,false,0,true);
	//-------------------------------------------------------------------------------------------------//
	// Place sensor planes
	G4double Sens_Thickness = 1.0 * cm;
	G4Box* Sens_box = new G4Box("Sens_box",TaggerBox_Width/2.,TaggerBox_Height/2.,Sens_Thickness/2.);
	G4LogicalVolume* Sens_log = new G4LogicalVolume(Sens_box,mat_vac,"Sens_log");
	new G4PVPlacement(0,G4ThreeVector(0,0,-TaggerBox_Length/2.+Sens_Thickness/2.),Sens_log,"tag1_sens1",Tag1_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,-TaggerBox_Length/6.+Sens_Thickness/2.),Sens_log,"tag1_sens1",Tag1_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,+TaggerBox_Length/2.-Sens_Thickness/2.),Sens_log,"tag1_sens2",Tag1_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,+TaggerBox_Length/6.-Sens_Thickness/2.),Sens_log,"tag1_sens2",Tag1_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,-TaggerBox_Length/2.+Sens_Thickness/2.),Sens_log,"tag2_sens1",Tag2_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,-TaggerBox_Length/6.+Sens_Thickness/2.),Sens_log,"tag2_sens1",Tag2_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,+TaggerBox_Length/2.-Sens_Thickness/2.),Sens_log,"tag2_sens2",Tag2_log,false,0,true);
	new G4PVPlacement(0,G4ThreeVector(0,0,+TaggerBox_Length/6.-Sens_Thickness/2.),Sens_log,"tag2_sens2",Tag2_log,false,0,true);
	G4VisAttributes* Sens_vis = new G4VisAttributes(G4Color::Blue());
	Sens_vis->SetVisibility(visibility);
	Sens_log->SetVisAttributes(Sens_vis);
	//-------------------------------------------------------------------------------------------------//
	// Place an interface beam pipe
	// length of the interface beam pipe
	G4double InterfaceBeamPipe_Length = TaggerBox_Limit/tan(Tag2_ThetaMin-Q3eR_Theta) - TaggerBox_Limit/tan(Tag1_ThetaMax-Q3eR_Theta);	
	// center position of the interface beam pipe
	G4double InterfaceBeamPipe_CenterL = (TaggerBox_Limit/tan(Tag1_ThetaMax-Q3eR_Theta) + InterfaceBeamPipe_Length/2.);
	G4double InterfaceBeamPipe_CenterX = B2eR_FocX + InterfaceBeamPipe_CenterL * sin(Q3eR_Theta); 
	G4double InterfaceBeamPipe_CenterZ = B2eR_FocZ + InterfaceBeamPipe_CenterL * cos(Q3eR_Theta); 

	G4Tubs* Interface_tube = new G4Tubs("Interface_tube", 0, Drift_Rad, InterfaceBeamPipe_Length/2., 0, 2.0 * M_PI);
	G4LogicalVolume* Interface_log = new G4LogicalVolume(Interface_tube, mat_vac,"Interface");
	G4RotationMatrix* Interface_rot = new G4RotationMatrix();
	Interface_rot->rotateY(-Q3eR_Theta);
	new G4PVPlacement(Interface_rot,G4ThreeVector(InterfaceBeamPipe_CenterX,0,InterfaceBeamPipe_CenterZ),
		Interface_log,"vac_interface",world_log,false,0,true);
	G4VisAttributes* Interface_vis = new G4VisAttributes(G4Color::Cyan());
	Interface_vis->SetVisibility(visibility);
	Interface_log->SetVisAttributes(Interface_vis);
	//-------------------------------------------------------------------------------------------------//
	// Print geometry parameters
	G4cout<<" B2eR_FocX = "<<B2eR_FocX/mm<<" [mm]; B2eR_FocZ = "<<B2eR_FocZ/mm<<" [mm]"<<G4endl;
	G4cout<<" Tag1_ThetaMax = "<<Tag1_ThetaMax/rad<<" [rad]; Tag2_ThetaMax = "<<Tag2_ThetaMax/rad<<" [rad]; Tag2_ThetaMin = "<<Tag2_ThetaMin/rad<<" [rad]"<<G4endl;
	G4cout<<" Tag1_BoxCenterX = "<<Tag1_BoxCenterX/mm<<" [mm]; Tag1_BoxCenterZ = "<<Tag1_BoxCenterZ/mm<<" [mm]"<<G4endl;
	G4cout<<" Tag2_BoxCenterX = "<<Tag2_BoxCenterX/mm<<" [mm]; Tag2_BoxCenterZ = "<<Tag2_BoxCenterZ/mm<<" [mm]"<<G4endl;
	G4cout<<" InterfaceBeamPipe_Length = "<<InterfaceBeamPipe_Length/mm<<" [mm]"<<G4endl;
	G4cout<<" InterfaceBeamPipe_CenterX = "<<InterfaceBeamPipe_CenterX/mm<<" [mm]; InterfaceBeamPipe_CenterZ = "<<InterfaceBeamPipe_CenterZ/mm<<" [mm]"<<G4endl;
	//-------------------------------------------------------------------------------------------------//

	G4VSolid* Drift_chamber = new G4Tubs("Drift_chamber", 0, Drift_Rad, Drift_Len/2, 0, 2.0 * M_PI);
	G4Box* Drift_cut1 = new G4Box("Drift_cut1",10 * m, 10 * m, 10 * m);
	G4Box* Drift_cut2 = new G4Box("Drift_cut2",10 * m, 10 * m, InterfaceBeamPipe_Length/2.);

	G4RotationMatrix* Cut_rot = new G4RotationMatrix();
	Cut_rot->rotateY(Q3eR_Theta);
	// subtract the central beam pipe 
	Drift_chamber = new G4SubtractionSolid("Drift_chamber",Drift_chamber,beampipeVac_solid,
		Cut_rot,G4ThreeVector(-Drift_CenterX,-Drift_CenterY,-Drift_CenterZ));
	// cut along the beam axis
	Drift_chamber = new G4SubtractionSolid("Drift_chamber",Drift_chamber,Drift_cut1,
		Cut_rot,G4ThreeVector(0,0,-(Drift_CenterZ - (B2eR_EndZ + 5 * mm) + 10 * m) / cos(Q3eR_Theta)));
	G4RotationMatrix* Drift_rot = new G4RotationMatrix();
	Drift_rot->rotateY(-Q3eR_Theta);

	// cut the fwd side of the central beam pipe 
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,Drift_cut1,0,G4ThreeVector(0,0,B2eR_EndZ + 5 * mm + 10 * m));
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,Drift_cut1,0,G4ThreeVector(0,0,B2eR_EndZ + 5 * mm + 10 * m));

	// subtract the interface beam pipe from the far-bwd chamber
	Drift_chamber = new G4SubtractionSolid("Drift_chamber",Drift_chamber,Drift_cut2,
		0,G4ThreeVector(0,0,InterfaceBeamPipe_CenterL - Drift_Len/2.));
	// place the chamber
	G4LogicalVolume* Drift_log = new G4LogicalVolume(Drift_chamber, mat_vac,"Drift");   

	new G4PVPlacement(Drift_rot,G4ThreeVector(Drift_CenterX,Drift_CenterY,Drift_CenterZ),
		Drift_log,"vac_farbeamline",world_log,false,0,true);

	return;
}

void DetectorConstruction::BuildFarBackwardBeamPipe()
{
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid || !world_box || !world_log)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildFarBackwardBeamPipe ==> Materials, beam pipe/world solids, or world LV are not created\n");
	}

	//- Feb 13, 2024
	// The geometry and numbers are based on 	src/BackwardsTaggers_geo.cpp & 
	//						compact/far_backward/taggers.xml & 
	//						compact/far_backward/definitions.xml 

	G4double Q3eR_InnerRadius = 0.05 * m;
	G4double Q3eR_StartX = -0.460027 * m;
	G4double Q3eR_StartZ = -37.696067 * m;
	G4double Q3eR_EndZ = -38.295969 * m;
	G4double Q3eR_Theta = 0.0180766389 * rad; 
	G4bool addLumi = true;
	G4double B2BeR_InnerRadius = 100.0 * mm;
	G4double B2BeR_Edge = 5.0 * mm; // to avoid overlap between B2BeR and far-backward beampipe
	G4double B2BeR_Length = 5.500075391000 * m;
	G4double B2BeR_CenterPosition = -15.000075390000 * m + B2BeR_Length / 2;
	G4double LumiWin_Zstart = -18.5 * m;
	G4double LumiWin_R = 4.0 * cm;

	G4double Exit_Width = B2BeR_InnerRadius;
	G4double Exit_Height = B2BeR_InnerRadius;
	G4double lumiZ = LumiWin_Zstart;
	G4double Lumi_R = LumiWin_R;
	G4double Beam_Theta = Q3eR_Theta; 
	G4double WidthL = Q3eR_InnerRadius;
	G4double WidthR = Q3eR_InnerRadius;
	G4double Width = (WidthL + WidthR) / 2;
	G4double Backwards_Box_Wall = 2.0 * mm;
	G4double wall = Backwards_Box_Wall;
	G4double Height = Q3eR_InnerRadius;
	G4double Dipole_Focus_X = 0.0 * mm;
	G4double Dipole_Focus_Y = 0.0 * mm;
	G4double Dipole_Focus_Z = Q3eR_StartZ-Q3eR_StartX/tan(Beam_Theta);
	G4double Thickness = (Dipole_Focus_Z-Q3eR_StartZ)/cos(Beam_Theta);
	G4double off = Dipole_Focus_Z;

	//- Make bounding box to make IntersectionSolid with other components
	G4double Vacuum_BB_MinX = -2.5 * m;
	G4double Vacuum_BB_MaxX = 0.5 * m;
	G4double Vacuum_BB_MinY = -1.5 * m;
	G4double Vacuum_BB_MaxY = 1.5 * m;
	G4double Vacuum_BB_MinZ = B2BeR_CenterPosition - B2BeR_Length/2 - B2BeR_Edge;
	G4double Vacuum_BB_MaxZ = Q3eR_EndZ;

	G4double BB_MinX = Vacuum_BB_MinX;
	G4double BB_MinY = Vacuum_BB_MinY;
	G4double BB_MinZ = Vacuum_BB_MinZ;
	G4double BB_MaxX = Vacuum_BB_MaxX;
	G4double BB_MaxY = Vacuum_BB_MaxY;
	G4double BB_MaxZ = Vacuum_BB_MaxZ;
	G4double BB_X = abs(BB_MaxX - BB_MinX);
	G4double BB_Y = abs(BB_MaxY - BB_MinY);
	G4double BB_Z = abs(BB_MaxZ - BB_MinZ);
	G4double exitDist = BB_MinZ - off;
	G4double ED_X = Exit_Width;
	G4double ED_Y = Exit_Height;
	G4double ED_Z = off - lumiZ;

 	// Maximum theta to exit the dipole from
	G4double Exit_Theta = 0.07 * rad;
	G4double maxTheta = Exit_Theta;
	G4double exitTheta = maxTheta;

	//- Central pipe box
	G4Box* Extended_Vacuum_Box = new G4Box("Extended_Vacuum_Box",Width, Height, Thickness); // Simpler box pipe
	G4VSolid* Vacuum_Box = Extended_Vacuum_Box;

  	//- Add Tagger box containers and vacuum box extension for modules
	G4double Tagger1_Width = 147.84 * mm;
	G4double Tagger2_Width = 147.84 * mm;
	G4double Tagger1_Height = 200 * mm;
	G4double Tagger2_Height = 150.0 * mm;
	G4double Tag_Tracker_1_Depth = 305 * mm;
	G4double Tag_Tracker_2_Depth = Tag_Tracker_1_Depth;
	G4double Tagger1_Length = Tag_Tracker_1_Depth;
	G4double Tagger2_Length = Tag_Tracker_2_Depth;
	const G4int nModules = 2;
	G4double Tagger1_Min_Theta = 0.0262 * rad;
	G4double Tagger2_Min_Theta = 0.02 * rad;
	G4double Tagger1_Min_Offset = Q3eR_InnerRadius;
	G4double Tagger2_Min_Offset = Q3eR_InnerRadius+Backwards_Box_Wall;	

	G4double offset_min[] = {Tagger1_Min_Offset,Tagger2_Min_Offset};
	G4double theta_min[] = {Tagger1_Min_Theta,Tagger2_Min_Theta};
	G4double theta_max[] = {0.030 * rad,Tagger1_Min_Theta};
    	G4bool max_align[] = {false,true};
    	G4bool extend_vacuum[] = {true,true};
	// Size of the actual tagger box, replicated in BackwardsTagger
    	G4double w[] = {Tagger1_Width/2,Tagger2_Width/2};
    	G4double h[] = {Tagger1_Height/2,Tagger2_Height/2};
    	G4double tagboxL[] = {Tagger1_Length,Tagger2_Length};
    	// Overlap left beyond theta setting
	G4double overlap[] = {0.0 * mm,Backwards_Box_Wall};

	G4double tag_w[] = {Tagger1_Width/2,Tagger2_Width/2};
	G4double tag_h[] = {Tagger1_Height/2,Tagger2_Height/2};
	G4double layerZ[] = {0 * mm, 0 * mm};
	G4double layerRot[] = {70 * deg, 70 * deg};
	G4double layerThickness[] = {100 * um, 100 * um};

	G4VSolid* Foil_Box_Solid[100];

	//- Loop over taggers 
	for(G4int iMod = 0; iMod < nModules; iMod++)
	{
    		// Offset from the electron beam
    		G4double tagoff  = offset_min[iMod];

    		// Theta coverage expected
    		G4double thetamin = theta_min[iMod] - Beam_Theta;
    		G4double thetamax = theta_max[iMod] - Beam_Theta;

    		// Width and height of vacuum volume
    		auto vac_w = w[iMod];
    		auto vac_h = h[iMod];

    		// Width and height of box volume
    		auto box_w = w[iMod] + wall;
    		auto box_h = h[iMod] + wall;

    		// Angle in relation to the main beam
    		auto theta = thetamin;

    		auto offsetx = -(box_w - wall) * (cos(theta));
    		auto offsetz = (box_w - wall) * (sin(theta));
    		auto vacoffsetx = -vac_w * (cos(theta));
    		auto vacoffsetz = vac_w * (sin(theta));
    		auto l = (tagoff) / (sin(theta)) + tagboxL[iMod];

    		auto tagoffsetx = vacoffsetx - (l) * sin(theta);
    		auto tagoffsetz = vacoffsetz - (l) * cos(theta);

    		// Align box to max or minimum theta expected at the tagger from focal point
    		if(max_align[iMod]) 
		{
      			theta = thetamax;
      			offsetx = (overlap[iMod]+box_w - wall) * (cos(theta));
      			offsetz = -(overlap[iMod]+box_w - wall) * (sin(theta));
      			vacoffsetx = (overlap[iMod]+vac_w) * (cos(theta));
      			vacoffsetz = -(overlap[iMod]+vac_w) * (sin(theta));
      			l = (2 * offsetx + tagoff) / sin(theta);
      			tagoffsetx = vacoffsetx - (l) * sin(theta);
      			tagoffsetz = vacoffsetz - (l) * cos(theta);
    		}

    		G4Box* TagVacBox = new G4Box("TagVacBox", vac_w, vac_h, l);

		G4RotationMatrix* rot_Tag = new G4RotationMatrix();
    		rot_Tag->rotateY(-theta); // opposite to eic-shell

    		//- Extend the beam vacuum around the taggers
    		if(extend_vacuum[iMod]) 
		{
      			Vacuum_Box = new G4UnionSolid("Vacuum_Box", Vacuum_Box, TagVacBox, 
				rot_Tag, G4ThreeVector(vacoffsetx, 0, vacoffsetz));
    		}

		//- Make the tagger solid
		G4Box* Foil_Box = new G4Box("Foil_Box",
			tag_w[iMod] / cos(layerRot[iMod]) - 0.5 * layerThickness[iMod] * tan(layerRot[iMod]), 
			tag_h[iMod], 
			layerThickness[iMod] / 2);
		
		G4RotationMatrix* rotation = new G4RotationMatrix();
		rotation->rotateY(-layerRot[iMod]); // opposite to eic-shell

		G4VSolid* Foil_Box_Sub = new G4IntersectionSolid("Foil_Box_Sub",TagVacBox,Foil_Box,
			rotation,G4ThreeVector(0,0,-l + tagboxL[iMod] + tag_w[iMod] * tan(layerRot[iMod])));
		Foil_Box_Solid[iMod] = new G4IntersectionSolid("Foil_Box_Solid_" + std::to_string(iMod),
			Vacuum_Box,Foil_Box_Sub, rot_Tag,G4ThreeVector(vacoffsetx, 0, vacoffsetz));
	}

	//- Cut off any vacuum right of the main beamline
	G4double xbox = 10 * m;
	G4double ybox = 10 * m;
	G4double zbox = 50 * m;
	G4Box* Cut_Box = new G4Box("Cut_Box",xbox, ybox, zbox);
	Vacuum_Box = new G4IntersectionSolid("Vacuum_Box",Vacuum_Box, Cut_Box, 0, G4ThreeVector(-xbox + Width, 0, 0)); 

	//- Luminosity connecting box
	if(addLumi)
	{
    		G4Box* Entry_Vacuum_Box = new G4Box("Entry_Vacuum_Box",ED_X, ED_Y, ED_Z - wall);
    		G4Tubs* Lumi_Exit = new G4Tubs("Lumi_Exit", 0, Lumi_R, ED_Z, 0, 2.0 * M_PI);

		//- Add entry boxes to the main beamline volume
		G4RotationMatrix* rot_Lumi = new G4RotationMatrix();
		rot_Lumi->rotateY(Beam_Theta); // opposite to eic-shell
		Vacuum_Box = new G4UnionSolid("Vacuum_Box", Vacuum_Box, Entry_Vacuum_Box, 
			rot_Lumi, G4ThreeVector(0,0,0));
		Vacuum_Box = new G4UnionSolid("Vacuum_Box", Vacuum_Box, Lumi_Exit, 
			rot_Lumi, G4ThreeVector(0,0,0));
	}

  	//- Restrict tagger boxes into region defined by exitTheta from the dipole magnet
  	G4double cutX     = (ED_X - exitDist * tan(-Beam_Theta)) * cos(Beam_Theta);
  	G4double cutZ     = (ED_X - exitDist * tan(-Beam_Theta)) * sin(Beam_Theta) + exitDist * cos(Beam_Theta);
  	G4double cutXwall = (ED_X - wall - exitDist * tan(-Beam_Theta)) * cos(Beam_Theta);
  	G4double cutZwall = (ED_X - wall - exitDist * tan(-Beam_Theta)) * sin(Beam_Theta) + exitDist * cos(Beam_Theta);

	G4RotationMatrix* rot_Restrict = new G4RotationMatrix();
	rot_Restrict->rotateY(-exitTheta); // opposite to eic-shell
  	Vacuum_Box = new G4IntersectionSolid("Vacuum_Box", Vacuum_Box, Cut_Box, 
		rot_Restrict, G4ThreeVector(xbox - cutXwall, 0, cutZwall));

	//- Cut solids so they are only in the far backwards box 
	G4Box* Far_Backwards_Box = new G4Box("Far_Backwards_Box", BB_X, BB_Y, BB_Z);

	G4RotationMatrix* rotate2 = new G4RotationMatrix();
	rotate2->rotateY(Beam_Theta); // opposite to eic-shell
	G4ThreeVector position(0, 0, (exitDist - BB_Z) / cos(Beam_Theta)); 
	bwd_beampipeVac_solid = new G4IntersectionSolid("Vacuum_Box_Sub",Vacuum_Box, Far_Backwards_Box, 
		rotate2, position);

	//- Place the far-beamline volume
	G4RotationMatrix* xyzRot = new G4RotationMatrix();
	xyzRot->rotateX(M_PI); // flip from eic-shell to Geant4 coordinate system
	xyzRot->rotateZ(M_PI); // flip from eic-shell to Geant4 coordinate system
	xyzRot->rotateY(-Beam_Theta); // opposite to eic-shell

	G4LogicalVolume* Vacuum_Box_log = new G4LogicalVolume(bwd_beampipeVac_solid,mat_vac, "Vacuum_Box_log");
	// set tracking time limit in the volume
	Vacuum_Box_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut()));

	new G4PVPlacement(xyzRot,G4ThreeVector(-Dipole_Focus_X,Dipole_Focus_Y,-Dipole_Focus_Z),
		Vacuum_Box_log,"vac_FarBackwardBeampipe",world_log,false,0,true);
	G4VisAttributes* Vacuum_Box_vis = new G4VisAttributes(G4Color::Red());
	Vacuum_Box_vis->SetVisibility(visibility);
	Vacuum_Box_log->SetVisAttributes(Vacuum_Box_vis);

	//- Place tagger inside the vacuum (mother volume)
	for(G4int iMod = 0; iMod < nModules; iMod++)
	{
		G4LogicalVolume* Foil_Box_log = new G4LogicalVolume(Foil_Box_Solid[iMod],mat_vac,
			"TaggerFoil_" + std::to_string(iMod));
		new G4PVPlacement(0,G4ThreeVector(0,0,0),Foil_Box_log,
			"vac_TaggerFoil_" + std::to_string(iMod),Vacuum_Box_log,false,0,true);
		G4VisAttributes* Foil_Box_vis = new G4VisAttributes(G4Color::Yellow());
		Foil_Box_vis->SetVisibility(visibility);
		Foil_Box_log->SetVisAttributes(Foil_Box_vis);
	}

	//- Place Lumi Exit Window
	if(addLumi)
	{
		G4double LumiExitWindowVac_thickness = 10.0 * mm;
		G4double LumiExitWindowVac_width = 100.0 * mm;
    		G4Box* LumiExitWindow_Solid = new G4Box("LumiExitWindow_Solid", 
			LumiExitWindowVac_width/2.,
			LumiExitWindowVac_width/2., 
			LumiExitWindowVac_thickness/2.);
		G4LogicalVolume* LumiExitWindow_log = new G4LogicalVolume(LumiExitWindow_Solid,mat_vac,
			"LumiExitWindow_log");
		// set tracking time limit in the volume
		LumiExitWindow_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut()));

		new G4PVPlacement(0,
			G4ThreeVector(	-Dipole_Focus_X,
					 Dipole_Focus_Y,
					-Dipole_Focus_Z + (ED_Z + LumiExitWindowVac_thickness/2.)),
			LumiExitWindow_log,"vac_LumiExitWindow",world_log,false,0,true);

		G4VisAttributes* LumiExitWindow_vis = new G4VisAttributes(G4Color::Yellow());
		LumiExitWindow_vis->SetVisibility(visibility);
		LumiExitWindow_log->SetVisAttributes(LumiExitWindow_vis);
	}

	//- Place Absorber at the end of the far-bwd vacuum beamline
	if(addAbsorber)
	{
		G4double absSizeX = 30.0 * cm;
		G4double absSizeY = 30.0 * cm;
		G4double absSizeZ = 5.0 * cm;
		G4Box* abs_box = new G4Box("abs_box_far-bwd",absSizeX/2,absSizeY/2,absSizeZ/2);
		//- Subtract vacuum far-bwd beam pipe
		G4VSolid* abs_solid = new G4IntersectionSolid("abs_solid_far-bwd",
			abs_box,Extended_Vacuum_Box,0,G4ThreeVector(0,0,Thickness));

		G4LogicalVolume* abs_log = new G4LogicalVolume(abs_solid,mat_vac, "abs_log_far-bwd");
		new G4PVPlacement(0,G4ThreeVector(0,0,-Thickness),abs_log,"abs_far-bwd",Vacuum_Box_log,false,0,true);

		//- Visualization
		G4VisAttributes* abs_vis = new G4VisAttributes(G4Color::Magenta());
		abs_vis->SetVisibility(visibility);
		if(abs_log) abs_log->SetVisAttributes(abs_vis);
	}

	//- Vacuum (subtract the far-backward volume)
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,Far_Backwards_Box,0,G4ThreeVector(0,0,-(Vacuum_BB_MinZ - BB_Z)));

	return;
}

void DetectorConstruction::BuildMaterials()
{
	G4NistManager* nistManager = G4NistManager::Instance();
	//- Vacuum
	mat_vac = nistManager->FindOrBuildMaterial("G4_Galactic");

	return;	
}


void DetectorConstruction::BuildAbsorbers()
{
	if(!mat_vac || !beampipeUni_solid || !world_log)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildAbsorbers ==> Materials, beam pipe solids, or world LV are not created\n");
	}

	//- Absorbers
	for(int iAbs = 0; iAbs < simPar->GetAbsNum(); iAbs++)
	{
		G4Box* abs_box = new G4Box("abs_box_"+to_string(iAbs),
			simPar->GetAbsSizeX(iAbs)/2,
			simPar->GetAbsSizeY(iAbs)/2,
			simPar->GetAbsSizeZ(iAbs)/2); 
		G4LogicalVolume* abs_log;

		if((bwd_beampipeVac_solid || farbwd_beampipeVac_solid) && iAbs == simPar->GetAbsNum() - 1) // skip the last absorber
		{
			continue;
		}
		else
		{
			//- Subtract vacuum beam pipe
			G4VSolid* abs_solid = new G4SubtractionSolid("abs_solid_"+to_string(iAbs),
				abs_box,beampipeUni_solid,
				0,G4ThreeVector(-simPar->GetAbsPosX(iAbs),
						-simPar->GetAbsPosY(iAbs),
						-simPar->GetAbsPosZ(iAbs)));

			abs_log = new G4LogicalVolume(abs_solid,mat_vac, "abs_log_"+to_string(iAbs));

			new G4PVPlacement(0,G4ThreeVector(	simPar->GetAbsPosX(iAbs),
								simPar->GetAbsPosY(iAbs),
								simPar->GetAbsPosZ(iAbs)),
				abs_log,"abs_"+to_string(iAbs),world_log,false,0,true);
		}

		//- Visualization
		G4VisAttributes* abs_vis = new G4VisAttributes(G4Color::Magenta());
		abs_vis->SetVisibility(visibility);
		if(abs_log) abs_log->SetVisAttributes(abs_vis);
	}

	return;
}
