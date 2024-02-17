/********************************/
/********************************/
/* Plot detector hit rates	*/
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

void getFileNames(const string directory, const string pattern, const string extension);
void readDataFromFiles(const string outputfile); 
void drawHistograms(const string filename);
Double_t getBeamCoreTailRatio(TString type = "core");

vector<string> _fileNames;

const Double_t BeamCurrent_A = 0.227; // [A]
const Double_t nElectrons_geant4 = 100000.0 * 10000.0;
const Int_t subDetNum = 11;

void plotHits(	const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_nosrmask_ultrahighstat_tmp/",
		const string pattern = ".hist",
		const string extension = ".root",
		const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_nosrmask_ultrahighstat_tmp_plotHits.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	//-- get data from eic-shell output data 
	getFileNames(directory, pattern, extension);
	readDataFromFiles(outputfile);

	//-- draw histograms from above
	drawHistograms(outputfile);

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

	TH1D* hHits = new TH1D("hHits","Hits;Sub-detector;Hits [counts]",subDetNum,0,subDetNum);

	const TString bName[subDetNum] = {	"TrackerEndcapHits","HcalEndcapNHits","EcalEndcapNHits",
						"VertexBarrelHits","TOFBarrelHits","SiBarrelHits",
						"MPGDBarrelHits","EcalEndcapPHits","B0TrackerHits",
						"HcalBarrelHits","LumiSpecCALHits"};

	//-----------------------------------------------------------//
	Float_t nHits[subDetNum], nHitsSum[subDetNum];

	// empty array
	for(Int_t i = 0; i < subDetNum; i++){nHitsSum[i] = 0;}

	// loop over files
	for(Int_t iFile = 0; iFile < _fileNames.size(); iFile++)
	{
		cout<<iFile+1<<"/"<<_fileNames.size()<<" : "<<_fileNames.at(iFile)<<endl;

		TFile* file = new TFile(_fileNames.at(iFile).c_str());
		TDirectory* dir = file->GetDirectory("analysisPlugin");
		// dir->ls();
	
		TNtuple* ntraw_nHits = (TNtuple*)dir->Get("ntraw_nHits");	
		Int_t nEntries_nHits = ntraw_nHits->GetEntries();

		for(Int_t i = 0; i < subDetNum; i++)
		{
			ntraw_nHits->SetBranchAddress(Form("nHits_%s",bName[i].Data()),&nHits[i]);
		}

		// loop over events in the ntuple
		for(Int_t i = 0; i < nEntries_nHits; i++)
		{
			ntraw_nHits->GetEntry(i);
			for(Int_t j = 0; j < subDetNum; j++)
			{
				nHitsSum[j] += nHits[j];
			}
		}

		file->Close();
	}
	//-----------------------------------------------------------//

	// fill histograms
	for(Int_t i = 0; i < subDetNum; i++)
	{
		hHits->SetBinContent(i+1,nHitsSum[i]);
		hHits->SetBinError(i+1,TMath::Sqrt(nHitsSum[i]));
		hHits->GetXaxis()->SetBinLabel(i+1,bName[i].Data());
	}

	cout<<"Writing to a file ..."<<endl;
	if(!f)
	{
		cout<<"[ERROR] Cannot recreate the output file: "<<outputfile<<endl;
		return;
	}
	f->cd();

	hHits->Write(); 

	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}

Double_t getBeamCoreTailRatio(TString type = "core")
{
	const Double_t Kx = 0.0;
	const Double_t Ky = 25e-7;
	const Double_t tailmin = 5; 
	const Double_t tailmax = 20; 

	// core, integral of gauss [0;inf] = 1/2 x [-inf;inf]
	Double_t Ic = TMath::Sqrt(TMath::PiOver2());
	// tail, integral of K_r/r^3 [tailmin;tailmax]
	Double_t xIt = (-0.5)*Kx*(1./TMath::Power(tailmax,2)-1./TMath::Power(tailmin,2)); // x-axis
	Double_t yIt = (-0.5)*Ky*(1./TMath::Power(tailmax,2)-1./TMath::Power(tailmin,2)); // y-axis
	// core + tail
	Double_t xIct = Ic + xIt; // x-axis 
	Double_t yIct = Ic + yIt; // y-axis 
	// ratio = core/(core+tail)
	Double_t Rc = Ic*Ic/(xIct*yIct);
	
	return (type == "core") ? (Rc /*core*/) : (1.0 - Rc /*tail*/);
}

void drawHistograms(const string filename)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	gStyle->SetOptStat(0);
	TFile *_file0 = TFile::Open(filename.c_str());

	TH1D* hHits = (TH1D*)_file0->Get("hHits");

	/* core */	hHits->Scale(BeamCurrent_A/(nElectrons_geant4*TMath::Qe()));
	/* tail */	// hHits->Scale(BeamCurrent_A/(nElectrons_geant4*TMath::Qe()));

	/* core */	// hHits->Scale(getBeamCoreTailRatio("core")*BeamCurrent_A/(nElectrons_geant4*TMath::Qe()));
	/* tail */	// hHits->Scale(getBeamCoreTailRatio("tail")*BeamCurrent_A/(nElectrons_geant4*TMath::Qe()));

	gStyle->SetErrorX(0);
	gStyle->SetOptStat(0);

	TCanvas* c1 = new TCanvas("c1","c1",2000,2000);
	c1->cd();
	TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
	pad1->SetBottomMargin(0.25);
	pad1->SetTopMargin(0.1);
	pad1->SetRightMargin(0.1);
	pad1->SetLeftMargin(0.12);
	pad1->Draw();
	pad1->cd();
	hHits->Draw("bar1 & e");
	hHits->SetFillColorAlpha(kBlue,0.4);
	hHits->SetLineWidth(2);
	hHits->SetLineColor(kBlack);
	hHits->SetBarWidth(0.8);
//	hHits->SetMinimum(1e3);
//	hHits->SetMaximum(1e8);
	hHits->GetYaxis()->SetTitleOffset(1.2);
	hHits->GetXaxis()->SetTitleOffset(3.0);
	hHits->GetXaxis()->CenterTitle(true);
	hHits->GetYaxis()->CenterTitle(true);
	hHits->GetXaxis()->SetLabelSize(0.04);
	hHits->GetXaxis()->SetLabelOffset(0.01);
	hHits->GetXaxis()->SetTitle("");
	hHits->LabelsOption("v");
	gPad->SetLogy();

	c1->SaveAs("./pics/c1_hits.png");

	return;
}
