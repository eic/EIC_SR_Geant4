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

void RunAction::BeginOfRunAction(const G4Run* run)
{
    	G4cout<<"\n\n[INFO] RunAction::BeginOfRunAction ==>  Output file name: "<<_output_filename<<"\n"<<G4endl;
    	_timer->Start();

    	_tree = new TTree("tree", "Collected Data");

    	_tree->Branch("event",		&_event);

    	_tree->Branch("beamPdg",	&_beamPdg);
    	_tree->Branch("beamPosX",	&_beamPosX);
    	_tree->Branch("beamPosY",	&_beamPosY);
    	_tree->Branch("beamPosZ",	&_beamPosZ);
    	_tree->Branch("beamMomX",	&_beamMomX);
    	_tree->Branch("beamMomY",	&_beamMomY);
    	_tree->Branch("beamMomZ",	&_beamMomZ);
    	_tree->Branch("beamEnergy",	&_beamEnergy);

    	_tree->Branch("gammaPosX",	&_gammaPosX);
    	_tree->Branch("gammaPosY",	&_gammaPosY);
    	_tree->Branch("gammaPosZ",	&_gammaPosZ);
    	_tree->Branch("gammaMomX",	&_gammaMomX);
    	_tree->Branch("gammaMomY",	&_gammaMomY);
    	_tree->Branch("gammaMomZ",	&_gammaMomZ);
    	_tree->Branch("gammaEnergy",	&_gammaEnergy);

	_avgEventTime = 0;

	return;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
	TFile* _hfile = new TFile(_output_filename, "RECREATE");
    	if(_hfile->IsZombie())
		throw runtime_error("\n\n[ERROR] RunAction::EndOfRunAction ==> Cannot open the file to write\n");

    	_tree->Write();

    	delete _hfile;

    	_timer->Stop();
    	G4cout<<"\n\n[INFO] RunAction::EndOfRunAction ==> Output file name: "<<_output_filename<<G4endl;
    	G4cout<<"[INFO] RunAction::EndOfRunAction ==> Run time: "<<*_timer<<G4endl;
	G4cout<<"[INFO] RunAction::EndOfRunAction ==> Avg. event time: "<<_avgEventTime/run->GetNumberOfEvent()<<" [sec]"<<G4endl;

	return;
}
