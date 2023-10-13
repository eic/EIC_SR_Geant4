#include "RunAction.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    // default value
    _output_filename = "output.root";

    _timer = new G4Timer;
    fRunMessenger = new RunMessenger(this);
}

RunAction::~RunAction()
{
    delete _timer;
    delete fRunMessenger;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    G4cout<<G4endl<<"--> Output file name: "<<_output_filename<<G4endl;
    _timer->Start();

    _tree = new TTree("tree", "A Tree with Data of the primary particle");

    _tree->Branch("EventID",    &_Event_ID);
    _tree->Branch("PDG",        &_PDG);
    _tree->Branch("Q",          &_Q);
    _tree->Branch("Mass",       &_Mass);

    _tree->Branch("Det0",       &_Det0);
    _tree->Branch("X0",         &_X0);
    _tree->Branch("Y0",         &_Y0);
    _tree->Branch("Z0",         &_Z0);
    _tree->Branch("MomX0",      &_Mom_X0);
    _tree->Branch("MomY0",      &_Mom_Y0);
    _tree->Branch("MomZ0",      &_Mom_Z0);
    _tree->Branch("Ekin0",      &_Ekin0);

    _tree->Branch("Det1",       &_Det1);
    _tree->Branch("X1",         &_X1);
    _tree->Branch("Y1",         &_Y1);
    _tree->Branch("Z1",         &_Z1);
    _tree->Branch("MomX1",      &_Mom_X1);
    _tree->Branch("MomY1",      &_Mom_Y1);
    _tree->Branch("MomZ1",      &_Mom_Z1);
    _tree->Branch("Ekin1",      &_Ekin1);

    _tree->Branch("targetL",    &_targetL);
    _tree->Branch("targetW",    &_targetW);
    _tree->Branch("targetH",    &_targetH);
}

void RunAction::EndOfRunAction(const G4Run* )
{
    TFile* _hfile = new TFile(_output_filename, "RECREATE");
    if(_hfile->IsZombie()) exit(-1);

    _tree->Write();

    delete _hfile;

    _timer->Stop();
    G4cout<<G4endl<<"--> Output file name: "<<_output_filename<<G4endl;
    G4cout<<G4endl<<G4endl<<"Time: "<<*_timer<<G4endl<<G4endl;
}
