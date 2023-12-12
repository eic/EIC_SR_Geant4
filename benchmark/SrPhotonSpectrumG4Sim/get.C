void get()
{
	TChain* tree = new TChain("tree");
	// tree->Add("/group/belle2/TMP/anatochi/eic/output_g4sim_10files.root");
	tree->Add("/group/belle2/TMP/anatochi/eic/output_g4sim_test_all.root");
	tree->ls();

	cout<<"nEntries = "<<tree->GetEntries()<<endl;
	
	TH1D* h1_ene = new TH1D("h1_ene_g4","",1e6,0,1e6);
	TH1D* h1_dirx = new TH1D("h1_dirx_g4","",4e6,-2,2);
	TH1D* h1_diry = new TH1D("h1_diry_g4","",4e6,-2,2);
	TH1D* h1_dirz = new TH1D("h1_dirz_g4","",4e6,-2,2);
	TH1D* h1_posx = new TH1D("h1_posx_g4","",10000,-10,0);
	TH1D* h1_posy = new TH1D("h1_posy_g4","",2000,-1,1);
	TH1D* h1_posz = new TH1D("h1_posz_g4","",11000,-3400,-2300);

	/*
	// Y > 0
	tree->Draw("gammaEnergy_eV >> h1_ene_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	tree->Draw("gammaMomX_eV/gammaEnergy_eV >> h1_dirx_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	tree->Draw("gammaMomY_eV/gammaEnergy_eV >> h1_diry_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	tree->Draw("gammaMomZ_eV/gammaEnergy_eV >> h1_dirz_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	tree->Draw("gammaPosX_cm >> h1_posx_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	tree->Draw("gammaPosY_cm >> h1_posy_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	tree->Draw("gammaPosZ_cm >> h1_posz_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 < gammaPosY_cm");
	TFile* file = new TFile("geant4_data_posy.root","recreate");
	*/
	/*
	// Y < 0
	tree->Draw("gammaEnergy_eV >> h1_ene_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	tree->Draw("gammaMomX_eV/gammaEnergy_eV >> h1_dirx_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	tree->Draw("gammaMomY_eV/gammaEnergy_eV >> h1_diry_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	tree->Draw("gammaMomZ_eV/gammaEnergy_eV >> h1_dirz_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	tree->Draw("gammaPosX_cm >> h1_posx_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	tree->Draw("gammaPosY_cm >> h1_posy_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	tree->Draw("gammaPosZ_cm >> h1_posz_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925 && 0 > gammaPosY_cm");
	TFile* file = new TFile("geant4_data_negy.root","recreate");
	*/

	// Y any
	tree->Draw("gammaEnergy_eV >> h1_ene_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	tree->Draw("gammaMomX_eV/gammaEnergy_eV >> h1_dirx_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	tree->Draw("gammaMomY_eV/gammaEnergy_eV >> h1_diry_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	tree->Draw("gammaMomZ_eV/gammaEnergy_eV >> h1_dirz_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	tree->Draw("gammaPosX_cm >> h1_posx_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	tree->Draw("gammaPosY_cm >> h1_posy_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	tree->Draw("gammaPosZ_cm >> h1_posz_g4","-3350 < gammaPosZ_cm && gammaPosZ_cm < -2350 && gammaPosX_cm < -4.925");
	TFile* file = new TFile("geant4_data_test.root","recreate");

	file->cd();
	h1_ene->Write();
	h1_dirx->Write();
	h1_diry->Write();
	h1_dirz->Write();
	h1_posx->Write();
	h1_posy->Write();
	h1_posz->Write();
	file->Close();			
	cout<<"Output file name = "<<file->GetName()<<endl;

	return;
}
