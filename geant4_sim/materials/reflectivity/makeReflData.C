void ReadHenkeReflectivity(
	TString fName,
	vector<double>& photonEnergy, 
	vector<double>& reflectivity,
	vector<double>& transmission);
void PrintData(const map<string, vector<vector<double>>>& dataStorage);
void WriteDataToCSV(
	const string& outputFileName, 
	const map<string, vector<vector<double>>>& dataStorage);

const Int_t angleNum = 17;
const Double_t angleRad[angleNum] = {0.001,0.002,0.003,0.005,0.01,0.015,0.02,0.03,0.05,0.08,0.125,0.2,0.4,0.6,0.8,1.25,1.5};

void makeReflData()
{
	const Int_t Enum = 4;
	Int_t Emin[] = {30, 620, 1405, 1700};
	Int_t Emax[] = {600, 1400, 1600, 30000};
	Int_t Estep[] = {10, 20, 5, 100};

	// TString dirName = "Cu_30000.0nmCu";
	// TString dirName = "Cu_4000000.0nmCu";
	TString dirName = "Cu_1000.0nmTiZrV";

	vector<double> photonEnergy;
	vector<double> reflectivity;
	vector<double> transmission;

	// Create a map to store vectors for each data set
    	map<string, vector<vector<double>>> dataStorage;

	for(Int_t j = 0; j < angleNum; j++)
	{
		for(Int_t i = 0; i < Enum; i++)
		{
			TString fName = TString::Format(
				"%s/%d_%d_%05.2f_raw.dat",
				dirName.Data(),Emin[i],Emax[i],angleRad[j]*180.0/TMath::Pi());
			cout<<"[FILE] "<<fName<<endl;
			ReadHenkeReflectivity(fName,photonEnergy,reflectivity,transmission);
			dataStorage[fName.Data()] = {photonEnergy,reflectivity,transmission};

			photonEnergy.clear();
			reflectivity.clear();
			transmission.clear();
		}
	}

	PrintData(dataStorage);
	WriteDataToCSV(Form("%s.csv",dirName.Data()),dataStorage);

	return;
}

void WriteDataToCSV(
	const string& outputFileName, 
	const map<string, vector<vector<double>>>& dataStorage) 
{
    	// Open the output CSV file
    	ofstream outputFile(outputFileName);

    	if (!outputFile.is_open()) 
	{
        	cerr << "Error: Unable to open file: " << outputFileName << endl;
        	return;
    	}

    	// Create a set to store unique photon energy values across all files
    	set<int> uniquePhotonEnergies;

    	// Fill the set with photon energy values from all files
    	for (const auto& entry : dataStorage) 
	{
        	const vector<double>& photonEnergy = entry.second[0];
        	uniquePhotonEnergies.insert(photonEnergy.begin(), photonEnergy.end());
    	}

    	// Write CSV header
    	outputFile << "Energy? Angle?";
	for(size_t j = 0; j < angleNum; j++)
	{
		outputFile << ',' << angleRad[j];
	}
	outputFile << '\n';

    	// Loop over each unique photon energy
    	for (double energy : uniquePhotonEnergies) 
	{
        	outputFile << int(energy); // Start the row with the photon energy

        	// For each file, find the corresponding reflectivity and transmission values
        	for (const auto& entry : dataStorage) 
		{
            		const vector<double>& photonEnergy = entry.second[0];
            		const vector<double>& reflectivity = entry.second[1];
            		const vector<double>& transmission = entry.second[2];

            		// Search for the energy in the file's photonEnergy vector
            		auto it = find(photonEnergy.begin(), photonEnergy.end(), energy);

            		if (it != photonEnergy.end()) 
			{
                		// If found, retrieve the index and write reflectivity and transmission values
                		size_t index = distance(photonEnergy.begin(), it);
                		outputFile << "," << scientific << setprecision(5) << reflectivity[index];
            		} 
        	}
       		outputFile << "\n"; // End the row for this photon energy
    	}

    	// Close the file
    	outputFile.close();

    	cout << "Data written to " << outputFileName << endl;
}

void PrintData(const map<string, vector<vector<double>>>& dataStorage)
{
    	// Loop over all files in dataStorage
    	for (const auto& entry : dataStorage) 
	{
        	const string& fileName = entry.first;
        	const vector<vector<double>>& dataVectors = entry.second;

        	// Print file name
        	cout << "Data from file: " << fileName << endl;

        	// Ensure we have three vectors: photon energy, reflectivity, transmission
        	if (dataVectors.size() == 3) 
		{
            		const vector<double>& photonEnergy = dataVectors[0];
            		const vector<double>& reflectivity = dataVectors[1];
            		const vector<double>& transmission = dataVectors[2];

            		// Loop through the data and print the corresponding values
            		for (size_t i = 0; i < photonEnergy.size(); ++i) 
			{
                		cout 	<< "Photon Energy: " << photonEnergy[i]
                          		<< ", Reflectivity: " << reflectivity[i]
                          		<< ", Transmission: " << transmission[i] << endl;
            		}
        	} 
		else 
		{
            		cerr << "Error: Data format for file " << fileName << " is incorrect." << endl;
        	}

        	cout << "---------------------------------" << endl;
    	}
	return;
}

void ReadHenkeReflectivity(
	TString fName,
	vector<double>& photonEnergy, 
	vector<double>& reflectivity,
	vector<double>& transmission)
{
	ifstream inputFile(fName.Data());
    	string line;

    	// Check if the file opened successfully
    	if (!inputFile.is_open()) 
	{
        	cerr << "Error opening file!" << endl;
		exit(-1);
    	}

    	// Skip lines until the data starts
    	while (getline(inputFile, line)) 
	{
        	if (line.find("Photon energy (eV)") != string::npos) 
		{
            		break;
        	}
    	}

    	// Read data lines
    	while (getline(inputFile, line)) 
	{
        	istringstream iss(line);
        	double energy, refl, trans;
        
        	// Check if we can read three values from the line
        	if (iss >> energy >> refl >> trans) 
		{
            		photonEnergy.push_back(energy);
            		reflectivity.push_back(refl);
            		transmission.push_back(trans);
        	}
    	}

    	inputFile.close();
    
	return;
}
