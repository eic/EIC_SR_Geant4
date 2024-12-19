#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

#include <string>

DetectorMessenger::DetectorMessenger(DetectorConstruction* DetConstr): G4UImessenger(),
    fDetectorConstruction(DetConstr), _dist_tag_Cmd(0), _bwd_achmbH_Cmd(0), _modify_Cmd(0)
{
	_dist_tag_Cmd = new G4UIcmdWithADoubleAndUnit("/detector/dist_tag",this);
	_dist_tag_Cmd->SetGuidance("Tagger distance from the start of the ante-chamber"); 
	_dist_tag_Cmd->SetParameterName("dist_tag",true); 
	_dist_tag_Cmd->SetRange("dist_tag>-1e9 && dist_tag<1e9"); 
	_dist_tag_Cmd->SetDefaultValue(0);
	_dist_tag_Cmd->SetDefaultUnit("cm");

	_bwd_achmbH_Cmd = new G4UIcmdWithADoubleAndUnit("/detector/bwd_achmbH",this);
	_bwd_achmbH_Cmd->SetGuidance("FWD ante-chamber height"); 
	_bwd_achmbH_Cmd->SetParameterName("bwd_achmbH",true); 
	_bwd_achmbH_Cmd->SetRange("bwd_achmbH>-1e9 && bwd_achmbH<1e9"); 
	_bwd_achmbH_Cmd->SetDefaultValue(0);
	_bwd_achmbH_Cmd->SetDefaultUnit("cm");

	_farBwdSrMaskDepth_Cmd = new G4UIcmdWithADoubleAndUnit("/detector/farBwdSrMaskDepth",this);
	_farBwdSrMaskDepth_Cmd->SetGuidance("Far-BWD SR mask depth"); 
	_farBwdSrMaskDepth_Cmd->SetParameterName("farBwdSrMaskDepth",true); 
	_farBwdSrMaskDepth_Cmd->SetRange("farBwdSrMaskDepth>-1e9 && farBwdSrMaskDepth<1e9"); 
	_farBwdSrMaskDepth_Cmd->SetDefaultValue(0);
	_farBwdSrMaskDepth_Cmd->SetDefaultUnit("mm");

	_farBwdSrMaskRelPos_Cmd = new G4UIcmdWithADoubleAndUnit("/detector/farBwdSrMaskRelPos",this);
	_farBwdSrMaskRelPos_Cmd->SetGuidance("Far-BWD SR mask rel. position"); 
	_farBwdSrMaskRelPos_Cmd->SetParameterName("farBwdSrMaskRelPos",true); 
	_farBwdSrMaskRelPos_Cmd->SetRange("farBwdSrMaskRelPos>-1e9 && farBwdSrMaskRelPos<1e9"); 
	_farBwdSrMaskRelPos_Cmd->SetDefaultValue(0);
	_farBwdSrMaskRelPos_Cmd->SetDefaultUnit("m");

	_modify_Cmd = new G4UIcmdWithoutParameter("/detector/ModifyGeometry",this);
	_modify_Cmd->SetGuidance("Modify geometry");
	_modify_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
    	delete _dist_tag_Cmd;
	delete _bwd_achmbH_Cmd;
	delete _farBwdSrMaskDepth_Cmd;
	delete _farBwdSrMaskRelPos_Cmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
	// read parameters
    	if(command == _dist_tag_Cmd)
	{
        	fDetectorConstruction->SetTaggerDistance(
			_dist_tag_Cmd->GetNewDoubleValue(newValue));
    	}
	else if(command == _bwd_achmbH_Cmd)
	{
		fDetectorConstruction->SetAnteChamberH(
			_bwd_achmbH_Cmd->GetNewDoubleValue(newValue));
	}
	else if(command == _farBwdSrMaskDepth_Cmd)
	{
		fDetectorConstruction->SetFarBwdSrMaskDepth(
			_farBwdSrMaskDepth_Cmd->GetNewDoubleValue(newValue));
	}
	else if(command == _farBwdSrMaskRelPos_Cmd)
	{
		fDetectorConstruction->SetFarBwdSrMaskRelPos(
			_farBwdSrMaskRelPos_Cmd->GetNewDoubleValue(newValue));
	}
	else if(command == _modify_Cmd)
	{
		fDetectorConstruction->ModifyGeometry();
	}

	return;
}
