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

	visibility = true;
	mat_vac = nullptr;
	world_log = nullptr;
	world_phys = nullptr;
	beampipeUni_solid = nullptr;
	beampipeVac_solid = nullptr;
	beampipeVac_log = nullptr;
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
	BuildIpBeamPipe();
	BuildMagnets();
	BuildIrBeamPipe();
	BuildAbsorbers();

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
		"[ERROR] DetectorConstruction::CreateBeamPipeSolid ==> Materials are not created\n");
	}

   	G4Box* world_box = new G4Box("world",
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
	G4LogicalVolume* beampipeExt_log = new G4LogicalVolume(beampipeExt_solid, mat_vac,"beampipeExt");

	//- Union (beam pipe with an extention)
	beampipeUni_solid = new G4UnionSolid("beampipeUni_solid", 
		beampipeOrig_solid, beampipeExt_solid, 0, G4ThreeVector(0, 0, simPar->GetExtE()-simPar->GetExtL()/2));

	//- Copy
	beampipeVac_solid = beampipeUni_solid;

	return;
}

void DetectorConstruction::BuildMagnets()
{
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildMagnets ==> Materials or beam pipe solids are not created\n");
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
	if(!mat_vac || !beampipeVac_solid || !beampipeUni_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildIpBeamPipe ==> Materials or beam pipe solids are not created\n");
	}

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
	if(!mat_vac || !beampipeVac_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildIrBeamPipe ==> Materials or beam pipe solids are not created\n");
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

void DetectorConstruction::BuildAbsorbers()
{
	if(!mat_vac || !beampipeUni_solid)
	{
		throw runtime_error(
		"[ERROR] DetectorConstruction::BuildAbsorbers ==> Materials or beam pipe solids are not created\n");
	}

	//- Absorbers
	for(int iAbs = 0; iAbs < simPar->GetAbsNum(); iAbs++)
	{
		G4Box* abs_box = new G4Box("abs_box_"+to_string(iAbs),
		simPar->GetAbsSizeX(iAbs)/2,simPar->GetAbsSizeY(iAbs)/2,simPar->GetAbsSizeZ(iAbs)/2); 

		G4VSolid* abs_solid = new G4SubtractionSolid("abs_solid_"+to_string(iAbs),abs_box,beampipeUni_solid,0,
		G4ThreeVector(-simPar->GetAbsPosX(iAbs),-simPar->GetAbsPosY(iAbs),-simPar->GetAbsPosZ(iAbs)));

		G4LogicalVolume* abs_log = new G4LogicalVolume(abs_solid,mat_vac, "abs_log_"+to_string(iAbs));

		new G4PVPlacement(0,
			G4ThreeVector(simPar->GetAbsPosX(iAbs),simPar->GetAbsPosY(iAbs),simPar->GetAbsPosZ(iAbs)),
			abs_log,"abs_"+to_string(iAbs),world_log,false,0,true);
		//- Visualization
		G4VisAttributes* abs_vis = new G4VisAttributes(G4Color::Magenta());
		abs_vis->SetVisibility(visibility);
		abs_log->SetVisAttributes(abs_vis);
	}

	return;
}
