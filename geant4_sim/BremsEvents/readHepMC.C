#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

using namespace std;

void readHepMC(TString fName = "lumi_18x275_bx_emin0p5_T3p3_10Mevt.hepmc")
{
    // Open the input TXT file
    ifstream inputFile(fName.Data());
    if (!inputFile.is_open()) {
        cerr << "Error opening input file." << endl;
        return;
    }

    // Create a ROOT file and a TTree
    TFile *outputFile = new TFile(Form("%s.root",fName.Data()), "recreate");
    TTree *tree = new TTree("tree", "HepMC data");

    // Define variables to store data from the TXT file
    int eventNumber = -1;
    double x, y, z;
    double true_el_E, true_el_phi, true_el_theta;
    double true_phot_E, true_phot_delta, true_phot_phi, true_phot_theta, true_phot_theta_n, true_phot_w;
    vector<int> id, pdg, status;
    vector<double> px, py, pz, E, mass;
    string particleType;
    double particleValue;

    // Create branches in the TTree
    tree->Branch("eventNumber", &eventNumber, "eventNumber/I");
    tree->Branch("x", &x, "x/D");
    tree->Branch("y", &y, "y/D");
    tree->Branch("z", &z, "z/D");
    tree->Branch("true_el_E", &true_el_E, "true_el_E/D");
    tree->Branch("true_el_phi", &true_el_phi, "true_el_phi/D");
    tree->Branch("true_el_theta", &true_el_theta, "true_el_theta/D");
    tree->Branch("true_phot_E", &true_phot_E, "true_phot_E/D");
    tree->Branch("true_phot_delta", &true_phot_delta, "true_phot_delta/D");
    tree->Branch("true_phot_phi", &true_phot_phi, "true_phot_phi/D");
    tree->Branch("true_phot_theta", &true_phot_theta, "true_phot_theta/D");
    tree->Branch("true_phot_theta_n", &true_phot_theta_n, "true_phot_theta_n/D");
    tree->Branch("true_phot_w", &true_phot_w, "true_phot_w/D");
    tree->Branch("id", &id);
    tree->Branch("pdg", &pdg);
    tree->Branch("status", &status);
    tree->Branch("px", &px);
    tree->Branch("py", &py);
    tree->Branch("pz", &pz);
    tree->Branch("E", &E);
    tree->Branch("mass", &mass);

    // Read the TXT file line by line
    string line;
    while (getline(inputFile, line)) {
        // Process the line based on its content
        if (line.find("E") == 0) 
	{
		// Fill the TTree with the data
		if(eventNumber >= 0)
		{
			tree->Fill();
			// Clean vectors
			id.clear();
			pdg.clear();
			px.clear();
			py.clear();
			pz.clear();
			E.clear();
			mass.clear();
			status.clear();
		}

            	// Read event header information
            	sscanf(line.c_str(), "E %d 0 2 @ %lf %lf %lf 0.0", &eventNumber, &x, &y, &z);
            	if(eventNumber % 10000 == 0){cout<<"--> Event: "<<eventNumber<<endl;}
        } 
	else if (line.find("A 0") == 0) 
	{
            	// Read particle information
            	sscanf(line.c_str(), "A 0 %s %lf", &particleType[0], &particleValue);
		particleType = particleType.c_str();
            	if (particleType == "true_el_E") {
                	true_el_E = particleValue;
            	} else if (particleType == "true_el_phi") {
                	true_el_phi = particleValue;
            	} else if (particleType == "true_el_theta") {
                	true_el_theta = particleValue;
            	} else if (particleType == "true_phot_E") {
                	true_phot_E = particleValue;
            	} else if (particleType == "true_phot_delta") {
                	true_phot_delta = particleValue;
            	} else if (particleType == "true_phot_phi") {
                	true_phot_phi = particleValue;
            	} else if (particleType == "true_phot_theta") {
                	true_phot_theta = particleValue;
            	} else if (particleType == "true_phot_theta_n") {
                	true_phot_theta_n = particleValue;
            	} else if (particleType == "true_phot_w") {
                	true_phot_w = particleValue;
            	}
        } 
	else if (line.find("P") == 0) 
	{
            // Read particle momentum information
    		int id_tmp, pdg_tmp, status_tmp;
    		double px_tmp, py_tmp, pz_tmp, E_tmp, mass_tmp;

            sscanf(line.c_str(), "P %d 0 %d %lf %lf %lf %lf %lf %d", 
		&id_tmp, &pdg_tmp, &px_tmp, &py_tmp, &pz_tmp, &E_tmp, &mass_tmp, &status_tmp);

	    	id.push_back(id_tmp);
	    	pdg.push_back(pdg_tmp);
		px.push_back(px_tmp);
		py.push_back(py_tmp);
		pz.push_back(pz_tmp);
		E.push_back(E_tmp);
		mass.push_back(mass_tmp);
		status.push_back(status_tmp);
        }
	else if(line == "HepMC::Asciiv3-END_EVENT_LISTING")
	{
        	// Fill the TTree with the data
        	tree->Fill();
	}
    }

    // Close the input file and ROOT file
    inputFile.close();
    outputFile->Write();
    outputFile->Close();
    cout<<"Output file: "<<outputFile->GetName()<<endl;

    return;
}
