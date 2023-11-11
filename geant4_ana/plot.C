Double_t GetEllipseValue(Double_t x, Int_t sign);
void CalculateEllipseCoord(Double_t alpha, Double_t beta, Double_t gamma, Double_t emit);
void CalculateEllipseCooef(Double_t x1, Double_t x2, Double_t x3, Double_t y1, Double_t y2, Double_t y3);
void GetEllipseValue1(Double_t alpha, Double_t beta, Double_t emit, Double_t XX, Int_t sign, Double_t &x, Double_t &xp);

Double_t _A, _B, _C, _orient;

using namespace std;

void plot()
{
	Double_t emit_hor = 24.0e-9; // [m]
	Double_t alpha_hor = 9.830508475; // []
	Double_t beta_hor = 57.60694915; // [m]
	Double_t gamma_hor = (1. + alpha_hor*alpha_hor)/beta_hor;
	Double_t tan_2phi_hor = 2.*alpha_hor/(gamma_hor-beta_hor);
	
	Double_t emit_ver = 2.0e-9; // [m]
	Double_t alpha_ver = 101.754386; // []
	Double_t beta_ver = 590.2324386; // [m]
	Double_t gamma_ver = (1. + alpha_ver*alpha_ver)/beta_ver;
	Double_t tan_2phi_ver = 2.*alpha_ver/(gamma_ver-beta_ver);

	TGraph* ellipse1_hor = new TGraph();
	TGraph* ellipse1_ver = new TGraph();

	TGraph* ellipse2_hor = new TGraph();
	TGraph* ellipse2_ver = new TGraph();

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// Method 1: based on normalised coordinates, no dispersion
	Double_t x, xp, y, yp;
	// horizontal
	for(Double_t XX = -TMath::Sqrt(emit_hor); XX <= TMath::Sqrt(emit_hor); XX += TMath::Sqrt(emit_hor)/100000.0)
	{
		GetEllipseValue1(alpha_hor,beta_hor,emit_hor,XX,+1,x,xp);
		ellipse1_hor->SetPoint(ellipse1_hor->GetN(),x,xp);
	}
	for(Double_t XX = TMath::Sqrt(emit_hor); XX >= -TMath::Sqrt(emit_hor); XX -= TMath::Sqrt(emit_hor)/100000.0)
	{
		GetEllipseValue1(alpha_hor,beta_hor,emit_hor,XX,-1,x,xp);
		ellipse1_hor->SetPoint(ellipse1_hor->GetN(),x,xp);
	}
	// vertical
	for(Double_t YY = -TMath::Sqrt(emit_ver); YY <= TMath::Sqrt(emit_ver); YY += TMath::Sqrt(emit_ver)/100000.0)
	{
		GetEllipseValue1(alpha_ver,beta_ver,emit_ver,YY,+1,y,yp);
		ellipse1_ver->SetPoint(ellipse1_ver->GetN(),y,yp);
	}
	for(Double_t YY = TMath::Sqrt(emit_ver); YY >= -TMath::Sqrt(emit_ver); YY -= TMath::Sqrt(emit_ver)/100000.0)
	{
		GetEllipseValue1(alpha_ver,beta_ver,emit_ver,YY,-1,y,yp);
		ellipse1_ver->SetPoint(ellipse1_ver->GetN(),y,yp);
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// Method 2: based on the general ellipse equation: Ax^2+Bxy+Cy^2=1, no dispersion
	// horizontal
	_orient = tan_2phi_hor/TMath::Abs(tan_2phi_hor);
	CalculateEllipseCoord(alpha_hor,beta_hor,gamma_hor,emit_hor);
	for(Double_t x = -TMath::Sqrt(emit_hor*beta_hor); x <= TMath::Sqrt(emit_hor*beta_hor); x += TMath::Sqrt(emit_hor*beta_hor)/100000.0)	
		ellipse2_hor->SetPoint(ellipse2_hor->GetN(),x,GetEllipseValue(x,+1));
	for(Double_t x = TMath::Sqrt(emit_hor*beta_hor); x >=- TMath::Sqrt(emit_hor*beta_hor); x -= TMath::Sqrt(emit_hor*beta_hor)/100000.0)	
		ellipse2_hor->SetPoint(ellipse2_hor->GetN(),x,GetEllipseValue(x,-1));
	// vertical
	_orient = tan_2phi_ver/TMath::Abs(tan_2phi_ver);
	CalculateEllipseCoord(alpha_ver,beta_ver,gamma_ver,emit_ver);
	for(Double_t x = -TMath::Sqrt(emit_ver*beta_ver); x <= TMath::Sqrt(emit_ver*beta_ver); x += TMath::Sqrt(emit_ver*beta_ver)/100000.0)	
		ellipse2_ver->SetPoint(ellipse2_ver->GetN(),x,GetEllipseValue(x,+1));
	for(Double_t x = TMath::Sqrt(emit_ver*beta_ver); x >=- TMath::Sqrt(emit_ver*beta_ver); x -= TMath::Sqrt(emit_ver*beta_ver)/100000.0)	
		ellipse2_ver->SetPoint(ellipse2_ver->GetN(),x,GetEllipseValue(x,-1));
	//-------------------------------------------------------------------------------------------------------------------------------------------//

	TCanvas* c_hor = new TCanvas("c_hor","c_hor",900,900);
	c_hor->cd();
	ellipse2_hor->SetLineWidth(10);
	ellipse2_hor->Draw("AL");
	ellipse1_hor->SetLineColor(kGreen);
	ellipse1_hor->SetLineWidth(2);
	ellipse1_hor->Draw("SAME & L");

	TCanvas* c_ver = new TCanvas("c_ver","c_ver",900,900);
	c_ver->cd();
	ellipse2_ver->SetLineWidth(10);
	ellipse2_ver->Draw("AL");
	ellipse1_ver->SetLineColor(kGreen);
	ellipse1_ver->SetLineWidth(2);
	ellipse1_ver->Draw("SAME & L");

	return;
}


void GetEllipseValue1(Double_t alpha, Double_t beta, Double_t emit, Double_t XX, Int_t sign, Double_t &x, Double_t &xp)
{
	// inverse matrix: norm coord to real coord
	Double_t a11 = TMath::Sqrt(beta);
	Double_t a12 = 0;
	Double_t a21 = -alpha/TMath::Sqrt(beta);
	Double_t a22 = 1./TMath::Sqrt(beta);

	Double_t XXp = (sign)*TMath::Sqrt(emit - XX*XX);

	x  = a11*XX + a12*XXp;
	xp = a21*XX + a22*XXp;
	
	return;
}


Double_t GetEllipseValue(Double_t x, Int_t sign)
{
	Double_t D = (x*x)*(_B*_B - 4.*_C*_A) + 4.*_C;
	if(D < 0) {D = 0;}
	return (-_B*x + (sign)*TMath::Sqrt(D))/(2.*_C);
}

void CalculateEllipseCoord(Double_t alpha, Double_t beta, Double_t gamma, Double_t emit)
{
	Double_t x1 = (_orient) * alpha*TMath::Sqrt(emit/gamma);
	Double_t x2 = (_orient) * TMath::Sqrt(emit*beta);
	Double_t x3 = (_orient) * TMath::Sqrt(emit/gamma);

	Double_t y1 = TMath::Sqrt(emit*gamma);
	Double_t y2 = alpha*TMath::Sqrt(emit/beta);
	Double_t y3 = 0;

	CalculateEllipseCooef(x1,x2,x3,y1,y2,y3);

	return;
}

void CalculateEllipseCooef(Double_t x1, Double_t x2, Double_t x3, Double_t y1, Double_t y2, Double_t y3)
{
	if(x1 == 0 || x2 == 0 || x3 == 0)
	{
		cout<<"[WARNING] CalculateEllipse ==> Bad input data: x[i] <> 0"<<endl;
		exit(-1);
	}

	Double_t k1 = 1./(x1*x1) - 1./(x2*x2);
	Double_t k2 = 1./(x1*x1) - 1./(x3*x3);
	Double_t k3 = 1./(y1/x1 - y2/x2);
	Double_t k4 = 1./(y1/x1 - y3/x3);
	Double_t k5 = (y1*y1)/(x1*x1) - (y2*y2)/(x2*x2);
	Double_t k6 = (y1*y1)/(x1*x1) - (y3*y3)/(x3*x3);

	_C = (k1*k3 - k2*k4)/(k5*k3 - k6*k4);
	_B = (k1 - _C*k5)*k3;
	_A = 1/(x1*x1) - _B*y1/x1 - _C*(y1*y1)/(x1*x1);

	return;
}




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
