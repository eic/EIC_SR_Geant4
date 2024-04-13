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

void getAllVerticesPerFile(	
	const string inputfile = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask4_2_farBWD_newMaterialsAndCoating/output_eicrecon_1.podio.root", 
	const string outputfile = "/gpfs/group/belle2/TMP/anatochi/eic/highstat_output_eicrecon_v6.2_srmask4_2_farBWD_newMaterialsAndCoating/output_eicrecon_1.vertex.root")
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;

	cout<<"--> inputfile = "<<inputfile<<endl;
	cout<<"--> outputfile = "<<outputfile<<endl;


	//-----------------------------------------------------------//
	TFile* file = new TFile(inputfile.c_str());
	if(!file) return;

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
	TFile* f = new TFile(outputfile.c_str(),"recreate");

	TH2F* hMcPrim_xz = new TH2F("hMcPrim_xz","MCparticle prim. XZ;Z [mm];X [mm]",5000,-40000,10000,1000,-800,200);
	TH2F* hMcPrim_yz = new TH2F("hMcPrim_yz","MCparticle prim. YZ;Z [mm];Y [mm]",5000,-40000,10000,1000,-800,200);
	TH2F* hMcPrim_yx = new TH2F("hMcPrim_yx","MCparticle prim. YX;X [mm];Y [mm]",1000,-800,200,1000,-800,200);
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
