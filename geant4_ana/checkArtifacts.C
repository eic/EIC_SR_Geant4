/********************************/
/********************************/
/* Plot SR tracks		*/
/* Run in eic-shell		*/
/********************************/
/********************************/
#include "TROOT.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"
#include "TTreeReaderArray.h"
#include "TTreeReader.h"
#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

#define NMAXFILES INT_MAX // maximum number of file to r aximum number of file to read

void getFileNames(const string directory, const string extension); 
void getStoredTracks(const string outputfile);

vector<string> _fileNames;

void checkArtifacts(	const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_g4sim_v6.2_srmask5_1_ip_cut3/",
			const string extension = ".root",
			const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_g4sim_v6.2_srmask5_1_ip_cut3_tracks.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	//-- get data from geant4 simulation output data
	getFileNames(directory,extension);
	getStoredTracks(outputfile);

	return;
}

void getStoredTracks(const string outputfile)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	for(Int_t ii = 0; ii < _fileNames.size(); ii++)
	{
		// Declaration of leaf types
		vector<float>   *trackPosX_cm = 0;
		vector<float>   *trackPosY_cm = 0;
		vector<float>   *trackPosZ_cm = 0;
		vector<float>   *trackEne_GeV = 0;
		TChain *fChain = new TChain("tracks");

		fChain->SetBranchAddress("trackPosX_cm", &trackPosX_cm);
		fChain->SetBranchAddress("trackPosY_cm", &trackPosY_cm);
		fChain->SetBranchAddress("trackPosZ_cm", &trackPosZ_cm);
		fChain->SetBranchAddress("trackEne_GeV", &trackEne_GeV);

		fChain->AddFile(_fileNames.at(ii).c_str());

		Long64_t nentries = fChain->GetEntries();

		for(Long64_t i = 0; i < nentries; i++)
		{
			// get track
			fChain->GetEntry(i);

			// loop over track steps
			for(Int_t j = 0; j < trackPosZ_cm->size(); j++)
			{
				if(trackPosZ_cm->at(j) < -1000){cout<<_fileNames.at(ii)<<endl; 

			for(Int_t jj = 0; jj < trackPosZ_cm->size(); jj++)
			{cout<<trackPosX_cm->at(jj)<<" , "<<trackPosY_cm->at(jj)<<" , "<<trackPosZ_cm->at(jj)<<"; "<<trackEne_GeV->at(jj)*1e6<<endl;}


exit(-1);}
			}
		}

		delete fChain->GetCurrentFile();
	}

	return;
}

void getFileNames(const string directory, const string extension) 
{
	try 
	{
        	for(const auto& entry : fs::directory_iterator(directory)) 
		{
			if(	entry.is_regular_file() && 
				entry.path().extension() == extension)
			{
				_fileNames.push_back(entry.path());
				if(_fileNames.size() >= NMAXFILES){return;}
            		}
        	}
    	} 
	catch (const filesystem::filesystem_error& ex) 
	{
        	cerr << "Error: " << ex.what() << endl;
    	}
	return;
}
