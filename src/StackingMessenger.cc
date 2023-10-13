#include "StackingMessenger.hh"
#include "StackingAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

StackingMessenger::StackingMessenger(StackingAction * StackAct):G4UImessenger(),
    fStackingAction(StackAct), _directory(0), _kill_secondaries_Cmd(0)
{
    _directory = new G4UIdirectory("/stack/");
    _directory->SetGuidance("Track stacking");

    _kill_secondaries_Cmd = new G4UIcmdWithAnInteger("/stack/killsecondaries",this);
    _kill_secondaries_Cmd->SetGuidance("Set kill secondaries");
    _kill_secondaries_Cmd->SetParameterName("kill_secondaries",false);
    _kill_secondaries_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

StackingMessenger::~StackingMessenger()
{
    delete _directory;
    delete _kill_secondaries_Cmd;
}

void StackingMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    if( command == _kill_secondaries_Cmd )
    {
        fStackingAction->SetKillStatus((G4int)_kill_secondaries_Cmd->GetNewIntValue(newValue));
    }
}
