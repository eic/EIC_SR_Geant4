#include "RunMessenger.hh"
#include "RunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

RunMessenger::RunMessenger(RunAction * RunAct):G4UImessenger(),
    fRunAction(RunAct), _directory(0), _output_filename_Cmd(0)
{
    _directory = new G4UIdirectory("/output/");
    _directory->SetGuidance("Output files");

    _output_filename_Cmd = new G4UIcmdWithAString("/output/filename",this);
    _output_filename_Cmd->SetGuidance("Set output file name");
    _output_filename_Cmd->SetParameterName("output_filename",false);
    _output_filename_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

RunMessenger::~RunMessenger()
{
    delete _directory;
    delete _output_filename_Cmd;
}

void RunMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    if( command == _output_filename_Cmd )
    {
        fRunAction->SetOutputFilename(newValue);
    }
}
