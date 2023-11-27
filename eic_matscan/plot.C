/***********************************************************************************************/
/***********************************************************************************************/
/* Plot MC partile distribution and material scan results                                      */
/* One should merge all output PODIO.ROOT files into one using the 'hadd' command; see below   */
/***********************************************************************************************/
/***********************************************************************************************/

// hadd -f /gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_podio_all.root /gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6/output_eicrecon_*.podio.root

#define MINX -4 // [cm]
#define MAXX  4 // [cm]
#define MINY -4 // [cm]
#define MAXY  4 // [cm]
#define MINZ -100 // [cm]
#define MAXZ  100 // [cm]

void plotMCparticles(TString fNameIn, TString fNameOut);
void plotMaterials(TString fNameIn, TString fNameOut);
void plotMatScanHisto(TString fNameIn, TString fNameOut, TString plane);
void plotMCpartAndMatScanProfileY(TString fNameIn, TString fNameInMc1, TString fNameInMc2, TString fNameOut);

void plot()
{	
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	//-----------------------------------------------------------//
	// MC particles (use eic-shell)
	plotMCparticles("/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_podio_all.root","./output/output_eicrecon_ip6_geant4_podio_all_hist.root");
	plotMCparticles("/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_ip6_synrad_XZflip/output_eicrecon.podio.root","./output/output_eicrecon_ip6_synrad_podio_all_hist.root");
	//-----------------------------------------------------------//
	// Material scan
	plotMaterials("/gpfs/group/belle2/TMP/anatochi/eic/output_matscan/g4mat_scan_*.root","./output/g4mat_scan_histo.root");
	plotMatScanHisto("./output/g4mat_scan_histo.root","./output/X0_2D","xz");
	//-----------------------------------------------------------//
	// MC particles onto Material scan
	plotMCpartAndMatScanProfileY(	"./output/g4mat_scan_histo.root",
					"./output/output_eicrecon_ip6_geant4_podio_all_hist.root",
					"./output/output_eicrecon_ip6_synrad_podio_all_hist.root",
					"./output/matscan_geant4_synrad");

	return;
}

void plotMCpartAndMatScanProfileY(TString fNameIn, TString fNameInMc1, TString fNameInMc2, TString fNameOut)
{
	cout<<"FUNCTION:: "<<__FUNCTION__<<endl;
	gStyle->SetOptStat(0);

	TFile *_file0 = TFile::Open(fNameIn.Data());
	TH1F* hx0_projY = (TH1F*)_file0->Get("hx0_x");

	TFile *_file1 = TFile::Open(fNameInMc1.Data());
	TH2F* hmc1 = (TH2F*)_file1->Get("hmc_xz");

	TFile *_file2 = TFile::Open(fNameInMc2.Data());
	TH2F* hmc2 = (TH2F*)_file2->Get("hmc_xz");

	TH1F* hmc1_projY = (TH1F*)hmc1->ProjectionY("hmc1_projY");
	TH1F* hmc2_projY = (TH1F*)hmc2->ProjectionY("hmc2_projY");

	TCanvas* c1 = new TCanvas("c1","c1",900,900); 
	c1->cd(); 
        TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
        pad1->SetBottomMargin(0.12);
        pad1->SetTopMargin(0.12);
        pad1->SetRightMargin(0.12);
        pad1->SetLeftMargin(0.12);
        pad1->Draw();
        pad1->cd();

	gPad->SetGrid();
	gPad->SetLogy(); 

	hx0_projY->SetLineWidth(3);
	hx0_projY->SetLineColor(kBlack);
	hmc1_projY->SetLineColor(kBlue);
	hmc2_projY->SetLineColor(kRed);

	hx0_projY->SetFillColorAlpha(kBlack,0.2);
	hmc1_projY->SetFillColorAlpha(kBlue,0.2);
	hmc2_projY->SetFillColorAlpha(kRed,0.2);

	hmc1_projY->SetFillStyle(3345);
	hmc2_projY->SetFillStyle(3354);

	hx0_projY->GetXaxis()->SetRangeUser(3.0,3.2);
	hx0_projY->SetMinimum(1e-9);
	hx0_projY->SetMaximum(1e+9);

	hx0_projY->Draw("HIST");
	hmc1_projY->Draw("SAME & HIST");
	hmc2_projY->Draw("SAME & HIST");

	c1->SaveAs(Form("%s_projY.png",fNameOut.Data()));
	c1->SaveAs(Form("%s_projY.root",fNameOut.Data()));
	delete c1;

	_file0->Close();
	_file1->Close();
	_file2->Close();

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
	TH2F* hx0_xz = new TH2F("hx0_xz","XZ;Z [cm];X [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXX-MINX)*10000,MINX,MAXX);
	TH2F* hx0_yz = new TH2F("hx0_yz","YZ;Z [cm];Y [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXY-MINY)*10000,MINY,MAXY);
	TH2F* hx0_yx = new TH2F("hx0_yx","YX;X [cm];Y [cm]",(MAXX-MINX)*100,MINX,MAXX,(MAXY-MINY)*100,MINY,MAXY);
	TH1F* hx0_x = new TH1F("hx0_x","X;X [cm]",(MAXX-MINX)*1000000,MINX,MAXX);
	TH1F* hx0_y = new TH1F("hx0_y","Y;Y [cm]",(MAXY-MINY)*1000000,MINY,MAXY);

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
	TH2F* hmc_xz = new TH2F("hmc_xz","MC XZ;Z [cm];X [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXX-MINX)*10000,MINX,MAXX);
	TH2F* hmc_yz = new TH2F("hmc_yz","MC YZ;Z [cm];Y [cm]",(MAXZ-MINZ)*1,MINZ,MAXZ,(MAXY-MINY)*10000,MINY,MAXY);
	TH2F* hmc_yx = new TH2F("hmc_yx","MC YX;X [cm];Y [cm]",(MAXX-MINX)*100,MINX,MAXX,(MAXY-MINY)*100,MINY,MAXY);

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

	//-----------------------------------------------------------//
	cout<<"MC particles calculation ..."<<endl;
	while(myReader.Next())
	{
		for (Int_t imcPart = 0; imcPart < pdg_mc.GetSize(); imcPart++)
		{
			if(simulatorStatus_mc[imcPart])
			{
				hmc_xz->Fill(vz_mc[imcPart]/1e1,vx_mc[imcPart]/1e1); // mm -> cm
				hmc_yz->Fill(vz_mc[imcPart]/1e1,vy_mc[imcPart]/1e1); // mm -> cm
				hmc_yx->Fill(vx_mc[imcPart]/1e1,vy_mc[imcPart]/1e1); // mm -> cm
			}
		}
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
