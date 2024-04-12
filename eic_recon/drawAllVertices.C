/********************************************************/
/********************************************************/
/* Plot MCparticle vertices and detector hit map 	*/
/* Run in eic-shell  					*/
/********************************************************/
/********************************************************/
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
void drawHistograms(const string outputdir);

vector<string> _fileNames;

void drawAllVertices(	
	const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask4_2_farBWD_newMaterialsAndCoating/", 
	const string pattern = ".vertex", 
	const string extension = ".root", 
	const string outputdir = "./pics")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	cout<<"--> directory = "<<directory<<endl;
	cout<<"--> pattern = "<<pattern<<endl;
	cout<<"--> extension = "<<extension<<endl;
	cout<<"--> outputdir = "<<outputdir<<endl;

	getFileNames(directory, pattern, extension);
	drawHistograms(outputdir);

	return;
}

void drawHistograms(const string outputdir)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	gStyle->SetOptStat(0);

	Int_t nFiles = 0, iFile = 0;

	TFile *_file0;
	TH2F* hMcPrimAll_xz;
	TH2F* hMcPrimAll_yz;
	TH2F* hMcPrimAll_yx;
	TH1F* hMcPrimAll_ene;
	TH2F* hVertexBarrelAll_xz;
	TH2F* hVertexBarrelAll_yz;
	TH2F* hVertexBarrelAll_yx;
	TH2F* hMcPrim_VertexBarrelAll_xz;
	TH2F* hMcPrim_VertexBarrelAll_yz;
	TH2F* hMcPrim_VertexBarrelAll_yx;
	TH1F* hVertexBarrelAll_edep;
	TH1F* hMcPrim_VertexBarrelAll_ene;
	TH2F* hTrackerEndcapAll_xz;
	TH2F* hTrackerEndcapAll_yz;
	TH2F* hTrackerEndcapAll_yx;
	TH2F* hMcPrim_TrackerEndcapAll_xz;
	TH2F* hMcPrim_TrackerEndcapAll_yz;
	TH2F* hMcPrim_TrackerEndcapAll_yx;
	TH1F* hTrackerEndcapAll_edep;
	TH1F* hMcPrim_TrackerEndcapAll_ene;
	Bool_t ready = kFALSE;

	while(!ready)
	{
		_file0 = TFile::Open(_fileNames.at(iFile).c_str());
		iFile++;
		if(!_file0)continue;

		hMcPrimAll_xz = (TH2F*)_file0->Get("hMcPrim_xz");
		hMcPrimAll_yz = (TH2F*)_file0->Get("hMcPrim_yz");
		hMcPrimAll_yx = (TH2F*)_file0->Get("hMcPrim_yx");
		hMcPrimAll_ene = (TH1F*)_file0->Get("hMcPrim_ene");

		hVertexBarrelAll_xz = (TH2F*)_file0->Get("hVertexBarrel_xz");
		hVertexBarrelAll_yz = (TH2F*)_file0->Get("hVertexBarrel_yz");
		hVertexBarrelAll_yx = (TH2F*)_file0->Get("hVertexBarrel_yx");
		hVertexBarrelAll_edep = (TH1F*)_file0->Get("hVertexBarrel_edep");
		hMcPrim_VertexBarrelAll_xz = (TH2F*)_file0->Get("hMcPrim_VertexBarrel_xz");
		hMcPrim_VertexBarrelAll_yz = (TH2F*)_file0->Get("hMcPrim_VertexBarrel_yz");
		hMcPrim_VertexBarrelAll_yx = (TH2F*)_file0->Get("hMcPrim_VertexBarrel_yx");
		hMcPrim_VertexBarrelAll_ene = (TH1F*)_file0->Get("hMcPrim_VertexBarrel_ene");

		hTrackerEndcapAll_xz = (TH2F*)_file0->Get("hTrackerEndcap_xz");
		hTrackerEndcapAll_yz = (TH2F*)_file0->Get("hTrackerEndcap_yz");
		hTrackerEndcapAll_yx = (TH2F*)_file0->Get("hTrackerEndcap_yx");
		hTrackerEndcapAll_edep = (TH1F*)_file0->Get("hTrackerEndcap_edep");
		hMcPrim_TrackerEndcapAll_xz = (TH2F*)_file0->Get("hMcPrim_TrackerEndcap_xz");
		hMcPrim_TrackerEndcapAll_yz = (TH2F*)_file0->Get("hMcPrim_TrackerEndcap_yz");
		hMcPrim_TrackerEndcapAll_yx = (TH2F*)_file0->Get("hMcPrim_TrackerEndcap_yx");
		hMcPrim_TrackerEndcapAll_ene = (TH1F*)_file0->Get("hMcPrim_TrackerEndcap_ene");

		hMcPrimAll_xz->SetName("hMcPrimAll_xz");
		hMcPrimAll_yz->SetName("hMcPrimAll_yz");
		hMcPrimAll_yx->SetName("hMcPrimAll_yx");
		hMcPrimAll_ene->SetName("hMcPrimAll_ene");
		hVertexBarrelAll_xz->SetName("hVertexBarrelAll_xz");
		hVertexBarrelAll_yz->SetName("hVertexBarrelAll_yz");
		hVertexBarrelAll_yx->SetName("hVertexBarrelAll_yx");
		hVertexBarrelAll_edep->SetName("hVertexBarrelAll_edep");
		hMcPrim_VertexBarrelAll_xz->SetName("hMcPrim_VertexBarrelAll_xz");
		hMcPrim_VertexBarrelAll_yz->SetName("hMcPrim_VertexBarrelAll_yz");
		hMcPrim_VertexBarrelAll_yx->SetName("hMcPrim_VertexBarrelAll_yx");
		hMcPrim_VertexBarrelAll_ene->SetName("hMcPrim_VertexBarrelAll_ene");
		hTrackerEndcapAll_xz->SetName("hTrackerEndcapAll_xz");
		hTrackerEndcapAll_yz->SetName("hTrackerEndcapAll_yz");
		hTrackerEndcapAll_yx->SetName("hTrackerEndcapAll_yx");
		hTrackerEndcapAll_edep->SetName("hTrackerEndcapAll_edep");
		hMcPrim_TrackerEndcapAll_xz->SetName("hMcPrim_TrackerEndcapAll_xz");
		hMcPrim_TrackerEndcapAll_yz->SetName("hMcPrim_TrackerEndcapAll_yz");
		hMcPrim_TrackerEndcapAll_yx->SetName("hMcPrim_TrackerEndcapAll_yx");
		hMcPrim_TrackerEndcapAll_ene->SetName("hMcPrim_TrackerEndcapAll_ene");

		ready = !ready;
	}

	while(iFile < _fileNames.size()) 
	{
		cout<<iFile+1<<"/"<<_fileNames.size()<<" : "<<_fileNames.at(iFile)<<endl;

		TFile *_file1 = TFile::Open(_fileNames.at(iFile).c_str());
		iFile++;
		if(!_file1)continue;

		TH2F* hMcPrim_xz = (TH2F*)_file1->Get("hMcPrim_xz");
		TH2F* hMcPrim_yz = (TH2F*)_file1->Get("hMcPrim_yz");
		TH2F* hMcPrim_yx = (TH2F*)_file1->Get("hMcPrim_yx");
		TH1F* hMcPrim_ene = (TH1F*)_file1->Get("hMcPrim_ene");

		TH2F* hVertexBarrel_xz = (TH2F*)_file1->Get("hVertexBarrel_xz");
		TH2F* hVertexBarrel_yz = (TH2F*)_file1->Get("hVertexBarrel_yz");
		TH2F* hVertexBarrel_yx = (TH2F*)_file1->Get("hVertexBarrel_yx");
		TH1F* hVertexBarrel_edep = (TH1F*)_file1->Get("hVertexBarrel_edep");
		TH2F* hMcPrim_VertexBarrel_xz = (TH2F*)_file1->Get("hMcPrim_VertexBarrel_xz");
		TH2F* hMcPrim_VertexBarrel_yz = (TH2F*)_file1->Get("hMcPrim_VertexBarrel_yz");
		TH2F* hMcPrim_VertexBarrel_yx = (TH2F*)_file1->Get("hMcPrim_VertexBarrel_yx");
		TH1F* hMcPrim_VertexBarrel_ene = (TH1F*)_file1->Get("hMcPrim_VertexBarrel_ene");

		TH2F* hTrackerEndcap_xz = (TH2F*)_file1->Get("hTrackerEndcap_xz");
		TH2F* hTrackerEndcap_yz = (TH2F*)_file1->Get("hTrackerEndcap_yz");
		TH2F* hTrackerEndcap_yx = (TH2F*)_file1->Get("hTrackerEndcap_yx");
		TH1F* hTrackerEndcap_edep = (TH1F*)_file1->Get("hTrackerEndcap_edep");
		TH2F* hMcPrim_TrackerEndcap_xz = (TH2F*)_file1->Get("hMcPrim_TrackerEndcap_xz");
		TH2F* hMcPrim_TrackerEndcap_yz = (TH2F*)_file1->Get("hMcPrim_TrackerEndcap_yz");
		TH2F* hMcPrim_TrackerEndcap_yx = (TH2F*)_file1->Get("hMcPrim_TrackerEndcap_yx");
		TH1F* hMcPrim_TrackerEndcap_ene = (TH1F*)_file1->Get("hMcPrim_TrackerEndcap_ene");

		hMcPrimAll_xz->Add(hMcPrim_xz);
		hMcPrimAll_yz->Add(hMcPrim_yz);
		hMcPrimAll_yx->Add(hMcPrim_yx);
		hMcPrimAll_ene->Add(hMcPrim_ene);
		hVertexBarrelAll_xz->Add(hVertexBarrel_xz);
		hVertexBarrelAll_yz->Add(hVertexBarrel_yz);
		hVertexBarrelAll_yx->Add(hVertexBarrel_yx);
		hVertexBarrelAll_edep->Add(hVertexBarrel_edep);
		hMcPrim_VertexBarrelAll_xz->Add(hMcPrim_VertexBarrel_xz);
		hMcPrim_VertexBarrelAll_yz->Add(hMcPrim_VertexBarrel_yz);
		hMcPrim_VertexBarrelAll_yx->Add(hMcPrim_VertexBarrel_yx);
		hMcPrim_VertexBarrelAll_ene->Add(hMcPrim_VertexBarrel_ene);
		hTrackerEndcapAll_xz->Add(hTrackerEndcap_xz);
		hTrackerEndcapAll_yz->Add(hTrackerEndcap_yz);
		hTrackerEndcapAll_yx->Add(hTrackerEndcap_yx);
		hTrackerEndcapAll_edep->Add(hTrackerEndcap_edep);
		hMcPrim_TrackerEndcapAll_xz->Add(hMcPrim_TrackerEndcap_xz);
		hMcPrim_TrackerEndcapAll_yz->Add(hMcPrim_TrackerEndcap_yz);
		hMcPrim_TrackerEndcapAll_yx->Add(hMcPrim_TrackerEndcap_yx);
		hMcPrim_TrackerEndcapAll_ene->Add(hMcPrim_TrackerEndcap_ene);

		_file1->Close();
		nFiles++;
	}

	TCanvas* cMcPrimAll_xz = new TCanvas("cMcPrimAll_xz","cMcPrimAll_xz",1800,900);
	cMcPrimAll_xz->cd();
	hMcPrimAll_xz->Draw("colz");
	gPad->SetLogz();
	cMcPrimAll_xz->SaveAs(Form("%s/cMcPrimAll_xz.png",outputdir.c_str()));
	cMcPrimAll_xz->SaveAs(Form("%s/cMcPrimAll_xz.root",outputdir.c_str()));

	TCanvas* cMcPrimAll_yz = new TCanvas("cMcPrimAll_yz","cMcPrimAll_yz",1800,900);
	cMcPrimAll_yz->cd();
	hMcPrimAll_yz->Draw("colz");
	gPad->SetLogz();
	cMcPrimAll_yz->SaveAs(Form("%s/cMcPrimAll_yz.png",outputdir.c_str()));
	cMcPrimAll_yz->SaveAs(Form("%s/cMcPrimAll_yz.root",outputdir.c_str()));

	TCanvas* cMcPrimAll_yx = new TCanvas("cMcPrimAll_yx","cMcPrimAll_yx",900,900);
	cMcPrimAll_yx->cd();
	hMcPrimAll_yx->Draw("colz");
	gPad->SetLogz();
	cMcPrimAll_yx->SaveAs(Form("%s/cMcPrimAll_yx.png",outputdir.c_str()));
	cMcPrimAll_yx->SaveAs(Form("%s/cMcPrimAll_yx.root",outputdir.c_str()));

	TCanvas* cMcPrimAll_ene = new TCanvas("cMcPrimAll_ene","cMcPrimAll_ene",900,900);
	cMcPrimAll_ene->cd();
	hMcPrimAll_ene->Rebin(10);
	hMcPrimAll_ene->Draw("hist");
	hMcPrimAll_ene->GetXaxis()->SetRangeUser(0,40e-6);
	gPad->SetLogy();
	cMcPrimAll_ene->SaveAs(Form("%s/cMcPrimAll_ene.png",outputdir.c_str()));
	cMcPrimAll_ene->SaveAs(Form("%s/cMcPrimAll_ene.root",outputdir.c_str()));

	TCanvas* cVertexBarrel = new TCanvas("cVertexBarrel","cVertexBarrel",1800,900);
	cVertexBarrel->Divide(4,2);

	cVertexBarrel->cd(1);
	hVertexBarrelAll_xz->SetMarkerStyle(21);
	hVertexBarrelAll_xz->SetMarkerSize(0.4);
	hVertexBarrelAll_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hVertexBarrelAll_xz->GetYaxis()->SetRangeUser(-200,200);
	hVertexBarrelAll_xz->Draw("box");

	cVertexBarrel->cd(2);
	hVertexBarrelAll_yz->SetMarkerStyle(21);
	hVertexBarrelAll_yz->SetMarkerSize(0.4);
	hVertexBarrelAll_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hVertexBarrelAll_yz->GetYaxis()->SetRangeUser(-200,200);
	hVertexBarrelAll_yz->Draw("box");

	cVertexBarrel->cd(3);
	hVertexBarrelAll_yx->SetMarkerStyle(21);
	hVertexBarrelAll_yx->SetMarkerSize(0.4);
	hVertexBarrelAll_yx->GetXaxis()->SetRangeUser(-200,200);
	hVertexBarrelAll_yx->GetYaxis()->SetRangeUser(-200,200);
	hVertexBarrelAll_yx->Draw("box");

	//-------------------------------------------------------//
	// draw the detector layers
	// from https://github.com/eic/epic/blob/main/compact/tracking/vertex_barrel.xml
	Double_t VertexBarrel_rmin = 36.0; // [mm]
	Double_t VertexBarrelMod_rmin = VertexBarrel_rmin;
	Double_t VertexBarrelMod1_rmin = VertexBarrelMod_rmin;
	Double_t VertexBarrelLayer_thickness = 2.0; // [mm]
	Double_t VertexBarrelMod2_rmin = VertexBarrelMod_rmin + 12.0; // [mm]
	Double_t VertexBarrelMod3_rmin = 120.0; // [mm] 
	Double_t VertexBarrel_length = 270.0; // [mm]

	Double_t VertexBarrelLayer1_rmin = VertexBarrelMod1_rmin - VertexBarrelLayer_thickness/2.0;
	Double_t VertexBarrelLayer1_rmax = VertexBarrelLayer1_rmin + VertexBarrelLayer_thickness;
	Double_t VertexBarrelLayer2_rmin = VertexBarrelMod2_rmin - VertexBarrelLayer_thickness/2.0;
	Double_t VertexBarrelLayer2_rmax = VertexBarrelLayer2_rmin + VertexBarrelLayer_thickness;
	Double_t VertexBarrelLayer3_rmin = VertexBarrelMod3_rmin - VertexBarrelLayer_thickness/2.0;
	Double_t VertexBarrelLayer3_rmax = VertexBarrelLayer3_rmin + VertexBarrelLayer_thickness;

	TArc *arc_VertexBarrelLayer1_rmin = new TArc(0,0,VertexBarrelLayer1_rmin);
	TArc *arc_VertexBarrelLayer1_rmax = new TArc(0,0,VertexBarrelLayer1_rmax);
	TArc *arc_VertexBarrelLayer2_rmin = new TArc(0,0,VertexBarrelLayer2_rmin);
	TArc *arc_VertexBarrelLayer2_rmax = new TArc(0,0,VertexBarrelLayer2_rmax);
	TArc *arc_VertexBarrelLayer3_rmin = new TArc(0,0,VertexBarrelLayer3_rmin);
	TArc *arc_VertexBarrelLayer3_rmax = new TArc(0,0,VertexBarrelLayer3_rmax);

	arc_VertexBarrelLayer1_rmin->SetLineStyle(1);
	arc_VertexBarrelLayer2_rmin->SetLineStyle(1);
	arc_VertexBarrelLayer3_rmin->SetLineStyle(1);
	arc_VertexBarrelLayer1_rmax->SetLineStyle(1);
	arc_VertexBarrelLayer2_rmax->SetLineStyle(1);
	arc_VertexBarrelLayer3_rmax->SetLineStyle(1);

	arc_VertexBarrelLayer1_rmin->SetFillStyle(0);
	arc_VertexBarrelLayer2_rmin->SetFillStyle(0);
	arc_VertexBarrelLayer3_rmin->SetFillStyle(0);
	arc_VertexBarrelLayer1_rmax->SetFillStyle(0);
	arc_VertexBarrelLayer2_rmax->SetFillStyle(0);
	arc_VertexBarrelLayer3_rmax->SetFillStyle(0);

	arc_VertexBarrelLayer1_rmin->SetLineWidth(1);
	arc_VertexBarrelLayer2_rmin->SetLineWidth(1);
	arc_VertexBarrelLayer3_rmin->SetLineWidth(1);
	arc_VertexBarrelLayer1_rmax->SetLineWidth(1);
	arc_VertexBarrelLayer2_rmax->SetLineWidth(1);
	arc_VertexBarrelLayer3_rmax->SetLineWidth(1);

	arc_VertexBarrelLayer1_rmin->SetLineColorAlpha(kRed,0.2);
	arc_VertexBarrelLayer2_rmin->SetLineColorAlpha(kRed,0.2);
	arc_VertexBarrelLayer3_rmin->SetLineColorAlpha(kRed,0.2);
	arc_VertexBarrelLayer1_rmax->SetLineColorAlpha(kRed,0.8);
	arc_VertexBarrelLayer2_rmax->SetLineColorAlpha(kRed,0.8);
	arc_VertexBarrelLayer3_rmax->SetLineColorAlpha(kRed,0.8);

	arc_VertexBarrelLayer1_rmin->Draw();
	arc_VertexBarrelLayer2_rmin->Draw();
	arc_VertexBarrelLayer3_rmin->Draw();
	arc_VertexBarrelLayer1_rmax->Draw();
	arc_VertexBarrelLayer2_rmax->Draw();
	arc_VertexBarrelLayer3_rmax->Draw();
	//-------------------------------------------------------//

	cVertexBarrel->cd(4);
	hVertexBarrelAll_edep->Rebin(1e3);
	hVertexBarrelAll_edep->GetXaxis()->SetRangeUser(0,40e-6);
	hVertexBarrelAll_edep->Draw("hist");
	gPad->SetLogy();

	cVertexBarrel->cd(5);
	hMcPrim_VertexBarrelAll_xz->SetMarkerStyle(21);
	hMcPrim_VertexBarrelAll_xz->SetMarkerSize(0.4);
	hMcPrim_VertexBarrelAll_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_VertexBarrelAll_xz->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrelAll_xz->Draw("box");

	cVertexBarrel->cd(6);
	hMcPrim_VertexBarrelAll_yz->SetMarkerStyle(21);
	hMcPrim_VertexBarrelAll_yz->SetMarkerSize(0.4);
	hMcPrim_VertexBarrelAll_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_VertexBarrelAll_yz->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrelAll_yz->Draw("box");

	cVertexBarrel->cd(7);
	hMcPrim_VertexBarrelAll_yx->SetMarkerStyle(21);
	hMcPrim_VertexBarrelAll_yx->SetMarkerSize(0.4);
	hMcPrim_VertexBarrelAll_yx->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrelAll_yx->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrelAll_yx->Draw("box");

	cVertexBarrel->cd(8);
	hMcPrim_VertexBarrelAll_ene->Rebin(1e3);
	hMcPrim_VertexBarrelAll_ene->GetXaxis()->SetRangeUser(0,40e-6);
	hMcPrim_VertexBarrelAll_ene->Draw("hist");
	gPad->SetLogy();

	cVertexBarrel->SaveAs(Form("%s/cVertexBarrel.png",outputdir.c_str()));
	cVertexBarrel->SaveAs(Form("%s/cVertexBarrel.root",outputdir.c_str()));

	TCanvas* cTrackerEndcap = new TCanvas("cTrackerEndcap","cTrackerEndcap",1800,900);
	cTrackerEndcap->Divide(4,2);

	cTrackerEndcap->cd(1);
	hTrackerEndcapAll_xz->SetMarkerStyle(21);
	hTrackerEndcapAll_xz->SetMarkerSize(0.4);
	hTrackerEndcapAll_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hTrackerEndcapAll_xz->Draw("box");

	cTrackerEndcap->cd(2);
	hTrackerEndcapAll_yz->SetMarkerStyle(21);
	hTrackerEndcapAll_yz->SetMarkerSize(0.4);
	hTrackerEndcapAll_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hTrackerEndcapAll_yz->Draw("box");

	cTrackerEndcap->cd(3);
	hTrackerEndcapAll_yx->SetMarkerStyle(21);
	hTrackerEndcapAll_yx->SetMarkerSize(0.4);
	hTrackerEndcapAll_yx->Draw("box");

	cTrackerEndcap->cd(4);
	hTrackerEndcapAll_edep->Rebin(1e3);
	hTrackerEndcapAll_edep->GetXaxis()->SetRangeUser(0,40e-6);
	hTrackerEndcapAll_edep->Draw("hist");
	gPad->SetLogy();

	cTrackerEndcap->cd(5);
	hMcPrim_TrackerEndcapAll_xz->SetMarkerStyle(21);
	hMcPrim_TrackerEndcapAll_xz->SetMarkerSize(0.4);
	hMcPrim_TrackerEndcapAll_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_TrackerEndcapAll_xz->Draw("box");

	cTrackerEndcap->cd(6);
	hMcPrim_TrackerEndcapAll_yz->SetMarkerStyle(21);
	hMcPrim_TrackerEndcapAll_yz->SetMarkerSize(0.4);
	hMcPrim_TrackerEndcapAll_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_TrackerEndcapAll_yz->Draw("box");

	cTrackerEndcap->cd(7);
	hMcPrim_TrackerEndcapAll_yx->SetMarkerStyle(21);
	hMcPrim_TrackerEndcapAll_yx->SetMarkerSize(0.4);
	hMcPrim_TrackerEndcapAll_yx->Draw("box");

	cTrackerEndcap->cd(8);
	hMcPrim_TrackerEndcapAll_ene->Rebin(1e3);
	hMcPrim_TrackerEndcapAll_ene->GetXaxis()->SetRangeUser(0,40e-6);
	hMcPrim_TrackerEndcapAll_ene->Draw("hist");
	gPad->SetLogy();

	cTrackerEndcap->SaveAs(Form("%s/cTrackerEndcap.png",outputdir.c_str()));
	cTrackerEndcap->SaveAs(Form("%s/cTrackerEndcap.root",outputdir.c_str()));

	_file0->Close();
	delete cMcPrimAll_xz;
	delete cMcPrimAll_yz;
	delete cMcPrimAll_yx;
	delete cMcPrimAll_ene;
	delete cVertexBarrel;
	delete cTrackerEndcap;

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
