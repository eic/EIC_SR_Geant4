#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VParticleChange.hh"
#include "G4ParticleChange.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4Transform3D.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

SteppingAction::SteppingAction(RunAction* runAct, G4String xmlFileName) : runAction(runAct)
{
	simPar = new SimParameters();
	simPar->InitDefault();
	simPar->ReadXML(xmlFileName);
	simPar->PrintGammaCut();
	simPar->PrintTrkGeoCut();
}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
	G4Track* track = step->GetTrack();

	G4StepPoint*            preStep         = step->GetPreStepPoint();
	G4StepPoint*            postStep        = step->GetPostStepPoint();
	G4VPhysicalVolume*      prePV           = preStep->GetPhysicalVolume();
	G4VPhysicalVolume*      postPV          = postStep->GetPhysicalVolume();

	//- Stop tracking on the backward side
	if(	postPV && 
		postStep->GetPosition().getZ() < simPar->GetTrackBwdGeoCut())
	{
		if(track->GetTrackID() == 1) // primary particle
		{
			runAction->_beamKillPosX.push_back(postStep->GetPosition().getX()/cm);
			runAction->_beamKillPosY.push_back(postStep->GetPosition().getY()/cm);
			runAction->_beamKillPosZ.push_back(postStep->GetPosition().getZ()/cm);
		}

		track->SetTrackStatus(fStopAndKill);
		return;
	}


	// the monitor is associated with one of the magnets
	G4String mon_name = prePV->GetName();
	// solenoid field in the IP beam pipe
	if(mon_name == "vac_IP" && simPar->IsBuildSolenoid()){mon_name = "mag_MARCO";}

	//--------------------------------------------------------------------------//
	//- Store taggers' info 
	if(	postPV &&
		postStep->GetStepStatus() == fGeomBoundary && // at boundary
		G4StrUtil::contains(postPV->GetName(),"sens_")) // entering the tagger
	{
		runAction->_sensName.push_back(postPV->GetName());
		runAction->_sensPDG.push_back(track->GetParticleDefinition()->GetPDGEncoding());
		runAction->_sensPosX.push_back(postStep->GetPosition().getX()/cm);
		runAction->_sensPosY.push_back(postStep->GetPosition().getY()/cm);
		runAction->_sensPosZ.push_back(postStep->GetPosition().getZ()/cm);
		runAction->_sensMomX.push_back(postStep->GetMomentum().getX()/eV);
		runAction->_sensMomY.push_back(postStep->GetMomentum().getY()/eV);
		runAction->_sensMomZ.push_back(postStep->GetMomentum().getZ()/eV);
		runAction->_sensEnergy.push_back(track->GetTotalEnergy()/eV);
		runAction->_sensTime.push_back(track->GetGlobalTime()/ms); // time from the start of the event

		if(track->GetTrackID() == 1) // primary particle
		{
			runAction->_beamKillPosX.push_back(postStep->GetPosition().getX()/cm);
			runAction->_beamKillPosY.push_back(postStep->GetPosition().getY()/cm);
			runAction->_beamKillPosZ.push_back(postStep->GetPosition().getZ()/cm);
		}

		// absorb the particle
		track->SetTrackStatus(fStopAndKill); 
		return;
	}

	//--------------------------------------------------------------------------//
	//- Kill primaries leaving the vacuum
	if(	track->GetTrackID() == 1 && // primary particle
		postStep->GetStepStatus() == fGeomBoundary && // at boundary
		postPV->GetName() == "world") // entering world
	{
		runAction->_beamKillPosX.push_back(postStep->GetPosition().getX()/cm);
		runAction->_beamKillPosY.push_back(postStep->GetPosition().getY()/cm);
		runAction->_beamKillPosZ.push_back(postStep->GetPosition().getZ()/cm);

		track->SetTrackStatus(fStopAndKill); 
		return;
	}
	//--------------------------------------------------------------------------//
	//- Beam monitor at the last magnet
	// check the track
	if(     postStep && // check for the entering volume
		track->GetTrackID() == 1 && // primary particle 
		postStep->GetStepStatus() == fGeomBoundary && // at boundary
		G4StrUtil::contains(postPV->GetName(),"vac_") && // entering the vacuum
		G4StrUtil::contains(mon_name,simPar->GetBeamMonName())) // leaving the magnet
	{
		G4int iMag = simPar->GetMagNum() - 1;
		// find the iterator of the magnet by its name
		auto it = std::find(runAction->_magName.begin(),runAction->_magName.end(),mon_name);
		if (it != runAction->_magName.end())
		{
			iMag = distance(runAction->_magName.begin(),it);
		}
		else
		{
			G4cout<<"\n\n=================================================================="<<G4endl;
			G4cout<<"[WARNING] SteppingAction::UserSteppingAction ==> ";
			G4cout<<"Cannot find the magnet for the beam monitoring"<<G4endl;
			G4cout<<"==================================================================\n"<<G4endl;
		}
		// get particle coordinates
		runAction->_monName = mon_name;
		runAction->_monPosX = postStep->GetPosition().getX()/cm;
		runAction->_monPosY = postStep->GetPosition().getY()/cm;
		runAction->_monPosZ = postStep->GetPosition().getZ()/cm;
		runAction->_monMomX = postStep->GetMomentum().getX()/eV;
		runAction->_monMomY = postStep->GetMomentum().getY()/eV;
		runAction->_monMomZ = postStep->GetMomentum().getZ()/eV;

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
		
		// get monitor coordinates
		runAction->_monRefPosX = magVector1.x()/cm;
        	runAction->_monRefPosY = magVector1.y()/cm;
        	runAction->_monRefPosZ = magVector1.z()/cm;

		// orientation angle of the beam/monitor
		runAction->_monRefAngle = (magVector2.z() != magVector1.z()) ? 
			atan((magVector2.x()-magVector1.x())/(magVector2.z()-magVector1.z())) : 0;

		// solenoid
		if(simPar->IsBuildSolenoid() && mon_name == "mag_MARCO")
		{
			runAction->_monRefPosX = 0.0;
			runAction->_monRefPosY = 0.0;
			runAction->_monRefPosZ = 0.0;
			runAction->_monRefAngle = 0.0;
		}
	}

	//--------------------------------------------------------------------------//
	//-- Store photon coordinates at the edge of the vacuum

	// check if the next step is not outside the world and skip everything except for gamma
	if(postPV && track->GetParticleDefinition()->GetPDGEncoding() == G4Gamma::Gamma()->GetPDGEncoding())
	{
		// fill trajectory vectors
		G4ThreeVector pos = track->GetPosition();
		runAction->_trackPosX_cm.push_back(pos.x()/cm);
		runAction->_trackPosY_cm.push_back(pos.y()/cm);
		runAction->_trackPosZ_cm.push_back(pos.z()/cm);
		runAction->_trackEne_GeV.push_back(track->GetKineticEnergy()/GeV);
	}

	//--------------------------------------------------------------------------//
	//-- Store killed photon info
	// check if the track (=gamma) is killed
	if(/*postPV && */track->GetTrackStatus() == fStopAndKill && 
		track->GetParticleDefinition()->GetPDGEncoding() == G4Gamma::Gamma()->GetPDGEncoding())
	{	
		if(!postPV)
		{
			G4cout<<"\n\n=================================================================="<<G4endl;
			G4cout<<"[WARNING] SteppingAction::UserSteppingAction"<<G4endl;
			G4cout<<" No postStep volume is assigned. Please check your geometry."<<G4endl;
			G4cout<<" All vacuum volumes where SR should propagate must have the 'vac_' prefix."<<G4endl;
			G4cout<<" The SR photon is skipped!"<<G4endl;
			G4cout<<"==================================================================\n"<<G4endl;
			return;
		}
		// do not store in absorbers
		if(G4StrUtil::contains(postStep->GetPhysicalVolume()->GetName(),"abs_")){return;}

		runAction->_gammaVtxPosX.push_back(track->GetVertexPosition().getX()/cm);
		runAction->_gammaVtxPosY.push_back(track->GetVertexPosition().getY()/cm);
		runAction->_gammaVtxPosZ.push_back(track->GetVertexPosition().getZ()/cm);
		runAction->_gammaVtxDirX.push_back(track->GetVertexMomentumDirection().getX());
		runAction->_gammaVtxDirY.push_back(track->GetVertexMomentumDirection().getY());
		runAction->_gammaVtxDirZ.push_back(track->GetVertexMomentumDirection().getZ());

		runAction->_gammaPostPV.push_back(postStep->GetPhysicalVolume()->GetName());
		runAction->_gammaPrePV.push_back(preStep->GetPhysicalVolume()->GetName());
		runAction->_gammaPosX.push_back(postStep->GetPosition().getX()/cm);
		runAction->_gammaPosY.push_back(postStep->GetPosition().getY()/cm);
		runAction->_gammaPosZ.push_back(postStep->GetPosition().getZ()/cm);
		runAction->_gammaMomX.push_back(postStep->GetMomentum().getX()/eV);
		runAction->_gammaMomY.push_back(postStep->GetMomentum().getY()/eV);
		runAction->_gammaMomZ.push_back(postStep->GetMomentum().getZ()/eV);
		runAction->_gammaEnergy.push_back(track->GetTotalEnergy()/eV);
		runAction->_gammaTime.push_back(track->GetGlobalTime()/ms); // time from the start of the event

		// lumi window
		if(postStep->GetPhysicalVolume()->GetName() == "lumiWind")
		{
			runAction->_lumiPosX.push_back(postStep->GetPosition().getX()/cm);
			runAction->_lumiPosY.push_back(postStep->GetPosition().getY()/cm);
			runAction->_lumiPosZ.push_back(postStep->GetPosition().getZ()/cm);
			runAction->_lumiMomX.push_back(postStep->GetMomentum().getX()/eV);
			runAction->_lumiMomY.push_back(postStep->GetMomentum().getY()/eV);
			runAction->_lumiMomZ.push_back(postStep->GetMomentum().getZ()/eV);
			runAction->_lumiEnergy.push_back(track->GetTotalEnergy()/eV);
			runAction->_lumiTime.push_back(track->GetGlobalTime()/ms); // time from the start of the event
		}
	}

	return;
}
