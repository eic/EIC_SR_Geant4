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
void readDataFromFiles(const string outputfile);
void drawHistograms(const string filename);

vector<string> _fileNames;

void plotVertices(	const string directory = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_3_v6.2_new/", 
			const string pattern = ".podio", 
			const string extension = ".root", 
			const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_3_v6.2_new_plotVertices.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	//-- get data from eic-shell output data
//	getFileNames(directory, pattern, extension);
//	readDataFromFiles(outputfile);

	//-- draw histograms from above
	drawHistograms(outputfile);

	return;
}

void drawHistograms(const string filename)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	gStyle->SetOptStat(0);
	TFile *_file0 = TFile::Open(filename.c_str());

	TH2F* hMcPrim_xz = (TH2F*)_file0->Get("hMcPrim_xz");
	TH2F* hMcPrim_yz = (TH2F*)_file0->Get("hMcPrim_yz");
	TH2F* hMcPrim_yx = (TH2F*)_file0->Get("hMcPrim_yx");
	TH1F* hMcPrim_ene = (TH1F*)_file0->Get("hMcPrim_ene");

	TH2F* hVertexBarrel_xz = (TH2F*)_file0->Get("hVertexBarrel_xz");
	TH2F* hVertexBarrel_yz = (TH2F*)_file0->Get("hVertexBarrel_yz");
	TH2F* hVertexBarrel_yx = (TH2F*)_file0->Get("hVertexBarrel_yx");
	TH1F* hVertexBarrel_edep = (TH1F*)_file0->Get("hVertexBarrel_edep");
	TH2F* hMcPrim_VertexBarrel_xz = (TH2F*)_file0->Get("hMcPrim_VertexBarrel_xz");
	TH2F* hMcPrim_VertexBarrel_yz = (TH2F*)_file0->Get("hMcPrim_VertexBarrel_yz");
	TH2F* hMcPrim_VertexBarrel_yx = (TH2F*)_file0->Get("hMcPrim_VertexBarrel_yx");
	TH1F* hMcPrim_VertexBarrel_ene = (TH1F*)_file0->Get("hMcPrim_VertexBarrel_ene");

	TH2F* hTrackerEndcap_xz = (TH2F*)_file0->Get("hTrackerEndcap_xz");
	TH2F* hTrackerEndcap_yz = (TH2F*)_file0->Get("hTrackerEndcap_yz");
	TH2F* hTrackerEndcap_yx = (TH2F*)_file0->Get("hTrackerEndcap_yx");
	TH1F* hTrackerEndcap_edep = (TH1F*)_file0->Get("hTrackerEndcap_edep");
	TH2F* hMcPrim_TrackerEndcap_xz = (TH2F*)_file0->Get("hMcPrim_TrackerEndcap_xz");
	TH2F* hMcPrim_TrackerEndcap_yz = (TH2F*)_file0->Get("hMcPrim_TrackerEndcap_yz");
	TH2F* hMcPrim_TrackerEndcap_yx = (TH2F*)_file0->Get("hMcPrim_TrackerEndcap_yx");
	TH1F* hMcPrim_TrackerEndcap_ene = (TH1F*)_file0->Get("hMcPrim_TrackerEndcap_ene");

	TCanvas* cMcPrim_xz = new TCanvas("cMcPrim_xz","cMcPrim_xz",1800,900);
	cMcPrim_xz->cd();
	hMcPrim_xz->Draw("colz");
	gPad->SetLogz();
	cMcPrim_xz->SaveAs("./pics/cMcPrim_xz.png");
	cMcPrim_xz->SaveAs("./pics/cMcPrim_xz.root");

	TCanvas* cMcPrim_yz = new TCanvas("cMcPrim_yz","cMcPrim_yz",1800,900);
	cMcPrim_yz->cd();
	hMcPrim_yz->Draw("colz");
	gPad->SetLogz();
	cMcPrim_yz->SaveAs("./pics/cMcPrim_yz.png");
	cMcPrim_yz->SaveAs("./pics/cMcPrim_yz.root");

	TCanvas* cMcPrim_yx = new TCanvas("cMcPrim_yx","cMcPrim_yx",900,900);
	cMcPrim_yx->cd();
	hMcPrim_yx->Draw("colz");
	gPad->SetLogz();
	cMcPrim_yx->SaveAs("./pics/cMcPrim_yx.png");
	cMcPrim_yx->SaveAs("./pics/cMcPrim_yx.root");

	TCanvas* cMcPrim_ene = new TCanvas("cMcPrim_ene","cMcPrim_ene",900,900);
	cMcPrim_ene->cd();
	hMcPrim_ene->Rebin(10);
	hMcPrim_ene->Draw("hist");
	hMcPrim_ene->GetXaxis()->SetRangeUser(0,40e-6);
	gPad->SetLogy();
	cMcPrim_ene->SaveAs("./pics/cMcPrim_ene.png");
	cMcPrim_ene->SaveAs("./pics/cMcPrim_ene.root");

	TCanvas* cVertexBarrel = new TCanvas("cVertexBarrel","cVertexBarrel",1800,900);
	cVertexBarrel->Divide(4,2);

	cVertexBarrel->cd(1);
	hVertexBarrel_xz->SetMarkerStyle(21);
	hVertexBarrel_xz->SetMarkerSize(0.4);
	hVertexBarrel_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hVertexBarrel_xz->GetYaxis()->SetRangeUser(-200,200);
	hVertexBarrel_xz->Draw("box");

	cVertexBarrel->cd(2);
	hVertexBarrel_yz->SetMarkerStyle(21);
	hVertexBarrel_yz->SetMarkerSize(0.4);
	hVertexBarrel_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hVertexBarrel_yz->GetYaxis()->SetRangeUser(-200,200);
	hVertexBarrel_yz->Draw("box");

	cVertexBarrel->cd(3);
	hVertexBarrel_yx->SetMarkerStyle(21);
	hVertexBarrel_yx->SetMarkerSize(0.4);
	hVertexBarrel_yx->GetXaxis()->SetRangeUser(-200,200);
	hVertexBarrel_yx->GetYaxis()->SetRangeUser(-200,200);
	hVertexBarrel_yx->Draw("box");

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
	hVertexBarrel_edep->Rebin(1e3);
	hVertexBarrel_edep->GetXaxis()->SetRangeUser(0,40e-6);
	hVertexBarrel_edep->Draw("hist");
	gPad->SetLogy();

	cVertexBarrel->cd(5);
	hMcPrim_VertexBarrel_xz->SetMarkerStyle(21);
	hMcPrim_VertexBarrel_xz->SetMarkerSize(0.4);
	hMcPrim_VertexBarrel_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_VertexBarrel_xz->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrel_xz->Draw("box");

	cVertexBarrel->cd(6);
	hMcPrim_VertexBarrel_yz->SetMarkerStyle(21);
	hMcPrim_VertexBarrel_yz->SetMarkerSize(0.4);
	hMcPrim_VertexBarrel_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_VertexBarrel_yz->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrel_yz->Draw("box");

	cVertexBarrel->cd(7);
	hMcPrim_VertexBarrel_yx->SetMarkerStyle(21);
	hMcPrim_VertexBarrel_yx->SetMarkerSize(0.4);
	hMcPrim_VertexBarrel_yx->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrel_yx->GetYaxis()->SetRangeUser(-200,200);
	hMcPrim_VertexBarrel_yx->Draw("box");

	cVertexBarrel->cd(8);
	hMcPrim_VertexBarrel_ene->Rebin(1e3);
	hMcPrim_VertexBarrel_ene->GetXaxis()->SetRangeUser(0,40e-6);
	hMcPrim_VertexBarrel_ene->Draw("hist");
	gPad->SetLogy();

	cVertexBarrel->SaveAs("./pics/cVertexBarrel.png");
	cVertexBarrel->SaveAs("./pics/cVertexBarrel.root");

	TCanvas* cTrackerEndcap = new TCanvas("cTrackerEndcap","cTrackerEndcap",1800,900);
	cTrackerEndcap->Divide(4,2);

	cTrackerEndcap->cd(1);
	hTrackerEndcap_xz->SetMarkerStyle(21);
	hTrackerEndcap_xz->SetMarkerSize(0.4);
	hTrackerEndcap_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hTrackerEndcap_xz->Draw("box");

	cTrackerEndcap->cd(2);
	hTrackerEndcap_yz->SetMarkerStyle(21);
	hTrackerEndcap_yz->SetMarkerSize(0.4);
	hTrackerEndcap_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hTrackerEndcap_yz->Draw("box");

	cTrackerEndcap->cd(3);
	hTrackerEndcap_yx->SetMarkerStyle(21);
	hTrackerEndcap_yx->SetMarkerSize(0.4);
	hTrackerEndcap_yx->Draw("box");

	cTrackerEndcap->cd(4);
	hTrackerEndcap_edep->Rebin(1e3);
	hTrackerEndcap_edep->GetXaxis()->SetRangeUser(0,40e-6);
	hTrackerEndcap_edep->Draw("hist");
	gPad->SetLogy();

	cTrackerEndcap->cd(5);
	hMcPrim_TrackerEndcap_xz->SetMarkerStyle(21);
	hMcPrim_TrackerEndcap_xz->SetMarkerSize(0.4);
	hMcPrim_TrackerEndcap_xz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_TrackerEndcap_xz->Draw("box");

	cTrackerEndcap->cd(6);
	hMcPrim_TrackerEndcap_yz->SetMarkerStyle(21);
	hMcPrim_TrackerEndcap_yz->SetMarkerSize(0.4);
	hMcPrim_TrackerEndcap_yz->GetXaxis()->SetRangeUser(-2000,2000);
	hMcPrim_TrackerEndcap_yz->Draw("box");

	cTrackerEndcap->cd(7);
	hMcPrim_TrackerEndcap_yx->SetMarkerStyle(21);
	hMcPrim_TrackerEndcap_yx->SetMarkerSize(0.4);
	hMcPrim_TrackerEndcap_yx->Draw("box");

	cTrackerEndcap->cd(8);
	hMcPrim_TrackerEndcap_ene->Rebin(1e3);
	hMcPrim_TrackerEndcap_ene->GetXaxis()->SetRangeUser(0,40e-6);
	hMcPrim_TrackerEndcap_ene->Draw("hist");
	gPad->SetLogy();

	cTrackerEndcap->SaveAs("./pics/cTrackerEndcap.png");
	cTrackerEndcap->SaveAs("./pics/cTrackerEndcap.root");

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

	TH2F* hMcPrim_xz = new TH2F("hMcPrim_xz","MCparticle prim. XZ;Z [mm];X [mm]",2000,-10000,10000,1000,-500,500);
	TH2F* hMcPrim_yz = new TH2F("hMcPrim_yz","MCparticle prim. YZ;Z [mm];Y [mm]",2000,-10000,10000,1000,-500,500);
	TH2F* hMcPrim_yx = new TH2F("hMcPrim_yx","MCparticle prim. YX;X [mm];Y [mm]",1000,-500,500,1000,-500,500);
	TH1F* hMcPrim_ene = new TH1F("hMcPrim_ene","MCparticle prim. E;E [GeV]",1e6,0,1e-3);

	TH2F* hVertexBarrel_xz 
		= new TH2F("hVertexBarrel_xz","VertexBarrel XZ;Z [mm];X [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hVertexBarrel_yz 
		= new TH2F("hVertexBarrel_yz","VertexBarrel YZ;Z [mm];Y [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hVertexBarrel_yx 
		= new TH2F("hVertexBarrel_yx","VertexBarrel YX;X [mm];Y [mm]",1000,-500,500,1000,-500,500);
	TH1F* hVertexBarrel_edep 
		= new TH1F("hVertexBarrel_edep","VertexBarrel E_{dep}; E_{dep} [GeV]",1e6,0,1e-3);
	TH2F* hMcPrim_VertexBarrel_xz 
		= new TH2F("hMcPrim_VertexBarrel_xz","VertexBarrel MCp XZ;Z [mm];X [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hMcPrim_VertexBarrel_yz 
		= new TH2F("hMcPrim_VertexBarrel_yz","VertexBarrel MCp YZ;Z [mm];Y [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hMcPrim_VertexBarrel_yx 
		= new TH2F("hMcPrim_VertexBarrel_yx","VertexBarrel MCp YX;X [mm];Y [mm]",1000,-500,500,1000,-500,500);
	TH1F* hMcPrim_VertexBarrel_ene 
		= new TH1F("hMcPrim_VertexBarrel_ene","VertexBarrel MCp E; E [GeV]",1e6,0,1e-3);

	TH2F* hTrackerEndcap_xz 
		= new TH2F("hTrackerEndcap_xz","TrackerEndcap XZ;Z [mm];X [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hTrackerEndcap_yz 
		= new TH2F("hTrackerEndcap_yz","TrackerEndcap YZ;Z [mm];Y [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hTrackerEndcap_yx 
		= new TH2F("hTrackerEndcap_yx","TrackerEndcap YX;X [mm];Y [mm]",1000,-500,500,1000,-500,500);
	TH1F* hTrackerEndcap_edep 
		= new TH1F("hTrackerEndcap_edep","TrackerEndcap E_{dep}; E_{dep} [GeV]",1e6,0,1e-3);
	TH2F* hMcPrim_TrackerEndcap_xz 
		= new TH2F("hMcPrim_TrackerEndcap_xz","TrackerEndcap MCp XZ;Z [mm];X [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hMcPrim_TrackerEndcap_yz 
		= new TH2F("hMcPrim_TrackerEndcap_yz","TrackerEndcap MCp YZ;Z [mm];Y [mm]",200,-10000,10000,1000,-500,500);
	TH2F* hMcPrim_TrackerEndcap_yx 
		= new TH2F("hMcPrim_TrackerEndcap_yx","TrackerEndcap MCp YX;X [mm];Y [mm]",1000,-500,500,1000,-500,500);
	TH1F* hMcPrim_TrackerEndcap_ene 
		= new TH1F("hMcPrim_TrackerEndcap_ene","TrackerEndcap MCp E; E [GeV]",1e6,0,1e-3);

	//-----------------------------------------------------------//
	// loop over files
	for(Int_t iFile = 0; iFile < _fileNames.size(); iFile++)
	{
		cout<<iFile+1<<"/"<<_fileNames.size()<<" : "<<_fileNames.at(iFile)<<endl;

		TFile* file = new TFile(_fileNames.at(iFile).c_str());
		TTreeReader myReader("events", file);
		TTreeReaderArray<Int_t> 	pdg_mc(myReader,"MCParticles.PDG");
		TTreeReaderArray<Int_t>		generatorStatus_mc(myReader,"MCParticles.generatorStatus");
		TTreeReaderArray<Int_t>		simulatorStatus_mc(myReader,"MCParticles.simulatorStatus");
		TTreeReaderArray<Double_t>	mass_mc(myReader,"MCParticles.mass");
		TTreeReaderArray<Double_t> 	vx_mc(myReader, "MCParticles.vertex.x");
		TTreeReaderArray<Double_t> 	vy_mc(myReader, "MCParticles.vertex.y");
		TTreeReaderArray<Double_t> 	vz_mc(myReader, "MCParticles.vertex.z");
		TTreeReaderArray<Float_t>	px_mc(myReader, "MCParticles.momentum.x");
		TTreeReaderArray<Float_t>	py_mc(myReader, "MCParticles.momentum.y");
		TTreeReaderArray<Float_t>	pz_mc(myReader, "MCParticles.momentum.z");
		TTreeReaderArray<UInt_t> 	parents_begin_mc(myReader, "MCParticles.parents_begin");
		TTreeReaderArray<UInt_t> 	parents_end_mc(myReader, "MCParticles.parents_end");
		TTreeReaderArray<Int_t>         parents_index_mc(myReader, "_MCParticles_parents.index");

		TTreeReaderArray<Float_t>      	edep_VertexBarrel(myReader, "VertexBarrelHits.EDep");
		TTreeReaderArray<Double_t>      vx_VertexBarrel(myReader, "VertexBarrelHits.position.x");
		TTreeReaderArray<Double_t>      vy_VertexBarrel(myReader, "VertexBarrelHits.position.y");
		TTreeReaderArray<Double_t>      vz_VertexBarrel(myReader, "VertexBarrelHits.position.z");
		TTreeReaderArray<Int_t>		index_mc_VertexBarrel(myReader, "_VertexBarrelHits_MCParticle.index");

		TTreeReaderArray<Float_t>      	edep_TrackerEndcap(myReader, "TrackerEndcapHits.EDep");
		TTreeReaderArray<Double_t>      vx_TrackerEndcap(myReader, "TrackerEndcapHits.position.x");
		TTreeReaderArray<Double_t>      vy_TrackerEndcap(myReader, "TrackerEndcapHits.position.y");
		TTreeReaderArray<Double_t>      vz_TrackerEndcap(myReader, "TrackerEndcapHits.position.z");
		TTreeReaderArray<Int_t>		index_mc_TrackerEndcap(myReader, "_TrackerEndcapHits_MCParticle.index");

		//-----------------------------------------------------------//
		// loop over events
		while(myReader.Next())
		{
			//-- VertexBarrel
			if(edep_VertexBarrel.GetSize() > 0)
			{
				// get VertexBarrel hits
				for(Int_t iHit = 0; iHit < edep_VertexBarrel.GetSize(); iHit++)
				{
					// hit's info
					hVertexBarrel_xz->Fill(vz_VertexBarrel[iHit],vx_VertexBarrel[iHit],1);
					hVertexBarrel_yz->Fill(vz_VertexBarrel[iHit],vy_VertexBarrel[iHit],1);
					hVertexBarrel_yx->Fill(vx_VertexBarrel[iHit],vy_VertexBarrel[iHit],1);
					hVertexBarrel_edep->Fill(edep_VertexBarrel[iHit],1);

					// get MCparticle info
					Int_t index = index_mc_VertexBarrel[iHit];
					Bool_t primary = false;
					if(simulatorStatus_mc[index] > 0 && generatorStatus_mc[index] == 1)
					{primary = true;} // primary
					else // secondary
					{
						// loop over parents
						for(Int_t ind = parents_begin_mc[index]; ind <= parents_end_mc[index]; ind++)
						{
							if(simulatorStatus_mc[parents_index_mc[ind]] > 0 && 
							generatorStatus_mc[parents_index_mc[ind]] == 1)
							{index = parents_index_mc[ind]; primary = true;} // primary
						}
					}
					// fill primary MCparticle (gamma) info
					if(primary && pdg_mc[index] == 22)
					{
						hMcPrim_VertexBarrel_xz->Fill(vz_mc[index],vx_mc[index],1);
						hMcPrim_VertexBarrel_yz->Fill(vz_mc[index],vy_mc[index],1);
						hMcPrim_VertexBarrel_yx->Fill(vx_mc[index],vy_mc[index],1);
						hMcPrim_VertexBarrel_ene->Fill(TMath::Sqrt(	TMath::Power(px_mc[index],2) +
												TMath::Power(py_mc[index],2) +
												TMath::Power(pz_mc[index],2) +
												TMath::Power(mass_mc[index],2)),1);
					}
					else // skip
					{
						cout<<endl<<"[WARNING] : Cannot find the primary vertex. Skipping MCparticle info."<<endl<<endl;
					}
				}
			}
	
			//-- TrackerEndcap
			if(edep_TrackerEndcap.GetSize() > 0)
			{
				// get TrackerEndcap hits
				for(Int_t iHit = 0; iHit < edep_TrackerEndcap.GetSize(); iHit++)
				{
					// hit's info
					hTrackerEndcap_xz->Fill(vz_TrackerEndcap[iHit],vx_TrackerEndcap[iHit],1);
					hTrackerEndcap_yz->Fill(vz_TrackerEndcap[iHit],vy_TrackerEndcap[iHit],1);
					hTrackerEndcap_yx->Fill(vx_TrackerEndcap[iHit],vy_TrackerEndcap[iHit],1);
					hTrackerEndcap_edep->Fill(edep_TrackerEndcap[iHit],1);

					// get MCparticle info
					Int_t index = index_mc_TrackerEndcap[iHit];
					Bool_t primary = false;
					if(simulatorStatus_mc[index] > 0 && generatorStatus_mc[index] == 1)
					{primary = true;} // primary
					else // secondary
					{
						// loop over parents
						for(Int_t ind = parents_begin_mc[index]; ind <= parents_end_mc[index]; ind++)
						{
							if(	simulatorStatus_mc[parents_index_mc[ind]] > 0 && 
								generatorStatus_mc[parents_index_mc[ind]] == 1)
							{index = parents_index_mc[ind]; primary = true;} // primary
						}
					}
					// fill primary MCparticle (gamma) info
					if(primary && pdg_mc[index] == 22)
					{
						hMcPrim_TrackerEndcap_xz->Fill(vz_mc[index],vx_mc[index],1);
						hMcPrim_TrackerEndcap_yz->Fill(vz_mc[index],vy_mc[index],1);
						hMcPrim_TrackerEndcap_yx->Fill(vx_mc[index],vy_mc[index],1);
						hMcPrim_TrackerEndcap_ene->Fill(TMath::Sqrt(	TMath::Power(px_mc[index],2) +
												TMath::Power(py_mc[index],2) +
												TMath::Power(pz_mc[index],2) +
												TMath::Power(mass_mc[index],2)),1);
					}
					else // skip
					{
						cout<<endl<<"[WARNING] : Cannot find the primary vertex.";
						cout<<" Skipping MCparticle info."<<endl<<endl;
					}
				}
			}

			//-- Primary MCparticles
			for (Int_t imcPart = 0; imcPart < pdg_mc.GetSize(); imcPart++)
			{
				if(simulatorStatus_mc[imcPart] > 0 && generatorStatus_mc[imcPart] == 1)
				{
					hMcPrim_xz->Fill(vz_mc[imcPart],vx_mc[imcPart],1);
					hMcPrim_yz->Fill(vz_mc[imcPart],vy_mc[imcPart],1);
					hMcPrim_yx->Fill(vx_mc[imcPart],vy_mc[imcPart],1);
					hMcPrim_ene->Fill(TMath::Sqrt(	TMath::Power(px_mc[imcPart],2) + 
									TMath::Power(py_mc[imcPart],2) + 
									TMath::Power(pz_mc[imcPart],2) +
									TMath::Power(mass_mc[imcPart],2)),1);
				}
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

	hVertexBarrel_xz->Write();
	hVertexBarrel_yz->Write();
	hVertexBarrel_yx->Write();
	hVertexBarrel_edep->Write();
	hMcPrim_VertexBarrel_xz->Write();
	hMcPrim_VertexBarrel_yz->Write();
	hMcPrim_VertexBarrel_yx->Write();
	hMcPrim_VertexBarrel_ene->Write();

	hTrackerEndcap_xz->Write();
	hTrackerEndcap_yz->Write();
	hTrackerEndcap_yx->Write();
	hTrackerEndcap_edep->Write();
	hMcPrim_TrackerEndcap_xz->Write();
	hMcPrim_TrackerEndcap_yz->Write();
	hMcPrim_TrackerEndcap_yx->Write();
	hMcPrim_TrackerEndcap_ene->Write();

	hMcPrim_xz->Write();
	hMcPrim_yz->Write();
	hMcPrim_yx->Write();
	hMcPrim_ene->Write();

	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}
