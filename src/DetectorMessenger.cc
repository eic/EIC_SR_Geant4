#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4SystemOfUnits.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction * Detector):G4UImessenger(),
    fDetector(Detector), _directory(0), _target_length_Cmd(0), _target_width_Cmd(0), _target_height_Cmd(0)
{
    _directory = new G4UIdirectory("/target/");
    _directory->SetGuidance("Target geometry");

    _target_length_Cmd = new G4UIcmdWithADoubleAndUnit("/target/length",this);
    _target_length_Cmd->SetGuidance("Set target length along the beam");
    _target_length_Cmd->SetParameterName("target_length",false);
    _target_length_Cmd->SetUnitCategory("Length");
    _target_length_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    _target_width_Cmd = new G4UIcmdWithADoubleAndUnit("/target/width",this);
    _target_width_Cmd->SetGuidance("Set target width across the beam");
    _target_width_Cmd->SetParameterName("target_width",false);
    _target_width_Cmd->SetUnitCategory("Length");
    _target_width_Cmd->AvailableForStates(G4State_PreInit);

    _target_height_Cmd = new G4UIcmdWithADoubleAndUnit("/target/height",this);
    _target_height_Cmd->SetGuidance("Set target height across the beam");
    _target_height_Cmd->SetParameterName("height",false);
    _target_height_Cmd->SetUnitCategory("Length");
    _target_height_Cmd->AvailableForStates(G4State_PreInit);
}

DetectorMessenger::~DetectorMessenger()
{
    delete _directory;
    delete _target_length_Cmd;
    delete _target_width_Cmd;
    delete _target_height_Cmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    if( command == _target_length_Cmd )
    {
        fDetector->SetTargetLength(_target_length_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _target_width_Cmd )
    {
        fDetector->SetTargetWidth(_target_width_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _target_height_Cmd )
    {
        fDetector->SetTargetHeight(_target_height_Cmd->GetNewDoubleValue(newValue));
    }
}
