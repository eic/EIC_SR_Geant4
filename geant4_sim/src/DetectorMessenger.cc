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

	_modify_Cmd = new G4UIcmdWithoutParameter("/detector/ModifyGeometry",this);
	_modify_Cmd->SetGuidance("Modify geometry");
	_modify_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
    	delete _dist_tag_Cmd;
	delete _bwd_achmbH_Cmd;
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
	else if(command == _modify_Cmd)
	{
		fDetectorConstruction->ModifyGeometry();
	}

	return;
}
