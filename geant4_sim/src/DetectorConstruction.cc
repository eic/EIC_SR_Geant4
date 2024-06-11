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
// 	CreateSrMask();
	BuildFwdAnteChamber();
	BuildIpBeamPipe();
	BuildMagnets();
	BuildIrBeamPipe();
	BuildTaggers();
	if(addAbsorber) BuildAbsorbers();

	return world_phys;
}

void DetectorConstruction::BuildMaterials()
{
	G4NistManager* nistManager = G4NistManager::Instance();
	//- Vacuum
	mat_vac = nistManager->FindOrBuildMaterial("G4_Galactic");

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

	//- FWD extention
	G4VSolid* beampipeExtFwd_solid = new G4Tubs("beampipeExtFwd_solid", 0, 
		simPar->GetExtR(), simPar->GetExtL()/2, 0, 2.0 * M_PI);

	//- Union (beam pipe with an extention)
	beampipeUni_solid = new G4UnionSolid("beampipeUni_solid", 
		beampipeOrig_solid, beampipeExtFwd_solid, 0, 
		G4ThreeVector(0, 0, simPar->GetExtE()+simPar->GetExtL()/2));

	//- Cut the vacuum on the BWD side 
	G4Box* beampipe_cut = new G4Box("beampipe_cut",2*m, 2*m, 10*m);
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,beampipe_cut,0,G4ThreeVector(0,0,-9280.0 * mm - 10*m));

	//- Copy
	beampipeVac_solid = beampipeUni_solid;

	//---------------------------------------------------------------------------------------------------------------//
	//- Build an extended bwd beam pipe
	const G4double B2eR_bend = 0.020 * rad;
	const G4double B2eR_bend_eps = 0.0001 * rad; // a small eps for the torus, otherwise the geo builder fails
	const G4double B2eR_len = 5.500091667736 * m;
	const G4double B2eR_cut_offset = 1 * cm;
	G4ThreeVector B2eR_start(0*cm,0*cm,-1675*cm);
	G4double torus_radius = (0.5*B2eR_len/sin(B2eR_bend/2.)); 
	G4ThreeVector B2eR_end = B2eR_start - G4ThreeVector(-torus_radius,0,B2eR_start.z());	
	B2eR_end.rotateY(B2eR_bend);
	B2eR_end += G4ThreeVector(-torus_radius,0,B2eR_start.z());  

	const G4int numZPlanes = 8;
	const G4double zPlane[numZPlanes] = {
		-9280.0 * mm, -9288.0 * mm, -9364.0 * mm, -11364.0 * mm, -11414.0 * mm, 
		-15000.0 * mm, -16000.0 * mm, -25000.0 * mm};
	const G4double rInner[numZPlanes] = {0,0,0,0,0,0,0,0};
	const G4double rOuter[numZPlanes] = {
		128.6/2 * mm, 128.6/2 * mm, 154.0/2 * mm, 154.0/2 * mm, 196.0/2 * mm, 
		196.0/2 * mm, B2eR_beampipe_radius, B2eR_beampipe_radius};
	G4Polycone* polycone = new G4Polycone("polycone",
		0, 2.0 * M_PI, numZPlanes, zPlane, rInner, rOuter);

	// Torus - bent beam pipe
	G4RotationMatrix* torus_rot = new G4RotationMatrix();
	torus_rot->rotateX(M_PI * 0.5 * rad);
	
	G4VSolid* torus_solid = new G4Torus("torus_solid",0,B2eR_beampipe_radius,
		torus_radius,0,B2eR_bend + B2eR_bend_eps);
	bwd_beampipeVac_solid = new G4UnionSolid("bwd_beampipeVac_solid",
		polycone,torus_solid,
		torus_rot,G4ThreeVector(-torus_radius, 0, B2eR_start.z()));

	//---------------------------------------------------------------------------------------------------------------//
	// Cut the BWD beam pipe at the end of B2eR
	bwd_beampipeVac_solid = new G4SubtractionSolid("bwd_beampipeVac_solid",
		bwd_beampipeVac_solid,beampipe_cut,0,G4ThreeVector(0,0,B2eR_end.z() - B2eR_cut_offset - 10*m));
	//---------------------------------------------------------------------------------------------------------------//
	// Far-BWD beam pipe
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
		farbwd_tube,ante_chmbr_box,0,G4ThreeVector(-25*cm/2.,0,0));
	// distance between the main beam pipe center (placement coordinates) and the cut
	G4double L = (abs(B2eR_end.z()) + 0.5*farbwd_len*cos(B2eR_bend)-(abs(B2eR_end.z()) + B2eR_cut_offset))/cos(B2eR_bend);
	// half-width of the cut in the main beam pipe
	G4double M = B2eR_end.x() + 0.5*farbwd_len*sin(B2eR_bend) - L*sin(B2eR_bend);
	// displacement of the lumi pipe w.r.t. the main beam pipe
	G4double dL = M * sin(B2eR_bend); // along main beam pipe Z-axis
	G4double dK = M * cos(B2eR_bend); // along main beam pipe X-axis
	// unite lumi and main+ante-chamber in the main beam pipe coordinate system
	farbwd_tube = new G4UnionSolid("farbwd_tube",farbwd_tube,lumi_tube,lumi_rot,G4ThreeVector(-dK,0,L-dL));

	// Cut the FarBWD beam pipe at the end of B2eR
	farbwd_beampipeVac_solid = new G4SubtractionSolid("farbwd_beampipeVac_solid",
		farbwd_tube,beampipe_cut,cut_rot,G4ThreeVector(0,0,(L+(10*m)/cos(B2eR_bend))));

	// Place the assembly
	farbwd_beampipeVac_log = new G4LogicalVolume(farbwd_beampipeVac_solid,mat_vac,"farbwd_beampipeVac_log");
	farbwd_beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut())); 
	G4ThreeVector far_bwd_center(	B2eR_end.x() - 0.5*farbwd_len*sin(B2eR_bend),
					B2eR_end.y(),
					B2eR_end.z() - 0.5*farbwd_len*cos(B2eR_bend));
	new G4PVPlacement(farbwd_rot, 
		G4ThreeVector(far_bwd_center.x(),far_bwd_center.y(),far_bwd_center.z()),
		farbwd_beampipeVac_log,"vac_farbwd_beampipeVac",world_log,false,0,true);

	// print far-bwd center coordinates
	printf("[INFO] Far-BWD center: (%.10f,%.10f,%.10f) [cm]\n",
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
		new G4PVPlacement(0,G4ThreeVector(0,0,-(farbwd_len+farbwd_ext)/2. + 1 * cm),
			abs_log,"abs_farbwd",farbwd_beampipeVac_log,false,0,true);

		//- Visualization
		G4VisAttributes* abs_vis = new G4VisAttributes(G4Color::Magenta());
		abs_vis->SetVisibility(visibility);
		abs_log->SetVisAttributes(abs_vis);
	}
	//---------------------------------------------------------------------------------------------------------------//
	// Place a lumi window
	G4double lumiWindStart_posz = B2eR_end.z() - B2eR_cut_offset - lumi_halflen;
	G4cout<<"[INFO] lumiWindStart_posz = "<<lumiWindStart_posz/cm<<" [cm]"<<G4endl;
	G4Box* lumiWind_box = new G4Box("lumiWind_box",B2eR_beampipe_radius,B2eR_beampipe_radius,1 * cm);
	G4LogicalVolume* lumiWind_log = new G4LogicalVolume(lumiWind_box,mat_vac,"lumiWind_log");
	new G4PVPlacement(0,G4ThreeVector(0,0,lumiWindStart_posz - 1*cm),lumiWind_log,"lumiWind",world_log,false,0,true);
	
	return;
}

void DetectorConstruction::BuildFwdAnteChamber()
{
	if(!beampipeVac_solid || !beampipeUni_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildFwdAnteChamber ==> beam pipe solids are not created\n");
	}
	G4double startZ = 30.0 * m;
	G4double stopZ = 12.5 * m;
	G4Box* fwd_cut = new G4Box("fwd_cut",1 * m, 1 * m, (startZ - stopZ)/2.);

	// Cut the vacuum
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,fwd_cut,0,G4ThreeVector(0,0,startZ - (startZ - stopZ)/2.));
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,fwd_cut,0,G4ThreeVector(0,0,startZ - (startZ - stopZ)/2.));

	// fwd beam pipe	
	G4VSolid* fwd_beampipeVac_solid = new G4Tubs("fwd_beampipeVac_solid",
		0,10*cm/2,(startZ - stopZ)/2.,0,2.0 * M_PI);
	
	G4EllipticalTube* ell_tube = new G4EllipticalTube("ell_tube",12.6*cm/2.,10*cm/2,(23.5*m - stopZ)/2.);

	fwd_beampipeVac_solid = new G4UnionSolid("fwd_beampipeVac_solid",
		fwd_beampipeVac_solid,ell_tube,0,G4ThreeVector(0,0,-(startZ-stopZ)/2. + (23.5*m - stopZ)/2.));

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
	G4double dz = (startZ-stopZ)/2.;
	G4VSolid* ante_chamber_solid = new G4ExtrudedSolid("ante_chamber_solid", 
		polygon, dz, offsetA, scaleA, offsetB, scaleB);

	fwd_beampipeVac_solid = new G4UnionSolid("fwd_beampipeVac_solid",
		fwd_beampipeVac_solid,ante_chamber_solid,0,G4ThreeVector(0,0,0));

	G4LogicalVolume* fwd_beampipeVac_log = new G4LogicalVolume(
		fwd_beampipeVac_solid,mat_vac,"fwd_beampipeVac_log");
	fwd_beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut())); 
	new G4PVPlacement(0,G4ThreeVector(0,0,startZ - dz),
		fwd_beampipeVac_log,"vac_fwd_beampipeVac",world_log,false,0,true);

	// Visualization
	G4VisAttributes* fwd_beampipeVac_vis = new G4VisAttributes(G4Color::White());
	fwd_beampipeVac_vis->SetVisibility(visibility);
	fwd_beampipeVac_log->SetVisAttributes(fwd_beampipeVac_vis);

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
	new G4PVPlacement(0,G4ThreeVector(-(B2eR_beampipe_radius+18.0 * cm /2),0,-tag1_posZ),
		tag_log,"sens_tag1",farbwd_beampipeVac_log,false,0,true);

	// Tagger 2
	new G4PVPlacement(0,G4ThreeVector(-(B2eR_beampipe_radius+18.0 * cm /2),0,-(farbwd_len/2. - 1*mm)),
		tag_log,"sens_tag2",farbwd_beampipeVac_log,false,0,true);

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
			G4ThreeVector magVector1(	-simPar->GetMagStartPosX(iMag) + simPar->GetIpPosX(),
							-simPar->GetMagStartPosY(iMag) + simPar->GetIpPosY(),
							-simPar->GetMagStartPosZ(iMag) + simPar->GetIpPosZ());
			// vector of end coordinates in the global ref. system shifted w.r.t. the ip
			G4ThreeVector magVector2(	-simPar->GetMagEndPosX(iMag) + simPar->GetIpPosX(),
							-simPar->GetMagEndPosY(iMag) + simPar->GetIpPosY(),
							-simPar->GetMagEndPosZ(iMag) + simPar->GetIpPosZ());
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
		else
		{
			beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
				beampipeVac_solid,mag_vol,mag_rot,mag_center);
		}
	}

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
	G4VisAttributes* bwd_beampipeVac_vis = new G4VisAttributes(G4Color::Grey());
	bwd_beampipeVac_vis->SetVisibility(visibility);
	bwd_beampipeVac_log->SetVisAttributes(bwd_beampipeVac_vis);

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
