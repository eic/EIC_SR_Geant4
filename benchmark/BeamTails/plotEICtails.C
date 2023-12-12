const Double_t nSigmaTail = 5;

Double_t getRho(Double_t *x, Double_t *par)
{
	return ( (x[0] > nSigmaTail ) ? ( TMath::Exp((-0.5)*TMath::Power(x[0],2)) + par[0]/TMath::Power(x[0],3) ) : TMath::Exp((-0.5)*TMath::Power(x[0],2)) );
}

Double_t getRhoxy(Double_t *x, Double_t *par)
{
	Double_t xx = TMath::Abs(x[0]);
	Double_t yy = TMath::Abs(x[1]);
	Double_t Kx = par[0];
	Double_t Ky = par[1];

	Double_t tailx = Kx/TMath::Power(xx,3);
	Double_t taily = Ky/TMath::Power(yy,3);
	Double_t rhoxy = TMath::Exp((-0.5)*TMath::Power(xx,2)+(-0.5)*TMath::Power(yy,2));
	
	if(xx > nSigmaTail){rhoxy += tailx;}
	if(yy > nSigmaTail){rhoxy += taily;}

	return rhoxy;
}

Double_t getRhoCore(Double_t *x, Double_t *par)
{
	return ( TMath::Exp((-0.5)*TMath::Power(x[0],2)) );
}

Double_t getRhoTail(Double_t *x, Double_t *par)
{
	return ( par[0]/TMath::Power(x[0],3) );
}

void plotEICtails()
{
	const Int_t nSigma = 20;

	//-------- At 10 GeV, 5 nTorr --------//
	const Double_t Kx = 25e-7; // [m^3]
	const Double_t Ky = 450e-7; // [m^3]
	//-------- At 18 GeV, 10 nTorr --------//
//	const Double_t Kx = 0; // [m^3]
//	const Double_t Ky = 25e-7; // [m^3]
	//------------------------------------//
	
	TF1* grRhox = new TF1("grRhox",getRho,0,nSigma,1);
	TF1* grRhoy = new TF1("grRhoy",getRho,0,nSigma,1);
	TF2* grRhoxy = new TF2("grRhoxy",getRhoxy,-nSigma,nSigma,-nSigma,nSigma,2);
	grRhoxy->SetNpx(1000);
	grRhoxy->SetNpy(1000);

	TF1* grRhoCorex = new TF1("grRhoCorex",getRhoCore,0,nSigma,1);
	TF1* grRhoCorey = new TF1("grRhoCorey",getRhoCore,0,nSigma,1);

	TF1* grRhoTailx = new TF1("grRhoTailx",getRhoTail,nSigmaTail,nSigma,1);
	TF1* grRhoTaily = new TF1("grRhoTaily",getRhoTail,nSigmaTail,nSigma,1);

	grRhox->SetParameter(0,Kx);
	grRhoy->SetParameter(0,Ky);
	grRhoxy->SetParameters(Kx,Ky);

	grRhoCorex->SetParameter(0,Kx);
	grRhoCorey->SetParameter(0,Ky);

	grRhoTailx->SetParameter(0,Kx);
	grRhoTaily->SetParameter(0,Ky);

	// fill histograms with random numbers
	TH1F* hCorex = new TH1F("hCorex","",nSigma*10,0,nSigma);
	TH1F* hCorey = new TH1F("hCorey","",nSigma*10,0,nSigma);
	TH1F* hTailx = new TH1F("hTailx","",nSigma*10,0,nSigma);
	TH1F* hTaily = new TH1F("hTaily","",nSigma*10,0,nSigma);
	TH2F* hBeamxy = new TH2F("hBeamxy","",nSigma*10,-nSigma,nSigma,nSigma*10,-nSigma,nSigma);

	for(Int_t i = 0; i < 1000000; i++)
	{
		hCorex->Fill(grRhoCorex->GetRandom());
		hCorey->Fill(grRhoCorey->GetRandom());
		hTailx->Fill(grRhoTailx->GetRandom());
		hTaily->Fill(grRhoTaily->GetRandom());
		Double_t xrnd, yrnd;
		grRhoxy->GetRandom2(xrnd,yrnd);
		hBeamxy->Fill(xrnd,yrnd);
	}

	// plot
	gStyle->SetOptStat(0);

	TCanvas* c0 = new TCanvas("c0","c0",900,900);
	c0->cd();
	hBeamxy->Draw("colz");
	hBeamxy->SetMinimum(1e-11);
	gPad->SetLogz();
	gPad->SetGrid();

	TCanvas* c1 = new TCanvas("c1","c1",1800,900);
	c1->Divide(2,1);

	//-- X
	c1->cd(1);

	if(hTailx->GetMaximum() > 0)hTailx->Scale(1./hTailx->GetMaximum());
	hTailx->Scale(grRhoTailx->Eval(nSigmaTail));

	hCorex->Scale(1./hCorex->GetMaximum());

	hCorex->SetFillColorAlpha(kBlue,0.3);
	hTailx->SetFillColorAlpha(kRed,0.3);

	hCorex->SetFillStyle(3345);
	hTailx->SetFillStyle(3354);

	hCorex->SetLineColor(kBlue);
	hTailx->SetLineColor(kRed);

	THStack* hsx = new THStack("hsx","");
	hsx->Add(hTailx);
	hsx->Add(hCorex);

	hsx->SetMinimum(1e-11);
	hsx->Draw("HIST");
	hsx->GetXaxis()->SetTitle("X/#sigma_{X}");
	hsx->SetTitle(Form("K_{X} = %.1e",Kx));

	grRhox->SetLineWidth(6);
	grRhox->SetLineStyle(9);
	grRhox->SetLineColor(kBlack);

	grRhox->Draw("SAME & L");

	gPad->SetLogy();
	gPad->SetGrid();

	TLegend* legx = new TLegend(0.4,0.6,0.9,0.9);
	legx->AddEntry(hCorex,Form("Core (#Sigma = %.3e)",hCorex->Integral()),"f");
	legx->AddEntry(hTailx,Form("Tail (#Sigma = %.3e)",hTailx->Integral()),"f");
	legx->AddEntry(grRhox,"Core+Tail","l");
	legx->Draw();

	//-- Y
	c1->cd(2);

	hTaily->Scale(1./hTaily->GetMaximum());
	hTaily->Scale(grRhoTaily->Eval(nSigmaTail));

	hCorey->Scale(1./hCorey->GetMaximum());

	hCorey->SetFillColorAlpha(kBlue,0.3);
	hTaily->SetFillColorAlpha(kRed,0.3);

	hCorey->SetFillStyle(3345);
	hTaily->SetFillStyle(3354);

	hCorey->SetLineColor(kBlue);
	hTaily->SetLineColor(kRed);

	THStack* hsy = new THStack("hsy","");
	hsy->Add(hTaily);
	hsy->Add(hCorey);

	hsy->SetMinimum(1e-11);
	hsy->Draw("HIST");
	hsy->GetXaxis()->SetTitle("Y/#sigma_{Y}");
	hsy->SetTitle(Form("K_{Y} = %.1e",Ky));

	grRhoy->SetLineWidth(6);
	grRhoy->SetLineStyle(9);
	grRhoy->SetLineColor(kBlack);

	grRhoy->Draw("SAME & L");

	gPad->SetLogy();
	gPad->SetGrid();

	TLegend* legy = new TLegend(0.4,0.6,0.9,0.9);
	legy->AddEntry(hCorey,Form("Core (#Sigma = %.3e)",hCorey->Integral()),"f");
	legy->AddEntry(hTaily,Form("Tail (#Sigma = %.3e)",hTaily->Integral()),"f");
	legy->AddEntry(grRhoy,"Core+Tail","l");
	legy->Draw();

	c1->SaveAs("c1.png");
	
	return;
}


