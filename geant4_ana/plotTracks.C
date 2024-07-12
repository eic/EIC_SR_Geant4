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
void drawStoredTracks(const string fname, const string picsdir);

vector<string> _fileNames;

void plotTracks(const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_srmask5_extendedBWD_stdFWD_highstat/",
		const string extension = ".root",
		const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/output_g4sim_v6.3_srmask5_extendedBWD_stdFWD_highstat_tracks.root",
		const string picsdir = "pics_v6.3_srmask5_extendedBWD_stdFWD_highstat_tracks")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	//-- get data from geant4 simulation output data
	getFileNames(directory,extension);
	getStoredTracks(outputfile);
	//-- draw tracks
	drawStoredTracks(outputfile,picsdir);

	return;
}

void drawStoredTracks(const string fname, const string picsdir)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	gStyle->SetOptStat(0);

	TFile *file = TFile::Open(fname.c_str());
	TH2F* h1_xz = (TH2F*)file->Get("h1_xz");
	TH2F* h1_yz = (TH2F*)file->Get("h1_yz");
	TH2F* h1_rz = (TH2F*)file->Get("h1_rz");
	TH1F* h1_en = (TH1F*)file->Get("h1_en");

	TCanvas* c1 = new TCanvas("c1","c1",1800,900); 
	c1->Divide(1,3);

	c1->cd(1);
        TPad *pad11 = new TPad("pad11","pad11",0,0,1,1);
        pad11->SetBottomMargin(0.12);
        pad11->SetTopMargin(0.12);
        pad11->SetRightMargin(0.12);
        pad11->SetLeftMargin(0.12);
        pad11->Draw();
        pad11->cd();
	gPad->SetGrid();
	h1_xz->SetMinimum(1);
	h1_xz->SetMaximum(1e6);
	h1_xz->Draw("colz");
	gPad->SetLogz();

	c1->cd(2);
        TPad *pad12 = new TPad("pad12","pad12",0,0,1,1);
        pad12->SetBottomMargin(0.12);
        pad12->SetTopMargin(0.12);
        pad12->SetRightMargin(0.12);
        pad12->SetLeftMargin(0.12);
        pad12->Draw();
        pad12->cd();
	gPad->SetGrid();
	h1_yz->SetMinimum(1);
	h1_yz->SetMaximum(1e6);
	h1_yz->Draw("colz");
	gPad->SetLogz();

	c1->cd(3);
        TPad *pad13 = new TPad("pad13","pad13",0,0,1,1);
        pad13->SetBottomMargin(0.12);
        pad13->SetTopMargin(0.12);
        pad13->SetRightMargin(0.12);
        pad13->SetLeftMargin(0.12);
        pad13->Draw();
        pad13->cd();
	gPad->SetGrid();
	h1_rz->SetMinimum(1);
	h1_rz->SetMaximum(1e6);
	h1_rz->Draw("colz");
	gPad->SetLogz();

	c1->SaveAs(Form("%s/c1_tracks.png",picsdir.c_str()));
	c1->SaveAs(Form("%s/c1_tracks.root",picsdir.c_str()));
	h1_xz->SaveAs(Form("%s/h1_xz_tracks.root",picsdir.c_str()));
	h1_yz->SaveAs(Form("%s/h1_yz_tracks.root",picsdir.c_str()));
	h1_rz->SaveAs(Form("%s/h1_rz_tracks.root",picsdir.c_str()));

	TCanvas* c2 = new TCanvas("c2","c2",900,900);
	c2->cd();
	TPad *pad2 = new TPad("pad2","pad2",0,0,1,1);
        pad2->SetBottomMargin(0.12);
        pad2->SetTopMargin(0.12);
        pad2->SetRightMargin(0.12);
        pad2->SetLeftMargin(0.12);
        pad2->Draw();
        pad2->cd();
	gPad->SetGrid();
	h1_en->SetLineWidth(2);
	h1_en->SetLineColor(kBlue);
	h1_en->SetFillColorAlpha(kCyan,0.3);
	h1_en->Draw("HIST");
	gPad->SetLogy();

	c2->SaveAs(Form("%s/c2_tracks.png",picsdir.c_str()));
	c2->SaveAs(Form("%s/c2_tracks.root",picsdir.c_str()));
	h1_en->SaveAs(Form("%s/h1_en_tracks.root",picsdir.c_str()));

	return;
}

void getStoredTracks(const string outputfile)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	// Declaration of leaf types
	vector<float>   *trackPosX_cm = 0;
	vector<float>   *trackPosY_cm = 0;
	vector<float>   *trackPosZ_cm = 0;
	vector<float>   *trackEne_GeV = 0;
	TChain *fChain = new TChain("tracks");

	for(Int_t i = 0; i < _fileNames.size(); i++)
	{
		cout<<"File name: "<<_fileNames.at(i)<<endl;
		fChain->AddFile(_fileNames.at(i).c_str());
	}

	fChain->SetBranchAddress("trackPosX_cm", &trackPosX_cm);
	fChain->SetBranchAddress("trackPosY_cm", &trackPosY_cm);
	fChain->SetBranchAddress("trackPosZ_cm", &trackPosZ_cm);
	fChain->SetBranchAddress("trackEne_GeV", &trackEne_GeV);

	Long64_t nentries = fChain->GetEntries();
	
	Double_t x0, x1, y0, y1, z0, z1, r0, r1;
	Double_t Lx, Ly, Lr, dLx, dLy, dLr;
	Double_t sinThetaX, cosThetaX, sinThetaY, cosThetaY, sinThetaR, cosThetaR;

	TH2F* h1_xz = new TH2F("h1_xz","XZ;Z [cm];X [cm]",5000,-5000,5000,4000,-40,40);
	TH2F* h1_yz = new TH2F("h1_yz","YZ;Z [cm];Y [cm]",5000,-5000,5000,4000,-40,40);
	TH2F* h1_rz = new TH2F("h1_rz","RZ;Z [cm];R [cm]",5000,-5000,5000,4000,-40,40);
	TH1F* h1_en = new TH1F("h1_en","E_{#gamma};E_{#gamma} [keV]",1000,0,1000);
	// loop over tracks
	for(Long64_t i = 0; i < nentries; i++)
	{
		// get track
		fChain->GetEntry(i);
		h1_en->Fill(trackEne_GeV->back()*1e6); // [GeV --> keV]

		// loop over track steps
		for(Int_t j = 1; j < trackPosX_cm->size(); j++)
		{
			// initial coordinates for the step
			x0 = trackPosX_cm->at(j-1);
			y0 = trackPosY_cm->at(j-1);
			z0 = trackPosZ_cm->at(j-1);
			r0 = TMath::Sqrt(x0*x0 + y0*y0);
			// final coordinates for the step
			x1 = trackPosX_cm->at(j);
			y1 = trackPosY_cm->at(j);
			z1 = trackPosZ_cm->at(j);
			r1 = TMath::Sqrt(x1*x1 + y1*y1);
			// step length
			Lx = TMath::Sqrt(	TMath::Power(x1-x0,2) + 
						TMath::Power(z1-z0,2));
			Ly = TMath::Sqrt(	TMath::Power(y1-y0,2) + 
						TMath::Power(z1-z0,2));
			Lr = TMath::Sqrt(       TMath::Power(r1-r0,2) +
						TMath::Power(z1-z0,2)); 
			// sub-step length along the step
			dLx = 1.0; // [cm]
			dLy = 1.0; // [cm]
			dLr = 1.0; // [cm]
			// angles
			sinThetaX = (x1-x0)/Lx;
			cosThetaX = (z1-z0)/Lx;
			sinThetaY = (y1-y0)/Ly;
			cosThetaY = (z1-z0)/Ly;
			sinThetaR = (r1-r0)/Lr;
			cosThetaR = (z1-z0)/Lr;
			// loop over sub-steps and fill
			for(Double_t ll = 0; ll < Lx; ll += dLx)
				h1_xz->Fill(z0+ll*cosThetaX,x0+ll*sinThetaX,1);
			for(Double_t ll = 0; ll < Ly; ll += dLy)
				h1_yz->Fill(z0+ll*cosThetaY,y0+ll*sinThetaY,1);
			for(Double_t ll = 0; ll < Lr; ll += dLr)
				h1_rz->Fill(z0+ll*cosThetaR,r0+ll*sinThetaR,1);
			// fill the last coordinates of the step
			h1_xz->Fill(z1,x1,1);
			h1_yz->Fill(z1,y1,1);
			h1_rz->Fill(z1,r1,1);
		}
	}

	// write the multigraph to file
	TFile* file = new TFile(outputfile.c_str(),"recreate");
	file->cd();
	h1_xz->Write();
	h1_yz->Write();
	h1_rz->Write();
	h1_en->Write();
	file->Close();
	cout<<"Output file: "<<file->GetName()<<endl;

	delete fChain->GetCurrentFile();
	delete file;

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
