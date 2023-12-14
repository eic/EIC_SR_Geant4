/*********************************************************************************************************************/
/*********************************************************************************************************************/
/* Plot beam profiles in XX', YY', and ZP phase-space with SR photon spectrum and distribution                       */
/* One should merge all output ROOT files into one using the 'hadd' command:                                         */
/* hadd -f /path/output_g4ana_all.root /path/output_g4ana/output_ana_*.root                                          */
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void DrawEllipse(Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta, Double_t scale, Int_t nSigma, Color_t color);
void GetEllipseValue(Double_t alpha, Double_t beta, Double_t emit, Double_t XX, Int_t sign, Double_t &x, Double_t &xp);
void DrawPhaseSpace(	TString name, TH2F* hX, 
		Double_t minX, Double_t maxX, Double_t minXp, Double_t maxXp, 
		Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta);
void DrawMomentum(TString name, TH1F* hP, Double_t minP, Double_t maxP, Double_t delta, Double_t gamma, Double_t mass_GeV);
void DrawMagData(TH1F* h1[], TH1F* h2[], Int_t nHist);

using namespace std;

#define NMAGMAX 100 // maximum number of magnets

void plot_tails(TString fName = "output_ana_all.root")
{
	Double_t delta, emitX, emitY, gamma, mass_GeV;

	TString beam_name;
	Double_t beam_minX, beam_maxX, beam_minXp, beam_maxXp;
	Double_t beam_minY, beam_maxY, beam_minYp, beam_maxYp;
	Double_t beam_alphaX, beam_betaX, beam_etaX, beam_etaPrimeX;
	Double_t beam_alphaY, beam_betaY, beam_etaY, beam_etaPrimeY;
	Double_t beam_minP, beam_maxP;

	TString mon_name;
	Double_t mon_minX, mon_maxX, mon_minXp, mon_maxXp;
	Double_t mon_minY, mon_maxY, mon_minYp, mon_maxYp;
	Double_t mon_alphaX, mon_betaX, mon_etaX, mon_etaPrimeX;
	Double_t mon_alphaY, mon_betaY, mon_etaY, mon_etaPrimeY;
	Double_t mon_minP, mon_maxP;

	gStyle->SetOptStat(0);
	gStyle->SetLegendFont(42);
	gStyle->SetLegendTextSize(0.035);

	//------------------------------------------------------------------------------------------------//
	cout<<"[INFO] Input file name: "<<fName<<endl;
	TFile *file = TFile::Open(fName.Data());

	TH1F* h1 = (TH1F*)file->Get("h1"); // SR photon energy
	TH2F* h2 = (TH2F*)file->Get("h2"); // SR photon position
	TH2F* h3x = (TH2F*)file->Get("h3x"); // SR photon vertex position
	TH2F* h3y = (TH2F*)file->Get("h3y"); // SR photon vertex position
	TH2F* h4 = (TH2F*)file->Get("h4"); // Monitor Px vs X
	TH2F* h5 = (TH2F*)file->Get("h5"); // Monitor Py vs Y
	TH1F* h6 = (TH1F*)file->Get("h6"); // Monitor P
	TH2F* h7 = (TH2F*)file->Get("h7"); // Beam Px vs X
	TH2F* h8 = (TH2F*)file->Get("h8"); // Beam Py vs Y
	TH1F* h9 = (TH1F*)file->Get("h9"); // Beam P

	TH1F* h10[NMAGMAX];
	TH1F* h11[NMAGMAX];
	Int_t nMag = 0;
	for(Int_t i = 0; i < NMAGMAX; i++)
		if((TH1F*)file->Get(Form("h10_%d",i)) && (TH1F*)file->Get(Form("h11_%d",i))){nMag++;}
	cout<<"[INFO] nMag = "<<nMag<<endl;
	for(Int_t i = 0; i < nMag; i++)
	{
		h10[i] = (TH1F*)file->Get(Form("h10_%d",i));
		h11[i] = (TH1F*)file->Get(Form("h11_%d",i));
	}
	//------------------------------------------------------------------------------------------------//
	// plot SR photon histograms
	TCanvas* c0_1 = new TCanvas("c0_1","c0_1",900,900);
	c0_1->cd();
	TPad* pad0_1 = new TPad("pad0_1", "pad0_1",0,0,1,1);
    	pad0_1->SetBottomMargin(0.12);
    	pad0_1->SetTopMargin(0.10);
    	pad0_1->SetRightMargin(0.10);
    	pad0_1->SetLeftMargin(0.12);
   	pad0_1->Draw();
	pad0_1->cd();
	gPad->SetGrid();
	h1->SetLineWidth(2);
	h1->SetLineColor(kBlack);
	h1->SetFillColorAlpha(kBlue,0.2);
	h1->Draw("HIST");
	gPad->SetLogy();

	c0_1->SaveAs("./pics/c0_1.png");

	TCanvas* c0_2 = new TCanvas("c0_2","c0_2",2000,2000);
	c0_2->Divide(1,3);

	c0_2->cd(1);
	TPad* pad0_21 = new TPad("pad0_21", "pad0_21",0,0,1,1);
    	pad0_21->SetBottomMargin(0.12);
    	pad0_21->SetTopMargin(0.12);
    	pad0_21->SetRightMargin(0.12);
    	pad0_21->SetLeftMargin(0.12);
   	pad0_21->Draw();
	pad0_21->cd();
	gPad->SetGrid();
	h2->Draw("colz");
	gPad->SetLogz();

	c0_2->cd(2);
	TPad* pad0_22 = new TPad("pad0_22", "pad0_22",0,0,1,1);
    	pad0_22->SetBottomMargin(0.12);
    	pad0_22->SetTopMargin(0.12);
    	pad0_22->SetRightMargin(0.12);
    	pad0_22->SetLeftMargin(0.12);
   	pad0_22->Draw();
	pad0_22->cd();
	gPad->SetGrid();
	h3x->Draw("colz");
	gPad->SetLogz();

	c0_2->cd(3);
	TPad* pad0_23 = new TPad("pad0_23", "pad0_23",0,0,1,1);
    	pad0_23->SetBottomMargin(0.12);
    	pad0_23->SetTopMargin(0.12);
    	pad0_23->SetRightMargin(0.12);
    	pad0_23->SetLeftMargin(0.12);
   	pad0_23->Draw();
	pad0_23->cd();
	gPad->SetGrid();
	h3y->Draw("colz");
	gPad->SetLogz();

	c0_2->SaveAs("./pics/c0_2.png");
	//------------------------------------------------------------------------------------------------//
	//- Parameters
	emitX = 24e-9;	emitY = 2e-9;	delta = 0.109e-2; gamma = 34924.26476; mass_GeV = 0.00051099895;

	// Beam
	beam_name = "beam";
	beam_minX = -4.0;	beam_maxX = 4.0;	beam_minXp = -4.0e-3;	beam_maxXp = 4.0e-3;
	beam_minY = -2.0;	beam_maxY = 2.0;	beam_minYp = -1.0e-3;	beam_maxYp = 1.0e-3;
	beam_alphaX = -7.031713181;	beam_betaX = 161.7921567;	beam_etaX = -0.07618056746;	beam_etaPrimeX = 0.02481186422;
	beam_alphaY = -1.226925635;	beam_betaY = 121.6383193;	beam_etaY = 0.;	beam_etaPrimeY = 0.0;
	beam_minP = 17.5;	beam_maxP = 18.5;

	// D2EF1
//	mon_name = "D2EF1";
//	mon_minX = -1.0;	mon_maxX = 1.0;		mon_minXp = -0.5e-3;	mon_maxXp = 0.5e-3;
//	mon_minY = -0.4;	mon_maxY = 0.4;		mon_minYp = -0.4e-4;	mon_maxYp = 0.4e-4;
//	mon_alphaX = -7.881635149;	mon_betaX = 202.4452538;	mon_etaX = -0.02443133971;	mon_etaPrimeX = 0.01315593225;
//	mon_alphaY = -1.276673748;	mon_betaY = 128.4636335;	mon_etaY = 0.0;	mon_etaPrimeY = 0.0;
//	mon_minP = 17.5;	mon_maxP = 18.5;

	// D2EF2
//	mon_name = "D2EF2";
//	mon_minX = -1.0;	mon_maxX = 1.0;		mon_minXp = -0.5e-3;	mon_maxXp = 0.5e-3;
//	mon_minY = -0.4;	mon_maxY = 0.4;		mon_minYp = -0.4e-4;	mon_maxYp = 0.4e-4;
//	mon_alphaX = -8.812123355;	mon_betaX = 252.2653339;	mon_etaX = -0.00105615001;	mon_etaPrimeX = 0.001500000282;
//	mon_alphaY = -1.33098365;	mon_betaY = 136.2482041;	mon_etaY = 0.0;	mon_etaPrimeY = 0.0;
//	mon_minP = 17.5;	mon_maxP = 18.5;

	// D1EF
//	mon_name = "D1EF";
//	mon_minX = -1.0;	mon_maxX = 1.0;		mon_minXp = -0.5e-3;	mon_maxXp = 0.5e-3;
//	mon_minY = -0.4;	mon_maxY = 0.4;		mon_minYp = -0.4e-4;	mon_maxYp = 0.4e-4;
//	mon_alphaX = -9.170618095;	mon_betaX = 272.9418855;	mon_etaX = 3.629583613e-15;	mon_etaPrimeX = 1.112391429e-16;
//	mon_alphaY =  -1.354020837;	mon_betaY = 139.3355160;	mon_etaY = 0.0;	mon_etaPrimeY = 0.0;
//	mon_minP = 17.5;	mon_maxP = 18.5;

	// Q1EF
//	mon_name = "Q1EF";
//	mon_minX = -2.0;	mon_maxX = 2.0;		mon_minXp = -0.5e-2;	mon_maxXp = 0.5e-2;
//	mon_minY = -0.4;	mon_maxY = 0.4;		mon_minYp = -0.5e-3;	mon_maxYp = 0.5e-3;
//	mon_alphaX = 86.35527052;	mon_betaX = 575.3834696;	mon_etaX = 5.080805448e-15;	mon_etaPrimeX = -7.703104332e-16;
//	mon_alphaY = -39.75962439;	mon_betaY = 255.9452491;	mon_etaY = 0.0;	mon_etaPrimeY = 0.0;
//	mon_minP = 17.5;	mon_maxP = 18.5;

	// Q0EF
	mon_name = "Q0EF";
	mon_minX = -5.0;	mon_maxX = 5.0;		mon_minXp = -1.0e-2;	mon_maxXp = 1.0e-2;
	mon_minY = -5.0;	mon_maxY = 5.0;		mon_minYp = -1.0e-2;	mon_maxYp = 1.0e-2;
	mon_alphaX = 9.830508475;	mon_betaX = 57.60694915;	mon_etaX = 1.563145529e-15;	mon_etaPrimeX = -2.921487199e-16;
	mon_alphaY = 101.754386;	mon_betaY = 590.2324386;	mon_etaY = 0.0;	mon_etaPrimeY = 0.0;
	mon_minP = 17.5;	mon_maxP = 18.5;


	//------------------------------------------------------------------------------------------------//
	//- Draw
	// Beam
	DrawPhaseSpace(beam_name+"X",h7,
		beam_minX,beam_maxX,beam_minXp,beam_maxXp,	// hor (X) range
		emitX,beam_alphaX,beam_betaX,beam_etaX,beam_etaPrimeX,delta); // hor (X) twiss parameters

	DrawPhaseSpace(beam_name+"Y",h8,
		beam_minY,beam_maxY,beam_minYp,beam_maxYp,	// ver (Y) range
		emitY,beam_alphaY,beam_betaY,beam_etaY,beam_etaPrimeY,delta); // ver (Y) twiss parameters

	DrawMomentum(beam_name+"P",h9,beam_minP,beam_maxP,delta,gamma,mass_GeV);

	// Monitor
	DrawPhaseSpace(mon_name+"X",h4,
		mon_minX,mon_maxX,mon_minXp,mon_maxXp,	// hor (X) range
		emitX,mon_alphaX,mon_betaX,mon_etaX,mon_etaPrimeX,delta); // hor (X) twiss parameters

	DrawPhaseSpace(mon_name+"Y",h5,
		mon_minY,mon_maxY,mon_minYp,mon_maxYp,	// ver (Y) range
		emitY,mon_alphaY,mon_betaY,mon_etaY,mon_etaPrimeY,delta); // ver (Y) twiss parameters

	DrawMomentum(mon_name+"P",h6,mon_minP,mon_maxP,delta,gamma,mass_GeV);
	
	// Magnet SR
	cout<<"\n\n=================================================================="<<endl;

	DrawMagData(h10,h11,nMag);

	// calculate the number of radiated SR photons per electron	
	for(Int_t i = 0; i < nMag; i++)
	{
		// number of generated SR photons in a dipole per electron per second: nPh = (alpha*gamma*c/rho)*(5/2/sqrt(3))
		TString title = h10[i]->GetTitle();
		Double_t nPh = -1, t_s = 0, l_m = 0, theta_rad = 0;
		if(title.Contains("D2EF") || title.Contains("D1EF")) // Dipoles
		{
			if	(title.Contains("D2EF1")){l_m = 2.7260154313337; theta_rad = 0.011655839693698;}
			else if (title.Contains("D2EF2")){l_m = 2.7260154313337; theta_rad = 0.011655839693698;}
			else if (title.Contains("D1EF")){l_m = 0.89140008356876; theta_rad = 0.0015;}

			t_s = (theta_rad*l_m/(2.*TMath::Sin(theta_rad/2.)))/TMath::C(); 
			nPh = t_s*(1./137.)*gamma*TMath::C()*5./((l_m/(2.*sin(theta_rad/2)))*2.*sqrt(3.));
		}
		else if(title.Contains("Q1EF") || title.Contains("Q0EF")) // Quads
		{
			if	(title.Contains("Q1EF")){l_m = 1.61;}
			else if	(title.Contains("Q0EF")){l_m = 1.20;}
		
			t_s = l_m/TMath::C();
		}

		printf("[INFO] %s: \t mean = %.2f [phs/event] | stdDev = %.2f [phs/event] <> calc = %.2f [phs/e]\n",h10[i]->GetTitle(),h10[i]->GetMean(),h10[i]->GetStdDev(),nPh);
	}
	cout<<endl;
	// calculate the radiated SR power per electron	
	for(Int_t i = 0; i < nMag; i++)
	{
		// integrated power of generated SR photons per electron: power = (beta^4*c*C_gamma*E^4)/(2*pi*rho^2) [eV/s]
		TString title = h10[i]->GetTitle();
		Double_t P_W = -1, t_s = 0, l_m = 0, theta_rad = 0;
		if(title.Contains("D2EF") || title.Contains("D1EF")) // Dipoles
		{
			if	(title.Contains("D2EF1")){l_m = 2.7260154313337; theta_rad = 0.011655839693698;}
			else if (title.Contains("D2EF2")){l_m = 2.7260154313337; theta_rad = 0.011655839693698;}
			else if (title.Contains("D1EF")){l_m = 0.89140008356876; theta_rad = 0.0015;}

			t_s = (theta_rad*l_m/(2.*TMath::Sin(theta_rad/2.)))/TMath::C(); 
			Double_t hbrac_GeVm = 0.1973269631e-15; // https://pdg.lbl.gov/2010/reviews/rpp2010-rev-phys-constants.pdf
			Double_t r0_m = (1./137.)*hbrac_GeVm/mass_GeV; // classical radius of the particle
			Double_t C_gamma = (4.*TMath::Pi()/3.0)*r0_m/TMath::Power(mass_GeV,3);
			Double_t E0_GeV = gamma*mass_GeV;
			Double_t rho_m = l_m/(2.*TMath::Sin(theta_rad/2.));		

			P_W = (1e9)*TMath::Qe()*(TMath::Power(beta,4)*TMath::C()*C_gamma*TMath::Power(E0_GeV,4))/(2.*TMath::Pi()*TMath::Power(rho_m,2));
		}
		else if(title.Contains("Q1EF") || title.Contains("Q0EF")) // Quads
		{
			if	(title.Contains("Q1EF")){l_m = 1.61;}
			else if	(title.Contains("Q0EF")){l_m = 1.20;}
		
			t_s = l_m/TMath::C();
		}
		
		printf("[INFO] %s: \t mean = %.3f [uW] | stdDev = %.3f [uW] <> calc %.3f [uW]\n",h11[i]->GetTitle(),(1e6)*h11[i]->GetMean()*TMath::Qe()/t_s,(1e6)*h11[i]->GetStdDev()*TMath::Qe()/t_s,P_W*(1e6));
	}
	cout<<"==================================================================\n"<<endl;

	return;
}

void DrawMagData(TH1F* h1[], TH1F* h2[], Int_t nHist)
{
	TCanvas* c1 = new TCanvas("c1_mag","c1_mag",1600,800);
	c1->Divide(nHist,2);

	for(Int_t i = 0; i < nHist; i++)
	{
		c1->cd(i+1);
		h1[i]->Draw();
		h1[i]->SetLineColor(kBlack);
		h1[i]->SetLineWidth(2);
		h1[i]->SetFillColorAlpha(kBlue,0.2);
		h1[i]->GetXaxis()->SetRangeUser(h1[i]->GetMean()-10.0*h1[i]->GetStdDev(),h1[i]->GetMean()+10.0*h1[i]->GetStdDev());

		c1->cd(i+nHist+1);
		h2[i]->Draw();
		h2[i]->SetLineColor(kBlack);
		h2[i]->SetLineWidth(2);
		h2[i]->SetFillColorAlpha(kBlue,0.2);
		h2[i]->Rebin(10);
		gPad->SetLogx();
		gPad->SetLogy();
	}
	c1->SaveAs("./pics/c1_mag.png");

	return;
}

void DrawMomentum(TString name, TH1F* hP, Double_t minP, Double_t maxP, Double_t delta, Double_t gamma, Double_t mass_GeV)
{
	TCanvas* c1 = new TCanvas(Form("c1_%s",name.Data()),Form("c1_%s",name.Data()),1600,1600);
	c1->cd();

	TPad* pad1 = new TPad("pad1", "pad1",0,0.1,1,1);
    	pad1->SetBottomMargin(0.10);
    	pad1->SetTopMargin(0.10);
    	pad1->SetRightMargin(0.10);
    	pad1->SetLeftMargin(0.14);
   	pad1->Draw();
	pad1->cd();
	gPad->SetGrid();

	if(maxP > minP) hP->GetXaxis()->SetRangeUser(minP,maxP);
	hP->SetLineWidth(2);
	hP->SetLineColor(kBlack);
	hP->SetFillColorAlpha(kGreen+2,0.4);
	hP->Draw("HIST");

	c1->cd();
   	TLatex t;
   	t.SetTextFont(42);
   	t.SetTextSize(0.02);

	t.DrawLatex(0.1,0.085,Form("#color[4]{#mu(SIM) = %.3f #pm %.3f [GeV/c]}",hP->GetMean(),hP->GetMeanError()));
	t.DrawLatex(0.1,0.060,Form("#color[2]{#mu(TWI) = %.3f GeV/c \t | #gamma = E/mc^{2} = %f}",mass_GeV*TMath::Sqrt(gamma*gamma - 1.),gamma));
   	t.DrawLatex(0.1,0.035,Form("#color[4]{#sigma(SIM) = %.3e #pm %.3e [GeV/c]}",hP->GetStdDev(),hP->GetStdDevError()));
	t.DrawLatex(0.1,0.010,Form("#color[2]{#sigma(TWI) = %.3e [GeV/c]}",delta*mass_GeV*TMath::Sqrt(gamma*gamma - 1.)));

	c1->SaveAs(Form("./pics/c1_%s.png",name.Data()));

	return;
}

void DrawPhaseSpace(	TString name, TH2F* hX, 
		Double_t minX, Double_t maxX, Double_t minXp, Double_t maxXp, 
		Double_t emit, Double_t alpha, Double_t beta, Double_t eta, Double_t etaPrime, Double_t delta)
{
	//-------------------------------------------------------------------//
	TCanvas* c1 = new TCanvas(Form("c1_%s",name.Data()),Form("c1_%s",name.Data()),1600,1600);
	c1->cd();   

	// central pad
	TPad* pad1 = new TPad("pad1", "pad1",0.0,0.0,0.7,0.7);
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

	DrawEllipse(emit,alpha,beta,eta,etaPrime,delta,1e2 /*[m->cm]*/, 20, kBlack);

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

