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

#define NMAXFILES INT_MAX // maximum number of file to read

void getFileNames(const string directory, const string pattern, const string extension);
void drawHistograms(const string hname, const string title, const string outputdir);

vector<string> _fileNames;

const Double_t BeamCurrent_A = 0.227; // [A]
const Double_t nElectronsPerFile = 100000.0;

void drawAllHits(
	const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask5_1_ip_cut2/",
	const string pattern = ".hist", 
	const string extension = ".root",
	const string outputdir = "./pics")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	cout<<"--> directory = "<<directory<<endl;
	cout<<"--> pattern = "<<pattern<<endl;
	cout<<"--> extension = "<<extension<<endl;
	cout<<"--> outputdir = "<<outputdir<<endl;

	getFileNames(directory, pattern, extension);
        drawHistograms("hSimHits","SimHits",outputdir);
        drawHistograms("hRawHits","RawHits",outputdir);
        drawHistograms("hRecHits","RecHits",outputdir);

	return;
}

void drawHistograms(const string hname, const string title, const string outputdir)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	gStyle->SetOptStat(0);

	Int_t nFiles = 0, iFile = 0;

	TFile *_file0;
	TH1D* hAllHits;
	Bool_t ready = kFALSE;

	while(!ready)
	{
		_file0 = TFile::Open(_fileNames.at(iFile).c_str());
		iFile++;
		if(!_file0)continue;
		hAllHits = (TH1D*)_file0->Get(hname.c_str());
		hAllHits->SetName("hAllHits");
		ready = !ready;
	}

	while(iFile < _fileNames.size()) 
	{
		cout<<hname<<" | "<<iFile+1<<"/"<<_fileNames.size()<<" : "<<_fileNames.at(iFile)<<endl;
		
		TFile *_file1 = TFile::Open(_fileNames.at(iFile).c_str());
		iFile++;
		if(!_file1)continue;

		TH1D* hHits = (TH1D*)_file1->Get(hname.c_str());
		hAllHits->Add(hHits);
		_file1->Close();
		nFiles++;
	}

	gStyle->SetErrorX(0);
	gStyle->SetOptStat(0);

	TCanvas* c0 = new TCanvas("c0","c0",900,900);
	c0->cd();
	TPad *pad0 = new TPad("pad0","pad0",0,0,1,1);
	pad0->SetBottomMargin(0.4);
	pad0->SetTopMargin(0.1);
	pad0->SetRightMargin(0.1);
	pad0->SetLeftMargin(0.12);
	pad0->Draw();
	pad0->cd();
	hAllHits->Draw("bar1 & e");
	hAllHits->SetTitle(title.c_str());
	hAllHits->GetYaxis()->SetTitle("Total num. of hits [counts]");
	hAllHits->SetFillColorAlpha(kBlue,0.4);
	hAllHits->SetLineWidth(2);
	hAllHits->SetLineColor(kBlack);
	hAllHits->SetBarWidth(0.8);
	hAllHits->SetMinimum(1e-6);
	hAllHits->SetMaximum(1e6);
	hAllHits->GetYaxis()->SetTitleOffset(1.4);
	hAllHits->GetXaxis()->SetTitleOffset(3.0);
	hAllHits->GetXaxis()->CenterTitle(true);
	hAllHits->GetYaxis()->CenterTitle(true);
	hAllHits->GetXaxis()->SetLabelSize(0.04);
	hAllHits->GetXaxis()->SetLabelOffset(0.01);
	hAllHits->GetXaxis()->SetTitle("");
	hAllHits->LabelsOption("v");
	gPad->SetLogy();
	gPad->SetGrid();

	c0->SaveAs(Form("%s/c0_%s.png",outputdir.c_str(),hname.c_str()));
	hAllHits->SaveAs(Form("%s/hAllCounts_%s.root",outputdir.c_str(),hname.c_str()));

	hAllHits->Scale(BeamCurrent_A/(nElectronsPerFile*nFiles*TMath::Qe()));
	cout<<"--> Scaling = "<<BeamCurrent_A/(nElectronsPerFile*nFiles*TMath::Qe())<<endl;

	TCanvas* c1 = new TCanvas("c1","c1",900,900);
	c1->cd();
	TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
	pad1->SetBottomMargin(0.4);
	pad1->SetTopMargin(0.1);
	pad1->SetRightMargin(0.1);
	pad1->SetLeftMargin(0.12);
	pad1->Draw();
	pad1->cd();
	hAllHits->Draw("bar1 & e");
	hAllHits->GetYaxis()->SetTitle("Hit rate [Hz]");
	hAllHits->SetMinimum(1e6);
	hAllHits->SetMaximum(1e16);
	gPad->SetLogy();
	gPad->SetGrid();

	c1->SaveAs(Form("%s/c1_%s.png",outputdir.c_str(),hname.c_str()));
	hAllHits->SaveAs(Form("%s/hAllHits_%s.root",outputdir.c_str(),hname.c_str()));

	_file0->Close();
	delete c0;
	delete c1;

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

