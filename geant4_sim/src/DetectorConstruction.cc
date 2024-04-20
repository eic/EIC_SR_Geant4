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


DetectorConstruction::DetectorConstruction(G4String xmlFileName) : G4VUserDetectorConstruction()
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
	beampipeVac_log = nullptr;
	farbwd_beampipeVac_solid = nullptr;
	ip_box = nullptr;
	ip_log = nullptr;
	mass = -1;
	gamma = -1;
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
	CreateSrMask();
	BuildIpBeamPipe();
//	BuildFarBackwardBeamPipe();
	BuildMagnets();
	BuildIrBeamPipe();
	if(addAbsorber) BuildAbsorbers();

	return world_phys;
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
	G4LogicalVolume* beampipeOrig_log = new G4LogicalVolume(beampipeOrig_solid, mat_vac,"beampipeOrig");

	//- Extention
	G4VSolid* beampipeExt_solid = new G4Tubs("beampipeExt_solid", 0, 
		simPar->GetExtR(), simPar->GetExtL()/2, 0, 2.0 * M_PI);

	//- Union (beam pipe with an extention)
	beampipeUni_solid = new G4UnionSolid("beampipeUni_solid", 
		beampipeOrig_solid, beampipeExt_solid, 0, G4ThreeVector(0, 0, simPar->GetExtE()-simPar->GetExtL()/2));

	//- Copy
	beampipeVac_solid = beampipeUni_solid;

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
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid || !world_log)
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

			mag_solid = new G4IntersectionSolid(simPar->GetMagName(iMag)+"_solid",
				beampipeUni_solid, mag_vol,mag_rot,mag_center);
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

			G4VSolid* vac_wo_ip_solid = new G4SubtractionSolid("vac_wo_ip_solid", beampipeUni_solid, ip_box,0,
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
		new G4PVPlacement(
			0,G4ThreeVector(0.,0.,0.),mag_log,"mag_"+simPar->GetMagName(iMag),world_log,false,0,true);

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
		beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
			beampipeVac_solid,mag_vol,mag_rot,mag_center);
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

	//- ip_cut
	G4VSolid* ip_cut = new G4Tubs("ip_cut",62.0/2 * mm, 80.0/2 * mm, (150.0 + 20.1) * mm, 0, 2.0 * M_PI);

	//- Vacuum (subtract ip_cut from the beam pipe)
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",
		beampipeVac_solid,ip_cut,0,G4ThreeVector(0.0 * cm,0.0 * cm,65.0 * cm));
	beampipeUni_solid = new G4SubtractionSolid("beampipeUni_solid",
		beampipeUni_solid,ip_cut,0,G4ThreeVector(0.0 * cm,0.0 * cm,65.0 * cm));

	//- Visualization
//	G4LogicalVolume* ip_cut_log = new G4LogicalVolume(ip_cut, mat_vac, "ip_cut_log");
//	new G4PVPlacement(0,G4ThreeVector(0.0 * cm,0.0 * cm,65.0 * cm),ip_cut_log,"ip_cut",world_log,false,0,true);
//	G4VisAttributes* ip_cut_vis = new G4VisAttributes(G4Colour::Red());
//	ip_cut_log->SetVisAttributes(ip_cut_vis);

	//- IP beam pipe
	ip_box = new G4Box("ip_box",1*m/2,1*m/2,(simPar->GetIpBeampipeZmax()-simPar->GetIpBeampipeZmin())/2); 
	G4VSolid* ip_solid = new G4IntersectionSolid("ip_solid", beampipeUni_solid, ip_box,0,
		G4ThreeVector(	0,
				0,
				simPar->GetIpBeampipeZmin()+(simPar->GetIpBeampipeZmax()-simPar->GetIpBeampipeZmin())/2));
	ip_log = new G4LogicalVolume(ip_solid,mat_vac,"ip_log");
	new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),ip_log,"vac_IP",world_log,false,0,true);
	//- Visualization
	G4VisAttributes* ip_vis = new G4VisAttributes(G4Colour::Cyan());
	ip_vis->SetVisibility(visibility);
	ip_log->SetVisAttributes(ip_vis);

	//- Vacuum (subtract the ip beam pipe volume from the beam pipe volume)
	beampipeVac_solid = new G4SubtractionSolid("beampipeVac_solid",beampipeVac_solid,ip_box,
	0,G4ThreeVector(0.,0.,simPar->GetIpBeampipeZmin()+(simPar->GetIpBeampipeZmax()-simPar->GetIpBeampipeZmin())/2));

	return;
}

void DetectorConstruction::BuildIrBeamPipe()
{
	if(!mat_vac || !beampipeVac_solid || !world_log)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildIrBeamPipe ==> Materials, beam pipe solids, or world LV are not created\n");
	}

	//- Vacuum (place the beam pipe without magnet and IP volumes) 
	beampipeVac_log = new G4LogicalVolume(beampipeVac_solid, mat_vac,"beampipeVac");
	// set tracking time limit in the volume
	beampipeVac_log->SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,simPar->GetTrackTcut()));
	new G4PVPlacement(0,G4ThreeVector(0,0,0),beampipeVac_log,"vac_beampipe",world_log,false,0,true);
	//- Visualization
	G4VisAttributes* beampipeVac_vis = new G4VisAttributes(G4Color::Grey());
	beampipeVac_vis->SetVisibility(visibility);
	beampipeVac_log->SetVisAttributes(beampipeVac_vis);

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
	farbwd_beampipeVac_solid = new G4IntersectionSolid("Vacuum_Box_Sub",Vacuum_Box, Far_Backwards_Box, 
		rotate2, position);

	//- Place the far-beamline volume
	G4RotationMatrix* xyzRot = new G4RotationMatrix();
	xyzRot->rotateX(M_PI); // flip from eic-sheel to Geant4 coordinate system
	xyzRot->rotateZ(M_PI); // flip from eic-sheel to Geant4 coordinate system
	xyzRot->rotateY(-Beam_Theta); // opposite to eic-shell

	G4LogicalVolume* Vacuum_Box_log = new G4LogicalVolume(farbwd_beampipeVac_solid,mat_vac, "Vacuum_Box_log");
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

		if(farbwd_beampipeVac_solid && iAbs == simPar->GetAbsNum() - 1) // skip the last absorber
		{
			// see DetectorConstruction::BuildFarBackwardBeamPipe()
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
