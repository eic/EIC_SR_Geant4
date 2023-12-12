#include "TROOT.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

const Bool_t _print = kFALSE;

void plotData(TString fNameIn, TString fNameOut);

int main()
{
	cout<<endl<<__FUNCTION__<<endl<<endl;

	plotData("../../geant4_sim/geometry/MARCO_v.6.4.1.1.3_1.7T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt","1.7T.png");
	plotData("../../geant4_sim/geometry/MARCO_v.6.4.1.1.3_2T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt","2.0T.png");

	return 0;
}

void plotData(TString fNameIn, TString fNameOut)
{
	cout<<endl<<__FUNCTION__<<endl<<endl;

	// open the file
	ifstream inputFile(fNameIn.Data());

	// check if the file is open
	if (!inputFile.is_open())
	{
		cerr<<"Error opening the file."<<endl;
		return;
	}
	else
	{
		cout<<"The file '"<<fNameIn<<"' is open"<<endl;
	}

	// variables to store data from each column
	Double_t r, z, Br, Bz;
	Int_t line = 0;

	// 2D histograms for field maps
	TH2D* hBr = new TH2D("hBr",";Z [cm];R [cm];Br [T]",500,-500,500,200,0,400);
	TH2D* hBz = new TH2D("hBz",";Z [cm];R [cm];Bz [T]",500,-500,500,200,0,400);
	TH2D* hB = new TH2D("hB",";Z [cm];R [cm];B [T]",500,-500,500,200,0,400);

	hBr->GetYaxis()->SetTitleOffset(1.2);
	hBz->GetYaxis()->SetTitleOffset(1.2);
	hB->GetYaxis()->SetTitleOffset(1.2);

	cout<<"binxWidth: Br_X = "<<hBr->GetXaxis()->GetBinWidth(1)<<" (cm) | Br_Y = "<< hBr->GetYaxis()->GetBinWidth(1)<<" (cm)"<<endl;
	cout<<"binxWidth: Bz_X = "<<hBz->GetXaxis()->GetBinWidth(1)<<" (cm) | Bz_Y = "<< hBz->GetYaxis()->GetBinWidth(1)<<" (cm)"<<endl;
	cout<<"binxWidth: B_X  = "<<hB->GetXaxis()->GetBinWidth(1)<<" (cm) | B_Y  = "<< hB->GetYaxis()->GetBinWidth(1)<<" (cm)"<<endl;

	// read the file line by line
	if(_print) cout<<setw(10)<<"#"<<" "<<setw(10)<<"R (cm)"<<" "<<setw(10)<<"Z (cm)"<<" "<<setw(10)<<"Br (T)"<<" "<<setw(10)<<"Bz (T)"<<endl;
	while (inputFile >> r >> z >> Br >> Bz )
	{
		if(_print) cout<<setw(10)<<line<<" "<<setw(10)<<r<<" "<<setw(10)<<z<<" "<<setw(10)<<Br<<" "<<setw(10)<<Bz<<endl;
		hBr->SetBinContent(	hBr->GetXaxis()->FindBin(z),
					hBr->GetYaxis()->FindBin(r),
					Br);
		hBz->SetBinContent(	hBz->GetXaxis()->FindBin(z),
					hBz->GetYaxis()->FindBin(r),
					Bz);
		hB->SetBinContent(	hB->GetXaxis()->FindBin(z),
					hB->GetYaxis()->FindBin(r),
					TMath::Sqrt(Br*Br + Bz*Bz));
		line++;
	}
	if(_print) cout<<setw(10)<<"#"<<" "<<setw(10)<<"R (cm)"<<" "<<setw(10)<<"Z (cm)"<<" "<<setw(10)<<"Br (T)"<<" "<<setw(10)<<"Bz (T)"<<endl;

	// close the file
	inputFile.close();

	// draw histograms
	gStyle->SetOptStat(0);

	TCanvas* c1 = new TCanvas("c1","c1",2400,600);
	c1->Divide(3,1);

	c1->cd(1);
        TPad* pad1_1 = new TPad("pad1_1", "pad1_1",0,0,1,1);
        pad1_1->SetBottomMargin(0.10);
        pad1_1->SetTopMargin(0.10);
        pad1_1->SetRightMargin(0.12);
        pad1_1->SetLeftMargin(0.12);
        pad1_1->Draw();
        pad1_1->cd();
	hBr->Draw("colz");

	c1->cd(2);
        TPad* pad1_2 = new TPad("pad1_2", "pad1_2",0,0,1,1);
        pad1_2->SetBottomMargin(0.10);
        pad1_2->SetTopMargin(0.10);
        pad1_2->SetRightMargin(0.12);
        pad1_2->SetLeftMargin(0.12);
        pad1_2->Draw();
        pad1_2->cd();
	hBz->Draw("colz");

	c1->cd(3);
        TPad* pad1_3 = new TPad("pad1_3", "pad1_3",0,0,1,1);
        pad1_3->SetBottomMargin(0.10);
        pad1_3->SetTopMargin(0.10);
        pad1_3->SetRightMargin(0.12);
        pad1_3->SetLeftMargin(0.12);
        pad1_3->Draw();
        pad1_3->cd();
	hB->Draw("cont4 & colz");

	c1->SaveAs(fNameOut.Data());

	delete c1;
	delete hBr;
	delete hBz;
	delete hB;

	return;
}
