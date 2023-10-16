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


	string fName = "../materials/02-Copper.csv";
	_reflectData = ReadReflectData(fName,_grazingAngle,_photonEnergy);
	PrintReflectData(_reflectData,_grazingAngle,_photonEnergy);
	


	return;
}

void RunAction::PrintReflectData(std::vector<std::pair<double, std::vector<double>>> dataset,
		std::vector<double> angle, std::vector<double> energy)
{
	// print column names
	printf("Angl/Ene : ");
	for(int j = 0; j < angle.size(); ++j){printf("%.1e  ",angle.at(j));}
	printf("\n");

	printf("Angl/Ene : ");
    	for(int j = 0; j < dataset.size(); ++j)
    	{
        	printf("%.1e  ",dataset.at(j).first);
    	}
	printf("\n");

	// print data
    	for(int i = 0; i < dataset.at(0).second.size(); ++i)
    	{
		printf("%.1e  : ",energy.at(i));
        	for(int j = 0; j < dataset.size(); ++j)
        	{
           		printf("%.1e  ",dataset.at(j).second.at(i));
        	}
		printf("\n");
    	}

	return;
}

std::vector<std::pair<double, std::vector<double>>> RunAction::ReadReflectData(std::string filename,
	std::vector<double> &angle, std::vector<double> &energy)
{
	// Reads a CSV file into a vector of <string, vector<double>> pairs where
	// each pair represents <column name, column values>

	std::vector<std::pair<double, std::vector<double>>> dataset;

	// Create an input filestream
    	std::ifstream myFile(filename);

    	// make sure the file is open
    	if(!myFile.is_open()) 
		throw std::runtime_error(
			Form("\n\n[ERROR] RunAction::ReadCSV ==> Cannot open of find the file: %s\n",filename.c_str()));

    	// Helper vars
    	std::string line, colname;
    	double val;

    	// Read the column names
    	if(myFile.good())
    	{
        	// Extract the first line in the file
        	std::getline(myFile, line);

        	// Create a stringstream from line
        	std::stringstream ss(line);

        	// Extract each column name
		std::getline(ss, colname, ','); // skip the first column
        	while(std::getline(ss, colname, ','))
		{
			val = std::stod(colname);
			// Initialize and add <colname, double vector> pairs to dataset
            		dataset.push_back({val, std::vector<double> {}});
			angle.push_back(val);
        	}
    	}

    	// Read data, line by line
    	while(std::getline(myFile, line))
    	{
        	// Create a stringstream of the current line
        	std::stringstream ss(line);
        
        	// Keep track of the current column index
        	int colIdx = 0;
        
        	// Extract each value
        	while(ss >> val)
		{
            		// Add the current integer to the 'colIdx' column's values vector
            		if(colIdx == 0) energy.push_back(val);
			else dataset.at(colIdx-1).second.push_back(val);
            
            		// If the next token is a comma, ignore it and move on
            		if(ss.peek() == ',') ss.ignore();
            
            		// Increment the column index
            		colIdx++;
        	}
    	}

    	// Close file
    	myFile.close();

	return dataset;
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
