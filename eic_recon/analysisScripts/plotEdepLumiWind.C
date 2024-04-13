/********************************/
/********************************/
/* Plot detector hit rates	*/
/* Run in eic-shell		*/
/********************************/
/********************************/
#include "TROOT.h"
#include "TH2F.h"
#include "TH2D.h"
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

#define NMAXFILES INT_MAX // maximum number of file to read

void getFileNames(const string directory, const string pattern, const string extension);
void readDataFromFiles(const string outputfile);

vector<string> _fileNames;

void plotEdepLumiWind(	const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_v6.2_nosrmask_farBWD_newMaterialsAndCoating/", 
			const string pattern = ".podio", 
			const string extension = ".root", 
			const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_v6.2_nosrmask_farBWD_newMaterialsAndCoating_plotEdepLumiWind.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	//-- get data from eic-shell output data
	getFileNames(directory, pattern, extension);
	readDataFromFiles(outputfile);

	return;
}

void getFileNames(const string directory, const string pattern, const string extension) 
{
	try 
	{
        	for(const auto& entry : fs::directory_iterator(directory)) 
		{
			if(	entry.is_regular_file() && 
				entry.path().filename().string().find(pattern) != string::npos && 
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

void readDataFromFiles(const string outputfile)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	TFile* f = new TFile(outputfile.c_str(),"recreate");
	
	TH1D* h1 = new TH1D("h1","LumiWindowCALHits; E_{dep} [GeV]",1e6,0,0.1);
	//-----------------------------------------------------------//
	// loop over files
	for(Int_t iFile = 0; iFile < _fileNames.size(); iFile++)
	{
		cout<<iFile+1<<"/"<<_fileNames.size()<<" : "<<_fileNames.at(iFile)<<endl;

		TFile* file = new TFile(_fileNames.at(iFile).c_str());
		TTreeReader myReader("events", file);
		
		//- SimHits
		TTreeReaderArray<Float_t> var_LumiWindowCALHits(myReader, "LumiWindowCALHits.energy");
		//-----------------------------------------------------------//
		// loop over events
		while(myReader.Next())
		{
			//- Get # of SimHits
			Long64_t nSimHit = var_LumiWindowCALHits.GetSize();

			//- Loop over hits
			for(Long64_t iHit = 0; iHit < nSimHit; iHit++)
			{
				//- Fill the histogram
				h1->Fill(var_LumiWindowCALHits[iHit]);
			}
		}
		file->Close();
	}
	//-----------------------------------------------------------//
	cout<<"Writing to a file ..."<<endl;
	if(!f)
	{
		cout<<"[ERROR] Cannot recreate the output file: "<<outputfile<<endl;
		return;
	}
	f->cd();
	h1->Write();
	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}
