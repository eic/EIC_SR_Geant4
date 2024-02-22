#include "TROOT.h"
#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

vector<Double_t> Pos_X_cm;
vector<Double_t> Pos_Y_cm;
vector<Double_t> Pos_Z_cm;
vector<Double_t> Pos_u;
vector<Double_t> Pos_v;
vector<Double_t> Dir_X;
vector<Double_t> Dir_Y;
vector<Double_t> Dir_Z;
vector<Double_t> Dir_theta_rad;
vector<Double_t> Dir_phi_rad; 
vector<Double_t> LowFluxRatio;
vector<Double_t> Energy_eV;
vector<Double_t> Flux_phs;
vector<Double_t> Power_W;

void readCSVfile(TString fName);
void readSynRadData(TString inputFileName, TH2D* h2_posx_posy, TH1D* h1_ene, TH1D* h1_dirx, TH1D* h1_diry, TH1D* h1_dirz, TH1D* h1_posx, TH1D* h1_posy, TH1D* h1_posz, TH2D* h1_ene_dirx, TH2D* h1_ene_diry, TH2D* h1_ene_dirz);
void getSynRadData(TString dirname, TString filelist, TString outputFileName);

int main()
{
	//-- read the csv file
	getSynRadData("./data_ip","filelist_ip.txt","synrad_data_ip.root"); 
	return 0;
}

void getSynRadData(TString dirname, TString filelist, TString outputFileName)
{
	TFile* file = new TFile(outputFileName.Data(),"recreate");
	file->cd();

	TH1D* h1_ene_sr = new TH1D("h1_ene_sr","SR photon spectrum;E_{#gamma} [eV];Flux [ph/s]",1e6,0,1e6); 
	TH1D* h1_dirx_sr = new TH1D("h1_dirx_sr","SR photon momentum X; Px_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2); 
	TH1D* h1_diry_sr = new TH1D("h1_diry_sr","SR photon momentum Y; Py_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2); 
	TH1D* h1_dirz_sr = new TH1D("h1_dirz_sr","SR photon momentum Z; Pz_{#gamma}/E_{#gamma};Flux [ph/s]",4e6,-2,2); 
	TH1D* h1_posx_sr = new TH1D("h1_posx_sr","SR photon position X; X_{#gamma} [cm];Flux [ph/s]",20000,-10,10);  
	TH1D* h1_posy_sr = new TH1D("h1_posy_sr","SR photon position Y; Y_{#gamma} [cm];Flux [ph/s]",20000,-10,10); 
	TH1D* h1_posz_sr = new TH1D("h1_posz_sr","SR photon position Z; Z_{#gamma} [cm];Flux [ph/s]",20000,-100,100);
	TH2D* h1_ene_dirx_sr = new TH2D("h1_ene_dirx_sr","SR photon energy vs momDir X;Px_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",4e3,-2,2,4e3,0,40e3);
	TH2D* h1_ene_diry_sr = new TH2D("h1_ene_diry_sr","SR photon energy vs momDir Y;Py_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",4e3,-2,2,4e3,0,40e3);
	TH2D* h1_ene_dirz_sr = new TH2D("h1_ene_dirz_sr","SR photon energy vs momDir Z;Pz_{#gamma}/E_{#gamma};E_{#gamma} [eV];Flux [ph/s]",4e3,-2,2,4e3,0,40e3);
	TH2D* h1_posx_posy_sr = new TH2D("h1_posx_posy_sr","SR photon position YX; X_{#gamma} [cm];Y_{#gamma} [cm];Flux [ph/s]",2000,-10,10,2000,-10,10); 

	TString fname;
	ifstream infile(filelist.Data());
	if(infile.is_open())
	{
		while (infile >> fname)
		{
			readSynRadData(Form(	"%s/%s",dirname.Data(),fname.Data()),
						h1_posx_posy_sr,h1_ene_sr,h1_dirx_sr,h1_diry_sr,h1_dirz_sr,h1_posx_sr,
						h1_posy_sr,h1_posz_sr,h1_ene_dirx_sr,h1_ene_diry_sr,h1_ene_dirz_sr);
		}
	}
	infile.close();

	h1_posx_posy_sr->Write();
	h1_ene_sr->Write();
	h1_dirx_sr->Write();
	h1_diry_sr->Write();
	h1_dirz_sr->Write();
	h1_posx_sr->Write();
	h1_posy_sr->Write();
	h1_posz_sr->Write();
	h1_ene_dirx_sr->Write();
	h1_ene_diry_sr->Write();
	h1_ene_dirz_sr->Write();
	file->Close();

	cout<<"Output Synrad+ file name = "<<file->GetName()<<endl;

	return;
}

void readSynRadData(TString inputFileName, 	TH2D* h2_posx_posy, TH1D* h1_ene, TH1D* h1_dirx, TH1D* h1_diry, TH1D* h1_dirz, 
						TH1D* h1_posx, TH1D* h1_posy, TH1D* h1_posz, TH2D* h1_ene_dirx, TH2D* h1_ene_diry, TH2D* h1_ene_dirz)
{
	// clean data vectors
	Pos_X_cm.clear();
	Pos_Y_cm.clear();
	Pos_Z_cm.clear();
	Pos_u.clear();
	Pos_v.clear();
	Dir_X.clear();
	Dir_Y.clear();
	Dir_Z.clear();
	Dir_theta_rad.clear();
	Dir_phi_rad.clear(); 
	LowFluxRatio.clear();
	Energy_eV.clear();
	Flux_phs.clear();
	Power_W.clear();

	// read a CSV file
	readCSVfile(inputFileName);

	for(Int_t i = 0; i < Energy_eV.size(); i++)
	{
		if(Energy_eV.at(i) > 30.0)
		{
			h2_posx_posy->Fill(Pos_X_cm.at(i),Pos_Y_cm.at(i),Flux_phs.at(i));
			h1_ene->Fill(Energy_eV.at(i),Flux_phs.at(i));
			h1_posx->Fill(Pos_X_cm.at(i),Flux_phs.at(i));
			h1_posy->Fill(Pos_Y_cm.at(i),Flux_phs.at(i));
			h1_posz->Fill(Pos_Z_cm.at(i),Flux_phs.at(i));
			h1_dirx->Fill(Dir_X.at(i),Flux_phs.at(i));
			h1_diry->Fill(Dir_Y.at(i),Flux_phs.at(i));
			h1_dirz->Fill(Dir_Z.at(i),Flux_phs.at(i));
			h1_ene_dirx->Fill(Dir_X.at(i),Energy_eV.at(i),Flux_phs.at(i));
			h1_ene_diry->Fill(Dir_Y.at(i),Energy_eV.at(i),Flux_phs.at(i));
			h1_ene_dirz->Fill(Dir_Z.at(i),Energy_eV.at(i),Flux_phs.at(i));
		}
	}

	return;
}

void readCSVfile(TString fName)
{
	cout<<"Input file name: "<<fName<<endl;

	// Create an input filestream
    	ifstream myFile(fName.Data());

    	// Helper vars
    	string line, colname;
    	Double_t val;

    	// Read data, line by line
	getline(myFile, line); // skepp the first title line

    	while(getline(myFile, line))
    	{
        	// Create a stringstream of the current line
        	stringstream ss(line);
        
        	// Keep track of the current column index
        	Int_t colIdx = 0;
        
        	// Extract each value
        	while(ss >> val)
		{
            		// Add the current integer to the 'colIdx' column's values vector
			switch(colIdx)
			{
				case 0:
					Pos_X_cm.push_back(val);
					break;
				case 1:
					Pos_Y_cm.push_back(val);
					break;
				case 2:
					Pos_Z_cm.push_back(val);
					break;
				case 3:
					Pos_u.push_back(val);
					break;
				case 4:
					Pos_v.push_back(val);
					break;
				case 5:
					Dir_X.push_back(val);
					break;
				case 6:
					Dir_Y.push_back(val);
					break;
				case 7:
					Dir_Z.push_back(val);
					break;
				case 8:
					Dir_theta_rad.push_back(val);
					break;
				case 9:
					Dir_phi_rad.push_back(val);
					break;
				case 10:
					LowFluxRatio.push_back(val);
					break;
				case 11:
					Energy_eV.push_back(val);
					break;
				case 12:
					Flux_phs.push_back(val);
					break;
				case 13:
					Power_W.push_back(val);
					break;
			}
            
            		// If the next token is a comma, ignore it and move on
            		if(ss.peek() == ',') ss.ignore();
            
            		// Increment the column index
            		colIdx++;
        	}
    	}

    	// Close file
    	myFile.close();

	return;
}
