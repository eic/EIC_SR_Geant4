/***************************************************************************************************/
/***************************************************************************************************/
/* Plot MC partile distribution and material scan results                                          */
/* One should merge all output PODIO.ROOT files into one using the 'hadd' command; see below       */
/* hadd -f /path/output_eicrecon_podio_all.root /path/output_eicrecon/output_eicrecon_*.podio.root */
/***************************************************************************************************/
/***************************************************************************************************/
#include "TROOT.h"
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

using namespace std;

#define MINX -1000 // [cm]
#define MAXX  1000 // [cm]
#define MINY -1000 // [cm]
#define MAXY  1000 // [cm]
#define MINZ -1000 // [cm]
#define MAXZ  1000 // [cm]

void plotMCparticles(TString fNameIn, TString fNameOut);
void plotMCpHisto(TString fNameIn, TString fNameOut, TString plane);
void plotMaterials(TString fNameIn, TString fNameOut);
void plotMatScanHisto(TString fNameIn, TString fNameOut, TString plane);

void plot()
{	
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	//-----------------------------------------------------------//
	// MC particles (use eic-shell)
	plotMCparticles("/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_3_v6.2/output_eicrecon_7.podio.root","./output/output.root");
//	plotMCparticles(	"/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_podio_all.root",
//				"./output/output_eicrecon_geant4_podio_all_hist.root");
//	plotMCpHisto(		"./output/output_eicrecon_geant4_podio_all_hist.root",
//				"./output/MCP_2D",
//				"xz");
	//-----------------------------------------------------------//
	// Material scan
//	plotMaterials(		"/gpfs/group/belle2/TMP/anatochi/eic/output_matscan/g4mat_scan_*.root",
//				"./output/g4mat_scan_histo.root");
//	plotMatScanHisto(	"./output/g4mat_scan_histo.root",
//				"./output/X0_2D",
//				"xz");
	//-----------------------------------------------------------//

	return;
}

void plotMCpHisto(TString fNameIn, TString fNameOut, TString plane)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	gStyle->SetOptStat(0);

	TFile *_file0 = TFile::Open(fNameIn.Data());
	TH2F* hmc = (TH2F*)_file0->Get(Form("hmc_%s",plane.Data()));

	TCanvas* c1 = new TCanvas("c1","c1",1800,900); 
	c1->cd(); 
        TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
        pad1->SetBottomMargin(0.12);
        pad1->SetTopMargin(0.12);
        pad1->SetRightMargin(0.12);
        pad1->SetLeftMargin(0.12);
        pad1->Draw();
        pad1->cd();

	hmc->GetXaxis()->SetRangeUser(-800,800);
	hmc->GetYaxis()->SetRangeUser(0,600);
	hmc->SetMinimum(1e-1); 
	hmc->GetZaxis()->SetTitle("N_{#gamma} [photons]");
	hmc->Draw("colz"); 

	gPad->SetGrid();
	gPad->SetLogz(); 

	c1->SaveAs(Form("%s_%s.png",fNameOut.Data(),plane.Data()));
	delete c1;
	_file0->Close();

	return;
}

void plotMatScanHisto(TString fNameIn, TString fNameOut, TString plane)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	gStyle->SetOptStat(0);

	TFile *_file0 = TFile::Open(fNameIn.Data());
	TH2F* hx0 = (TH2F*)_file0->Get(Form("hx0_%s",plane.Data()));

	TCanvas* c1 = new TCanvas("c1","c1",1800,900); 
	c1->cd(); 
        TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
        pad1->SetBottomMargin(0.12);
        pad1->SetTopMargin(0.12);
        pad1->SetRightMargin(0.12);
        pad1->SetLeftMargin(0.12);
        pad1->Draw();
        pad1->cd();

	hx0->GetXaxis()->SetRangeUser(-800,800);
	hx0->GetYaxis()->SetRangeUser(0,600);
	hx0->SetMinimum(1e-4); 
	hx0->GetZaxis()->SetTitle("X^{-1}_{0} [cm^{-1}]");
	hx0->Draw("colz"); 

	gPad->SetGrid();
	gPad->SetLogz(); 

	c1->SaveAs(Form("%s_%s.png",fNameOut.Data(),plane.Data()));
	delete c1;
	_file0->Close();

	return;
}

void plotMaterials(TString fNameIn, TString fNameOut)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	TFile* f = new TFile(fNameOut.Data(),"recreate");
	TH2F* hx0_xz = new TH2F("hx0_xz","XZ;Z [cm];X [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXX-MINX)*1,MINX,MAXX);
	TH2F* hx0_yz = new TH2F("hx0_yz","YZ;Z [cm];Y [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXY-MINY)*1,MINY,MAXY);
	TH2F* hx0_yx = new TH2F("hx0_yx","YX;X [cm];Y [cm]",(MAXX-MINX)*1,MINX,MAXX,(MAXY-MINY)*1,MINY,MAXY);
	TH1F* hx0_x = new TH1F("hx0_x","X;X [cm]",(MAXX-MINX)*1,MINX,MAXX);
	TH1F* hx0_y = new TH1F("hx0_y","Y;Y [cm]",(MAXY-MINY)*1,MINY,MAXY);

	//-----------------------------------------------------------//
	// read the root file with material scan results
	TChain* fChain = new TChain("tree");
	fChain->Add(fNameIn.Data());
	fChain->ls();

	Float_t x0, posx, posy, posz;
	fChain->SetBranchAddress("x0", 		&x0);
	fChain->SetBranchAddress("posx", 	&posx);
	fChain->SetBranchAddress("posy", 	&posy);
	fChain->SetBranchAddress("posz", 	&posz);

	Long64_t nEntries = fChain->GetEntries();
	cout<<"nEntries = "<<nEntries<<endl;

	//-----------------------------------------------------------//
	cout<<"Material scan building ..."<<endl;
	Int_t binx, biny;
	Float_t binc;
	Long64_t jEntry = 0;
	while(jEntry < nEntries)
	{
		fChain->GetEntry(jEntry);

                if(jEntry%100000 == 0)
                {
                        printf("\r[INFO] Progress: %.3f [%%]",100.0*jEntry/nEntries);
                        fflush(stdout);
                }
                jEntry++;

		// XZ
		binx = hx0_xz->GetXaxis()->FindBin(posz);
		biny = hx0_xz->GetYaxis()->FindBin(posx);
		binc = hx0_xz->GetBinContent(binx,biny);
		hx0_xz->SetBinContent(binx,biny,(binc > 0) ? 0.5*(binc+1./x0) : (1./x0));

		// YZ
		binx = hx0_yz->GetXaxis()->FindBin(posz);
		biny = hx0_yz->GetYaxis()->FindBin(posy);
		binc = hx0_yz->GetBinContent(binx,biny);
		hx0_yz->SetBinContent(binx,biny,(binc > 0) ? 0.5*(binc+1./x0) : (1./x0));

		// YX
		binx = hx0_yx->GetXaxis()->FindBin(posx);
		biny = hx0_yx->GetYaxis()->FindBin(posy);
		binc = hx0_yx->GetBinContent(binx,biny);
		hx0_yx->SetBinContent(binx,biny,(binc > 0) ? 0.5*(binc+1./x0) : (1./x0));

		// X
		hx0_x->Fill(posx,1./x0);

		// Y
		hx0_y->Fill(posy,1./x0);
	}
        printf("\r[INFO] Progress: %.3f [%%]\n",100.0*jEntry/nEntries);
	delete fChain->GetCurrentFile();
	//-----------------------------------------------------------//

	cout<<"Writing to a file ..."<<endl;
	if(!f)
	{
		cout<<"[ERROR] Cannot recreate the output file: "<<fNameOut<<endl;
		return;
	}
	f->cd();
	hx0_xz->Write();
	hx0_yz->Write();
	hx0_yx->Write();
	hx0_x->Write();
	hx0_y->Write();
	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}

void plotMCparticles(TString fNameIn, TString fNameOut)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	TFile* f = new TFile(fNameOut.Data(),"recreate");
	TH2F* hmc_xz = new TH2F("hmc_xz","MC XZ;Z [cm];X [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXX-MINX)*100,MINX,MAXX);
	TH2F* hmc_yz = new TH2F("hmc_yz","MC YZ;Z [cm];Y [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXY-MINY)*100,MINY,MAXY);
	TH2F* hmc_yx = new TH2F("hmc_yx","MC YX;X [cm];Y [cm]",(MAXX-MINX)*10,MINX,MAXX,(MAXY-MINY)*10,MINY,MAXY);

	//-----------------------------------------------------------//
	TFile* file = new TFile(fNameIn.Data());
	TTreeReader myReader("events", file);
	TTreeReaderArray<Int_t> 	pdg_mc(myReader,"MCParticles.PDG");
	TTreeReaderArray<Int_t>		generatorStatus_mc(myReader,"MCParticles.generatorStatus");
	TTreeReaderArray<Int_t>		simulatorStatus_mc(myReader,"MCParticles.simulatorStatus");
	TTreeReaderArray<Float_t>	charge_mc(myReader,"MCParticles.charge");
	TTreeReaderArray<Float_t>	time_mc(myReader,"MCParticles.time");
	TTreeReaderArray<Double_t>	mass_mc(myReader,"MCParticles.mass");
	TTreeReaderArray<Double_t> 	vx_mc(myReader, "MCParticles.vertex.x");
	TTreeReaderArray<Double_t> 	vy_mc(myReader, "MCParticles.vertex.y");
	TTreeReaderArray<Double_t> 	vz_mc(myReader, "MCParticles.vertex.z");

	TTreeReaderArray<Float_t>      	edep_VertexBarrel(myReader, "VertexBarrelHits.EDep");
	TTreeReaderArray<Double_t>      vx_VertexBarrel(myReader, "VertexBarrelHits.position.x");
	TTreeReaderArray<Double_t>      vy_VertexBarrel(myReader, "VertexBarrelHits.position.y");
	TTreeReaderArray<Double_t>      vz_VertexBarrel(myReader, "VertexBarrelHits.position.z");
	TTreeReaderArray<Int_t>      	index_mc_VertexBarrel(myReader, "_VertexBarrelHits_MCParticle.index");

	TTreeReaderArray<UInt_t> 	parents_begin_mc(myReader, "MCParticles.parents_begin");
	TTreeReaderArray<UInt_t> 	parents_end_mc(myReader, "MCParticles.parents_end");

	//-----------------------------------------------------------//
	cout<<"MC particles calculation ..."<<endl;
	while(myReader.Next())
	{
		Int_t nHits_VertexBarrel = edep_VertexBarrel.GetSize();
		for(Int_t iHit = 0; iHit < nHits_VertexBarrel; iHit++)
		{
			printf("%d : \t eDep = %.3e [keV] | (x;y;z) = (%.3e;%.3e;%.3e) | index_mc_VertexBarrel = %d\n",iHit,edep_VertexBarrel[iHit]*1e6,vx_VertexBarrel[iHit],vy_VertexBarrel[iHit],vz_VertexBarrel[iHit],index_mc_VertexBarrel[iHit]);
			cout<<"parents_begin_mc["<<index_mc_VertexBarrel[iHit]<<"] = "<<parents_begin_mc[index_mc_VertexBarrel[iHit]]<<endl;
			cout<<"parents_end_mc["<<index_mc_VertexBarrel[iHit]<<"] = "<<parents_end_mc[index_mc_VertexBarrel[iHit]]<<endl;
		}
/*
		Int_t nPart = pdg_mc.GetSize();
		for (Int_t imcPart = 0; imcPart < nPart; imcPart++)
		{
			if(simulatorStatus_mc[imcPart])
			{
				hmc_xz->Fill(vz_mc[imcPart]/1e1,vx_mc[imcPart]/1e1); // mm -> cm
				hmc_yz->Fill(vz_mc[imcPart]/1e1,vy_mc[imcPart]/1e1); // mm -> cm
				hmc_yx->Fill(vx_mc[imcPart]/1e1,vy_mc[imcPart]/1e1); // mm -> cm
			}
		}
*/
	}
	file->Close();
	//-----------------------------------------------------------//

	cout<<"Writing to a file ..."<<endl;
	if(!f)
	{
		cout<<"[ERROR] Cannot recreate the output file: "<<fNameOut<<endl;
		return;
	}
	f->cd();
	hmc_xz->Write();
	hmc_yz->Write();
	hmc_yx->Write();
	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}
