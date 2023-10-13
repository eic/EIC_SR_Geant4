#include "PrimaryGeneratorAction.hh"
#include "PrimaryMessenger.hh"

PrimaryMessenger::PrimaryMessenger(PrimaryGeneratorAction* PrimGenAct):G4UImessenger(),
    fPrimaryGeneratorAction(PrimGenAct), _directory(0), _deltaP_Cmd(0), _P_Cmd(0), _PdirX_Cmd(0), _PdirY_Cmd(0), _PdirZ_Cmd(0), _pName_Cmd(0)
{
    _directory = new G4UIdirectory("/beam/");
    _directory->SetGuidance("Beam parameters");

    _deltaP_Cmd = new G4UIcmdWithADouble("/beam/deltaP",this);
    _deltaP_Cmd->SetGuidance("Set momentum spread");
    _deltaP_Cmd->SetParameterName("delta_mom",false);
    _deltaP_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    _P_Cmd = new G4UIcmdWithADoubleAndUnit("/beam/P",this);
    _P_Cmd->SetGuidance("Set momentum");
    _P_Cmd->SetParameterName("mom",false);
    _P_Cmd->SetUnitCategory("Energy");
    _P_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    _PdirX_Cmd = new G4UIcmdWithADouble("/beam/PdirX",this);
    _PdirX_Cmd->SetGuidance("Set momentum direction X");
    _PdirX_Cmd->SetParameterName("momDirX",false);
    _PdirX_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    _PdirY_Cmd = new G4UIcmdWithADouble("/beam/PdirY",this);
    _PdirY_Cmd->SetGuidance("Set momentum direction Y");
    _PdirY_Cmd->SetParameterName("momDirY",false);
    _PdirY_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    _PdirZ_Cmd = new G4UIcmdWithADouble("/beam/PdirZ",this);
    _PdirZ_Cmd->SetGuidance("Set momentum direction Z");
    _PdirZ_Cmd->SetParameterName("momDirZ",false);
    _PdirZ_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    _pName_Cmd = new G4UIcmdWithAString("/beam/pName",this);
    _pName_Cmd->SetGuidance("Set particle name name");
    _pName_Cmd->SetParameterName("particle_name",false);
    _pName_Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

PrimaryMessenger::~PrimaryMessenger()
{
    delete _directory;
    delete _deltaP_Cmd;
    delete _P_Cmd;
    delete _PdirX_Cmd;
    delete _PdirY_Cmd;
    delete _PdirZ_Cmd;
    delete _pName_Cmd;
}

void PrimaryMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    if( command == _deltaP_Cmd )
    {
        fPrimaryGeneratorAction->SetMomentumSpread((G4double)_deltaP_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _P_Cmd )
    {
        fPrimaryGeneratorAction->SetMomentum((G4double)_P_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _PdirX_Cmd )
    {
        fPrimaryGeneratorAction->SetMomentumDirectionX((G4double)_PdirX_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _PdirY_Cmd )
    {
        fPrimaryGeneratorAction->SetMomentumDirectionY((G4double)_PdirY_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _PdirZ_Cmd )
    {
        fPrimaryGeneratorAction->SetMomentumDirectionZ((G4double)_PdirZ_Cmd->GetNewDoubleValue(newValue));
    }
    if( command == _pName_Cmd )
    {
        fPrimaryGeneratorAction->SetParticleName(newValue);
    }
}
