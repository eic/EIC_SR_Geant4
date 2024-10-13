void draw_hepmc()
{
	TFile *_file0 = TFile::Open("output/hepmc_output.root");
	TH1D*	h_posx = (TH1D*)_file0->Get("h_posx"); //1	posx
	TH1D*	h_posy = (TH1D*)_file0->Get("h_posy"); //1	posy
	TH1D*	h_posz = (TH1D*)_file0->Get("h_posz"); //1	posz
	TH1D*	h_dirx = (TH1D*)_file0->Get("h_dirx"); //1	momx
	TH1D*	h_diry = (TH1D*)_file0->Get("h_diry"); //1	momy
	TH1D*	h_dirz = (TH1D*)_file0->Get("h_dirz"); //1	momz
	TH1D*	h_mag = (TH1D*)_file0->Get("h_mag"); //1	magp
	TH1D*	h_phi = (TH1D*)_file0->Get("h_phi"); //1	phi
	TH1D*	h_theta = (TH1D*)_file0->Get("h_theta"); //1	theta
	TH1D*	h_r = (TH1D*)_file0->Get("h_r"); //1	r
	TH1D*	h_psi = (TH1D*)_file0->Get("h_psi"); //1	psi
	TH2D*	h_cor = (TH2D*)_file0->Get("h_cor"); //1	cor
	TH2D*	h_err = (TH2D*)_file0->Get("h_err"); //1	err
	TH1D*	h_rnd_mag = (TH1D*)_file0->Get("h_rnd_mag"); //1	magp
	TH1D*	h_rnd_phi = (TH1D*)_file0->Get("h_rnd_phi"); //1	phi
	TH1D*	h_rnd_theta = (TH1D*)_file0->Get("h_rnd_theta"); //1	theta
	TH1D*	h_rnd_posz = (TH1D*)_file0->Get("h_rnd_posz"); //1	posz
	TH1D*	h_rnd_psi = (TH1D*)_file0->Get("h_rnd_psi"); //1	psi
	TH2D*	h_rnd_cor = (TH2D*)_file0->Get("h_rnd_cor"); //1	cor
	TH2D*	h_rnd_err = (TH2D*)_file0->Get("h_rnd_err"); //1	err
	TH2D*	h_all_cor = (TH2D*)_file0->Get("h_all_cor"); //1	cor
	TH2D*	h_all_err = (TH2D*)_file0->Get("h_all_err"); //1	err

	gStyle->SetOptStat(0);

	TCanvas* c1 = new TCanvas("c1","c1",1800,900);
	c1->Divide(3,2);
	c1->cd(1); h_posx->Draw("HIST");
	c1->cd(2); h_posy->Draw("HIST");
	c1->cd(3); h_posz->Draw("HIST");
	c1->cd(4); h_dirx->Draw("HIST");
	c1->cd(5); h_diry->Draw("HIST");
	c1->cd(6); h_dirz->Draw("HIST");

	TCanvas* c2 = new TCanvas("c2","c2",1800,900);
	c2->Divide(4,2);

	h_rnd_psi->SetLineColor(kRed);
	h_rnd_posz->SetLineColor(kRed);
	h_rnd_mag->SetLineColor(kRed);
	h_rnd_theta->SetLineColor(kRed);
	h_rnd_phi->SetLineColor(kRed);

	h_rnd_psi->SetLineWidth(2);
	h_rnd_posz->SetLineWidth(2);
	h_rnd_mag->SetLineWidth(2);
	h_rnd_theta->SetLineWidth(2);
	h_rnd_phi->SetLineWidth(2);

	h_psi->SetFillColorAlpha(kBlue,0.2);
	h_posz->SetFillColorAlpha(kBlue,0.2);
	h_mag->SetFillColorAlpha(kBlue,0.2);
	h_theta->SetFillColorAlpha(kBlue,0.2);
	h_phi->SetFillColorAlpha(kBlue,0.2);

	c1->SaveAs("c1.png");

	//=====================================================================//
	Double_t nEntries = h_psi->GetEntries();
	cout<<"nEntries = "<<nEntries<<endl;
	h_r->Scale(1.0/nEntries);
	h_psi->Scale(1.0/nEntries);
	h_posz->Scale(1.0/nEntries);
	h_mag->Scale(1.0/nEntries);
	h_theta->Scale(1.0/nEntries);
	h_phi->Scale(1.0/nEntries);

	Double_t nEntries_rnd = h_rnd_psi->GetEntries();
	cout<<"nEntries_rnd = "<<nEntries_rnd<<endl;
	h_rnd_psi->Scale(1.0/nEntries_rnd);
	h_rnd_posz->Scale(1.0/nEntries_rnd);
	h_rnd_mag->Scale(1.0/nEntries_rnd);
	h_rnd_theta->Scale(1.0/nEntries_rnd);
	h_rnd_phi->Scale(1.0/nEntries_rnd);


	h_r->GetXaxis()->SetRangeUser(30,32);
	h_psi->GetXaxis()->SetRangeUser(-4,4);
	h_mag->GetXaxis()->SetRangeUser(0,20000e-9);
	h_theta->GetXaxis()->SetRangeUser(2.9,3.2);
	h_phi->GetXaxis()->SetRangeUser(-4,4);

	gStyle->SetPaintTextFormat("1.2f");
	h_cor->SetMarkerSize(2);
	h_rnd_cor->SetMarkerSize(2);
	h_all_cor->SetMarkerSize(2);

	c2->cd(1); 
	TPad* p2_1 = new TPad("p2_1","p2_1",0,0,1,1);
   	p2_1->SetTopMargin(0.1);
	p2_1->SetBottomMargin(0.2);
	p2_1->SetLeftMargin(0.1);
	p2_1->SetRightMargin(0.2);
	p2_1->Draw();
	p2_1->cd();
	h_r->SetLineWidth(3);
	h_r->Draw("HIST"); 
	gPad->SetGrid();
	h_r->SetTitle("XYZ: r [mm]");
	h_r->GetYaxis()->SetLabelSize(0.05);
	h_r->GetXaxis()->SetLabelSize(0.05);
	h_r->GetXaxis()->SetNdivisions(5);

	c2->cd(2); 
	TPad* p2_2_1 = new TPad("p2_2_1","p2_2_1",0,0.2,1,1);
   	p2_2_1->SetTopMargin(0.1);
	p2_2_1->SetBottomMargin(0.0);
	p2_2_1->SetLeftMargin(0.1);
	p2_2_1->SetRightMargin(0.14);
	p2_2_1->Draw();
	p2_2_1->cd();
	h_psi->SetTitle("XYZ: #psi [rad]");
	h_psi->Draw("HIST"); 
	h_rnd_psi->Draw("SAME & HIST"); 
	gPad->SetLogy(); 
	gPad->SetGrid();
	h_psi->GetYaxis()->SetLabelSize(0.06);
	h_psi->GetXaxis()->SetLabelSize(0.0);
	h_psi->GetXaxis()->SetNdivisions(5);
	c2->cd(2);
	TPad* p2_2_2 = new TPad("p2_2_2","p2_2_2",0,0,1,0.2);
   	p2_2_2->SetTopMargin(0.0);
	p2_2_2->SetBottomMargin(0.24);
	p2_2_2->SetLeftMargin(0.1);
	p2_2_2->SetRightMargin(0.14);
	p2_2_2->Draw();
	p2_2_2->cd();
	TH1D* h_res_psi = (TH1D*)h_psi->Clone("h_res_psi");
	h_res_psi->Add(h_rnd_psi,-1);
	h_res_psi->Divide(h_psi);
	h_res_psi->SetMinimum(-0.15);
	h_res_psi->SetMaximum(+0.15);
	h_res_psi->SetLineColorAlpha(kGreen+2,0.4);
	h_res_psi->Draw("P");	
	gPad->SetGrid();
	h_res_psi->SetTitle("");
	h_res_psi->GetYaxis()->SetLabelSize(0.25);
	h_res_psi->GetXaxis()->SetLabelSize(0.25);
	h_res_psi->GetYaxis()->SetNdivisions(3);

	c2->cd(3); 
	TPad* p2_3_1 = new TPad("p2_3_1","p2_3_1",0,0.2,1,1);
   	p2_3_1->SetTopMargin(0.1);
	p2_3_1->SetBottomMargin(0.0);
	p2_3_1->SetLeftMargin(0.1);
	p2_3_1->SetRightMargin(0.14);
	p2_3_1->Draw();
	p2_3_1->cd();
	h_posz->SetTitle("XYZ: z [mm]");
	h_posz->Draw("HIST"); 
	h_rnd_posz->Draw("SAME & HIST"); 
	gPad->SetLogy(); 
	gPad->SetGrid();
	h_posz->GetYaxis()->SetLabelSize(0.06);
	h_posz->GetXaxis()->SetLabelSize(0.0);
	h_posz->GetXaxis()->SetNdivisions(5);
	c2->cd(3);
	TPad* p2_3_2 = new TPad("p2_3_2","p2_3_2",0,0,1,0.2);
   	p2_3_2->SetTopMargin(0.0);
	p2_3_2->SetBottomMargin(0.24);
	p2_3_2->SetLeftMargin(0.1);
	p2_3_2->SetRightMargin(0.14);
	p2_3_2->Draw();
	p2_3_2->cd();
	TH1D* h_res_posz = (TH1D*)h_posz->Clone("h_res_posz");
	h_res_posz->Add(h_rnd_posz,-1);
	h_res_posz->Divide(h_posz);
	h_res_posz->SetMinimum(-0.15);
	h_res_posz->SetMaximum(+0.15);
	h_res_posz->SetLineColorAlpha(kGreen+2,0.4);
	h_res_posz->Draw("P");	
	gPad->SetGrid();
	h_res_posz->SetTitle("");
	h_res_posz->GetYaxis()->SetLabelSize(0.25);
	h_res_posz->GetXaxis()->SetLabelSize(0.25);
	h_res_posz->GetYaxis()->SetNdivisions(3);

	c2->cd(4);
	TPad* p2_4 = new TPad("p2_4","p2_4",0,0,1,1);
   	p2_4->SetTopMargin(0.1);
	p2_4->SetBottomMargin(0.2);
	p2_4->SetLeftMargin(0.1);
	p2_4->SetRightMargin(0.2);
	p2_4->Draw();
	p2_4->cd();
	h_cor->Draw("COLZ & TEXT"); 
	gPad->SetGrid();
	h_cor->SetTitle("SIM: Pearson Correlation Coeff.");
	h_cor->GetYaxis()->SetLabelSize(0.06);
	h_cor->GetXaxis()->SetLabelSize(0.06);
	h_cor->GetZaxis()->SetLabelSize(0.05);
	h_cor->SetMinimum(-0.5);
	h_cor->SetMaximum(+1.0);

	c2->cd(5); 
	TPad* p2_5_1 = new TPad("p2_5_1","p2_5_1",0,0.2,1,1);
   	p2_5_1->SetTopMargin(0.1);
	p2_5_1->SetBottomMargin(0.0);
	p2_5_1->SetLeftMargin(0.1);
	p2_5_1->SetRightMargin(0.14);
	p2_5_1->Draw();
	p2_5_1->cd();
	h_mag->SetTitle("PxPyPz: E [GeV]");
	h_mag->Draw("HIST"); 
	h_rnd_mag->Draw("SAME & HIST"); 
	gPad->SetLogy(); 
	gPad->SetGrid();
	h_mag->GetYaxis()->SetLabelSize(0.06);
	h_mag->GetXaxis()->SetLabelSize(0.0);
	h_mag->GetXaxis()->SetNdivisions(5);
	c2->cd(5);
	TPad* p2_5_2 = new TPad("p2_5_2","p2_5_2",0,0,1,0.2);
   	p2_5_2->SetTopMargin(0.0);
	p2_5_2->SetBottomMargin(0.24);
	p2_5_2->SetLeftMargin(0.1);
	p2_5_2->SetRightMargin(0.14);
	p2_5_2->Draw();
	p2_5_2->cd();
	TH1D* h_res_mag = (TH1D*)h_mag->Clone("h_res_mag");
	h_res_mag->Add(h_rnd_mag,-1);
	h_res_mag->Divide(h_mag);
	h_res_mag->SetMinimum(-0.15);
	h_res_mag->SetMaximum(+0.15);
	h_res_mag->SetLineColorAlpha(kGreen+2,0.4);
	h_res_mag->Draw("P");	
	gPad->SetGrid();
	h_res_mag->SetTitle("");
	h_res_mag->GetYaxis()->SetLabelSize(0.25);
	h_res_mag->GetXaxis()->SetLabelSize(0.25);
	h_res_mag->GetYaxis()->SetNdivisions(3);

	c2->cd(6); 
	TPad* p2_6_1 = new TPad("p2_6_1","p2_6_1",0,0.2,1,1);
   	p2_6_1->SetTopMargin(0.1);
	p2_6_1->SetBottomMargin(0.0);
	p2_6_1->SetLeftMargin(0.1);
	p2_6_1->SetRightMargin(0.14);
	p2_6_1->Draw();
	p2_6_1->cd();
	h_theta->SetTitle("PxPyPz: #theta [rad]");
	h_theta->Draw("HIST"); 
	h_rnd_theta->Draw("SAME & HIST"); 
	gPad->SetLogy(); 
	gPad->SetGrid();
	h_theta->GetYaxis()->SetLabelSize(0.06);
	h_theta->GetXaxis()->SetLabelSize(0.0);
	h_theta->GetXaxis()->SetNdivisions(5);
	c2->cd(6);
	TPad* p2_6_2 = new TPad("p2_6_2","p2_6_2",0,0,1,0.2);
   	p2_6_2->SetTopMargin(0.0);
	p2_6_2->SetBottomMargin(0.24);
	p2_6_2->SetLeftMargin(0.1);
	p2_6_2->SetRightMargin(0.14);
	p2_6_2->Draw();
	p2_6_2->cd();
	TH1D* h_res_theta = (TH1D*)h_theta->Clone("h_res_theta");
	h_res_theta->Add(h_rnd_theta,-1);
	h_res_theta->Divide(h_theta);
	h_res_theta->SetMinimum(-0.75);
	h_res_theta->SetMaximum(+0.75);
	h_res_theta->SetLineColorAlpha(kGreen+2,0.4);
	h_res_theta->Draw("P");	
	gPad->SetGrid();
	h_res_theta->SetTitle("");
	h_res_theta->GetYaxis()->SetLabelSize(0.25);
	h_res_theta->GetXaxis()->SetLabelSize(0.25);
	h_res_theta->GetYaxis()->SetNdivisions(3);

	c2->cd(7); 
	TPad* p2_7_1 = new TPad("p2_7_1","p2_7_1",0,0.2,1,1);
   	p2_7_1->SetTopMargin(0.1);
	p2_7_1->SetBottomMargin(0.0);
	p2_7_1->SetLeftMargin(0.1);
	p2_7_1->SetRightMargin(0.14);
	p2_7_1->Draw();
	p2_7_1->cd();
	h_phi->SetTitle("PxPyPz: #phi [rad]");
	h_phi->Draw("HIST"); 
	h_rnd_phi->Draw("SAME & HIST"); 
	gPad->SetLogy(); 
	gPad->SetGrid();
	h_phi->GetYaxis()->SetLabelSize(0.06);
	h_phi->GetXaxis()->SetLabelSize(0.0);
	h_phi->GetXaxis()->SetNdivisions(5);
	c2->cd(7);
	TPad* p2_7_2 = new TPad("p2_7_2","p2_7_2",0,0,1,0.2);
   	p2_7_2->SetTopMargin(0.0);
	p2_7_2->SetBottomMargin(0.24);
	p2_7_2->SetLeftMargin(0.1);
	p2_7_2->SetRightMargin(0.14);
	p2_7_2->Draw();
	p2_7_2->cd();
	TH1D* h_res_phi = (TH1D*)h_phi->Clone("h_res_phi");
	h_res_phi->Add(h_rnd_phi,-1);
	h_res_phi->Divide(h_phi);
	h_res_phi->SetMinimum(-0.75);
	h_res_phi->SetMaximum(+0.75);
	h_res_phi->SetLineColorAlpha(kGreen+2,0.4);
	h_res_phi->Draw("P");	
	gPad->SetGrid();
	h_res_phi->SetTitle("");
	h_res_phi->GetYaxis()->SetLabelSize(0.25);
	h_res_phi->GetXaxis()->SetLabelSize(0.25);
	h_res_phi->GetYaxis()->SetNdivisions(3);

	c2->cd(8); 
	TPad* p2_8 = new TPad("p2_8","p2_8",0,0,1,1);
   	p2_8->SetTopMargin(0.1);
	p2_8->SetBottomMargin(0.2);
	p2_8->SetLeftMargin(0.1);
	p2_8->SetRightMargin(0.2);
	p2_8->Draw();
	p2_8->cd();
	h_rnd_cor->Draw("COLZ & TEXT"); 
	gPad->SetGrid();
	h_rnd_cor->SetTitle("GEN: Pearson Correlation Coeff.");
	h_rnd_cor->GetYaxis()->SetLabelSize(0.06);
	h_rnd_cor->GetXaxis()->SetLabelSize(0.06);
	h_rnd_cor->GetZaxis()->SetLabelSize(0.05);
	h_rnd_cor->SetMinimum(-0.5);
	h_rnd_cor->SetMaximum(+1.0);

	c2->SaveAs("c2.png");

	gStyle->SetPaintTextFormat("1.e");
	TCanvas* c3 = new TCanvas("c3","c3",450,450);
	c3->cd(); 
	TPad* p3 = new TPad("p3","p3",0,0,1,1);
   	p3->SetTopMargin(0.1);
	p3->SetBottomMargin(0.2);
	p3->SetLeftMargin(0.1);
	p3->SetRightMargin(0.2);
	p3->Draw();
	p3->cd();
	h_all_cor->Draw("COLZ & TEXT"); 
	gPad->SetGrid();
	h_all_cor->SetTitle("GEN vs SIM: Pearson Correlation Coeff.");
	h_all_cor->GetYaxis()->SetLabelSize(0.06);
	h_all_cor->GetXaxis()->SetLabelSize(0.06);
	h_all_cor->GetZaxis()->SetLabelSize(0.05);
	h_all_cor->SetMinimum(-0.5);
	h_all_cor->SetMaximum(+1.0);

	c3->SaveAs("c3.png");


	return;
} 



/*
 TFile*		hepmc_output.root	
  KEY: TH1D	h_posx;1	posx
  KEY: TH1D	h_posy;1	posy
  KEY: TH1D	h_posz;1	posz
  KEY: TH1D	h_dirx;1	momx
  KEY: TH1D	h_diry;1	momy
  KEY: TH1D	h_dirz;1	momz
  KEY: TH1D	h_mag;1	magp
  KEY: TH1D	h_phi;1	phi
  KEY: TH1D	h_theta;1	theta
  KEY: TH1D	h_r;1	r
  KEY: TH1D	h_psi;1	psi
  KEY: TH2D	h_cor;1	cor
  KEY: TH2D	h_err;1	err
  KEY: TH1D	h_rnd_mag;1	magp
  KEY: TH1D	h_rnd_phi;1	phi
  KEY: TH1D	h_rnd_theta;1	theta
  KEY: TH1D	h_rnd_posz;1	posz
  KEY: TH1D	h_rnd_psi;1	psi
  KEY: TH2D	h_rnd_cor;1	cor
  KEY: TH2D	h_rnd_err;1	err
root [2] TFile *_file0 = TFile::Open("hepmc_output.root")
*/
