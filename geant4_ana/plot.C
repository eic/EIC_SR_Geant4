
void DrawEllipse(Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta, Double_t scale, Int_t nSigma, Color_t color);
void GetEllipseValue(Double_t alpha, Double_t beta, Double_t emit, Double_t XX, Int_t sign, Double_t &x, Double_t &xp);
void DrawData(	TString name, TH2F* hX, 
		Double_t minX, Double_t maxX, Double_t minXp, Double_t maxXp, 
		Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta);

using namespace std;

void plot()
{
	gStyle->SetOptStat(0);
	gStyle->SetLegendFont(42);
	gStyle->SetLegendTextSize(0.035);

	TFile *file = TFile::Open("output/output.root");

	TH1F* h1 = (TH1F*)file->Get("h1");
	TH2F* h2 = (TH2F*)file->Get("h2");
	TH2F* h3 = (TH2F*)file->Get("h3");
	TH2F* h4 = (TH2F*)file->Get("h4");
	TH2F* h5 = (TH2F*)file->Get("h5");
	TH1F* h6 = (TH1F*)file->Get("h6");
	TH2F* h7 = (TH2F*)file->Get("h7");
	TH2F* h8 = (TH2F*)file->Get("h8");
	TH1F* h9 = (TH1F*)file->Get("h9");

	DrawData("beamX",h7,
		-1.0,1.0,-0.5e-3,0.5e-3,	// hor (X) range
		24e-9,-7.031713181,161.7921567,-0.07618056746,0.02481186422,0.109e-2); // hor (X) twiss parameters

	DrawData("beamY",h8,
		-0.4,0.4,-0.4e-4,0.4e-4,	// ver (Y) range
		2e-9,-1.226925635,121.6383193,0,0,0.109e-2); // ver (Y) twiss parameters


	return;
}

void DrawData(	TString name, TH2F* hX, 
		Double_t minX, Double_t maxX, Double_t minXp, Double_t maxXp, 
		Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta)
{
	//-------------------------------------------------------------------//
	TCanvas* c1 = new TCanvas(Form("c1_%s",name.Data()),Form("c1_%s",name.Data()),900,900);
	c1->cd();   

	// central pad
	TPad* pad1 = new TPad("pad1", "center_pad",0.0,0.0,0.7,0.7);
    	pad1->SetBottomMargin(0.14);
    	pad1->SetTopMargin(0.10);
    	pad1->SetRightMargin(0.10);
    	pad1->SetLeftMargin(0.14);
   	pad1->Draw();
	
	// right pad
	TPad* pad2 = new TPad("pad2", "pad2",0.7,0.0,1.0,0.7);
    	pad2->SetBottomMargin(0.14);
    	pad2->SetTopMargin(0.10);
    	pad2->SetRightMargin(0.08);
    	pad2->SetLeftMargin(0.12);
	pad2->Draw();

	// top pad
	TPad* pad3 = new TPad("pad3", "pad3",0.0,0.7,0.7,1.0);
    	pad3->SetBottomMargin(0.10);
    	pad3->SetTopMargin(0.10);
    	pad3->SetRightMargin(0.10);
    	pad3->SetLeftMargin(0.14);
	pad3->Draw();

	if(maxX > minX) hX->GetXaxis()->SetRangeUser(minX,maxX);
	if(maxXp > minXp) hX->GetYaxis()->SetRangeUser(minXp,maxXp);

	TH1D* projh2X = hX->ProjectionX();
	TH1D* projh2Xp = hX->ProjectionY();

	pad1->cd();
	gStyle->SetPalette(1);
	hX->Draw("colz");
	gPad->SetGrid();

	DrawEllipse(emit,alpha,beta,eta,etaPrime,delta,1e2 /*[m->cm]*/, 3, kBlack);

	pad2->cd();
	projh2Xp->SetFillColorAlpha(kBlue,0.4);
	projh2Xp->Draw("hbar");
	projh2Xp->SetTitle("");
	gPad->SetGrid();

	pad3->cd();
	projh2X->SetFillColorAlpha(kBlue,0.4);
	projh2X->Draw("bar");
	projh2X->SetTitle("");
	gPad->SetGrid();

	c1->cd();
   	TLatex t;
   	t.SetTextFont(42);
   	t.SetTextSize(0.02);

	t.DrawLatex(0.7,0.98,"#font[22]{#Rightarrow Coordinates (X or Y) #Leftarrow}");
	t.DrawLatex(0.7,0.95,Form("#color[4]{#mu(SIM) = %.1e #pm %.1e [cm]}",projh2X->GetMean(),projh2X->GetMeanError()));
	t.DrawLatex(0.7,0.92,Form("#color[2]{#mu(TWI) = %.1e [cm]}",0.0));
   	t.DrawLatex(0.7,0.89,Form("#color[4]{#sigma(SIM) = %.2e #pm %.2e [cm]}",projh2X->GetStdDev(),projh2X->GetStdDevError()));
	t.DrawLatex(0.7,0.86,Form("#color[2]{#sigma(TWI) = %.2e [cm]}",TMath::Sqrt(emit*beta + TMath::Power(eta*delta,2))*1e2 /*[m->cm]*/));

	t.DrawLatex(0.7,0.83,"#font[22]{#Rightarrow Divergence (X' or Y') #Leftarrow}");
	t.DrawLatex(0.7,0.80,Form("#color[4]{#mu(SIM) = %.1e #pm %.1e [cm]}",projh2Xp->GetMean(),projh2Xp->GetMeanError()));
	t.DrawLatex(0.7,0.77,Form("#color[2]{#mu(TWI) = %.1e [cm]}",0.0));
   	t.DrawLatex(0.7,0.74,Form("#color[4]{#sigma(SIM) = %.2e #pm %.2e [cm]}",projh2Xp->GetStdDev(),projh2Xp->GetStdDevError()));
	t.DrawLatex(0.7,0.71,Form("#color[2]{#sigma(TWI) = %.2e [cm]}",TMath::Sqrt(emit*(1. + alpha*alpha)/beta + TMath::Power(etaPrime*delta,2))));

	c1->SaveAs(Form("./pics/c1_%s.png",name.Data()));

	return;
}

void DrawEllipse(Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta, Double_t scale, Int_t nSigma, Color_t color)
{
	Double_t gamma = (1. + alpha*alpha)/beta;
	
	TGraph* ellipse = new TGraph();

	Double_t x, xp, y, yp;
	for(Double_t XX = -TMath::Sqrt(emit); XX <= TMath::Sqrt(emit); XX += TMath::Sqrt(emit)/100.0)
	{
		GetEllipseValue(alpha,beta,emit,XX,+1,x,xp);
		ellipse->SetPoint(ellipse->GetN(),x*scale*nSigma,xp*nSigma);
	}
	for(Double_t XX = TMath::Sqrt(emit); XX >= -TMath::Sqrt(emit); XX -= TMath::Sqrt(emit)/100.0)
	{
		GetEllipseValue(alpha,beta,emit,XX,-1,x,xp);
		ellipse->SetPoint(ellipse->GetN(),x*scale*nSigma,xp*nSigma);
	}

	ellipse->SetLineColor(color);
	ellipse->SetLineStyle(1);
	ellipse->SetLineWidth(1);
	ellipse->Draw("SAME & C");

	// beam size/divergence limits
	TBox* limits = new TBox(-nSigma*TMath::Sqrt(emit*beta + TMath::Power(eta*delta,2))*scale,
				-nSigma*TMath::Sqrt(emit*gamma + TMath::Power(etaPrime*delta,2)),
				+nSigma*TMath::Sqrt(emit*beta + TMath::Power(eta*delta,2))*scale,
				+nSigma*TMath::Sqrt(emit*gamma + TMath::Power(etaPrime*delta,2)));
	limits->SetFillStyle(0);
	limits->SetLineColor(kBlack);
	limits->SetLineWidth(1);
	limits->SetLineStyle(1);
	limits->Draw("SAME & L");

	TText *t = new TText(	-nSigma*TMath::Sqrt(emit*beta + TMath::Power(eta*delta,2))*scale,
				+nSigma*TMath::Sqrt(emit*gamma + TMath::Power(etaPrime*delta,2))*1.05,Form("n = %d",nSigma));
	t->SetTextFont(42);
	t->SetTextSize(0.03);
	t->Draw();

	return;
}

void GetEllipseValue(Double_t alpha, Double_t beta, Double_t emit, Double_t XX, Int_t sign, Double_t &x, Double_t &xp)
{
	// inverse matrix: norm coord to real coord
	Double_t a11 = TMath::Sqrt(beta);
	Double_t a12 = 0;
	Double_t a21 = -alpha/TMath::Sqrt(beta);
	Double_t a22 = 1./TMath::Sqrt(beta);

	Double_t XXp = (emit >= XX*XX) ? (sign)*TMath::Sqrt(emit - XX*XX) : 0;

	x  = a11*XX + a12*XXp;
	xp = a21*XX + a22*XXp;
	
	return;
}

//==================================================================================================================================================================================//
//==================================================================================================================================================================================//
//==================================================================================================================================================================================//
//==================================================================================================================================================================================//
//==================================================================================================================================================================================//




/*
	gStyle->SetOptStat(0);
	gStyle->SetLegendFont(42);
	gStyle->SetLegendTextSize(0.035);

	TFile *file0 = TFile::Open("output/output_noEcut.root");
	TFile *file1 = TFile::Open("output/output_5keVcut.root");

	TH1F* h0_1 = (TH1F*)file0->Get("h1"); h0_1->SetName("h0_1");
	TH1F* h1_1 = (TH1F*)file1->Get("h1"); h1_1->SetName("h1_1");
	TH2F* h0_2 = (TH2F*)file0->Get("h2"); h0_2->SetName("h0_2");
	TH2F* h1_2 = (TH2F*)file1->Get("h2"); h1_2->SetName("h1_2");

	TCanvas* c1 = new TCanvas("c1","c1",1200,900);
	c1->cd();
	TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
    	pad1->SetBottomMargin(0.1);
    	pad1->SetTopMargin(0.1);
    	pad1->SetRightMargin(0.1);
    	pad1->SetLeftMargin(0.1);
    	pad1->Draw();
    	pad1->cd();
	h0_1->Rebin(10);
	h1_1->Rebin(10);
	h0_1->SetLineWidth(4);
	h1_1->SetLineWidth(1);
	h1_1->SetFillStyle(3345);
	h1_1->SetFillColor(kRed);	
	h0_1->SetLineColor(kBlue);
	h1_1->SetLineColor(kRed);
	h0_1->Draw("hist");
	h1_1->Draw("same & hist");

	gPad->SetLogy();

	TLegend* leg1 = new TLegend(0.4,0.7,0.9,0.9);
	leg1->AddEntry(h0_1,Form("E_{#gamma} > 0 keV [nEntries = %.1e]",h0_1->GetEntries()),"f");
	leg1->AddEntry(h1_1,Form("E_{#gamma} > 5 keV [nEntries = %.1e]",h1_1->GetEntries()),"f");
	leg1->Draw();

	c1->SaveAs("c1.png");

	TCanvas* c2 = new TCanvas("c2","c2",1200,900);
	c2->Divide(1,2);

	c2->cd(1);
	TPad *pad21 = new TPad("pad21","pad21",0,0,1,1);
    	pad21->SetBottomMargin(0.1);
    	pad21->SetTopMargin(0.1);
    	pad21->SetRightMargin(0.1);
    	pad21->SetLeftMargin(0.1);
    	pad21->Draw();
    	pad21->cd();
	h0_2->SetTitle("SR photon position, E_{#gamma} > 0 keV");
	h0_2->Draw("colz");
	gPad->SetLogz();

	c2->cd(2);
	TPad *pad22 = new TPad("pad22","pad22",0,0,1,1);
    	pad22->SetBottomMargin(0.1);
    	pad22->SetTopMargin(0.1);
    	pad22->SetRightMargin(0.1);
    	pad22->SetLeftMargin(0.1);
    	pad22->Draw();
    	pad22->cd();
	h1_2->SetTitle("SR photon position, E_{#gamma} > 5 keV");
	h1_2->Draw("colz");
	h1_2->SetMaximum(h0_2->GetMaximum());
	gPad->SetLogz();

	c2->SaveAs("c2.png");

	return;
}
*/
