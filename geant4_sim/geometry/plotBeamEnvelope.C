void getOpticsParameters(TString opticsFileName, TGraph* gr_betaX, TGraph* gr_betaY, Bool_t oldLattice);

const Float_t IP_posS = 43.301534723896; // m

void plotBeamEnvelope()
{
	const Float_t minS = -80; // m
	const Float_t maxS = +50; // m
	const Float_t emitX = 24.0e-9; // m
	const Float_t emitY = 2.0e-9; // m
	const Float_t max_gr_beamX = 100; // mm
	const Float_t max_gr_beamY = 100; // mm
	const Float_t nSigmaX = 15.0; 
	const Float_t nSigmaY = 22.0;
	cout<<"--> nSigmaX = "<<nSigmaX<<endl;
	cout<<"--> nSigmaY = "<<nSigmaY<<endl;

	TString surveyFileName, opticsFileName;
	Bool_t oldLattice;
	TGraph* gr_betaX = new TGraph();
	TGraph* gr_betaY = new TGraph();

	//_______________________________ OLD LATTICE ______________________________________//
	// oldLattice = true;

	//- Aug. 2022 - v5.6 lattice
	// opticsFileName = "./lattice_v5.6/esr-twiss-elke.tfs";

	//_______________________________ NEW LATTICE ______________________________________//
	oldLattice = false;

	//- Sep. 2023 - so-called new lattice
	// opticsFileName = "./lattice_v6.X/ir6forward.18GeV.twi";

	//- Jan. 2024 - v6.2 lattice
	// opticsFileName = "./lattice_v6.2/esr-ring-norad.tfs";

	//- May. 2024 - v6.3 lattice
	// opticsFileName = "./lattice_v6.3/ir6twiss.tfs";

	//- Jul. 2024 - v6.3 lattice - modified BWD beam line for low-Q^2 taggers
	opticsFileName = "./lattice_v6.3_modif_bwd/ir6twiss.tfs";

	//__________________________________________________________________________________//

	//- get parameters
	getOpticsParameters(opticsFileName,gr_betaX,gr_betaY,oldLattice);

	//- draw parameters
	TCanvas* c1 = new TCanvas("c1","c1",1200,900);
	c1->Divide(1,2);

	c1->cd(1);
	// X
	gr_betaX->SetLineColor(kBlue);
	gr_betaX->SetLineWidth(2);
	gr_betaX->SetMarkerStyle(24);
	gr_betaX->SetMarkerSize(0.7);
	gr_betaX->GetXaxis()->SetLimits(minS,maxS);
	gr_betaX->GetXaxis()->SetTitle("S [m]");
	gr_betaX->GetYaxis()->SetTitle("#beta [m]");
	gr_betaX->SetTitle("ESR v6.3 18 GeV");
	gr_betaX->GetXaxis()->CenterTitle(1);
	gr_betaX->GetYaxis()->CenterTitle(1);
	gr_betaX->GetXaxis()->SetTitleSize(0.05);
	gr_betaX->GetYaxis()->SetTitleSize(0.05);
	gr_betaX->GetXaxis()->SetLabelSize(0.05);
	gr_betaX->GetYaxis()->SetLabelSize(0.05);
	gr_betaX->GetXaxis()->SetTitleOffset(0.8);
	gr_betaX->GetYaxis()->SetTitleOffset(0.8);
	gr_betaX->Draw("APL");
	// Y
	gr_betaY->SetLineColor(kRed);
	gr_betaY->SetLineStyle(9);
	gr_betaY->SetLineWidth(2);
	gr_betaY->SetMarkerStyle(21);
	gr_betaY->SetMarkerSize(0.5);
	gr_betaY->Draw("SAME & PL");

	TLegend* leg1 = new TLegend(0.15,0.70,0.35,0.87);
	leg1->AddEntry(gr_betaX,"Horizontal (#beta_{X})");
	leg1->AddEntry(gr_betaY,"Vertical (#beta_{Y})");
	leg1->SetTextSize(0.05);
	leg1->Draw();

	gPad->SetGrid();

	//- get beam envelope
	TGraph* gr_beamX1 = new TGraph();
	TGraph* gr_beamX2 = new TGraph();
	TGraph* gr_beamY1 = new TGraph();
	TGraph* gr_beamY2 = new TGraph();

	for(Int_t i = 0; i < gr_betaX->GetN(); i++)
	{
		Double_t s, betaX, betaY;
		gr_betaX->GetPoint(i,s,betaX);
		betaY = gr_betaY->GetPointY(i);
		gr_beamX1->SetPoint(i,s,+nSigmaX*TMath::Sqrt(betaX*emitX)*1000);
		gr_beamX2->SetPoint(i,s,-nSigmaX*TMath::Sqrt(betaX*emitX)*1000);
		gr_beamY1->SetPoint(i,s,+nSigmaY*TMath::Sqrt(betaY*emitY)*1000);
		gr_beamY2->SetPoint(i,s,-nSigmaY*TMath::Sqrt(betaY*emitY)*1000);
	}

	//- draw envelope
	c1->cd(2);
	// X
	gr_beamX1->SetLineColor(kBlue);
	gr_beamX2->SetLineColor(kBlue);
	gr_beamX1->SetLineWidth(2);
	gr_beamX2->SetLineWidth(2);
	gr_beamX1->SetMarkerStyle(24);
	gr_beamX2->SetMarkerStyle(24);
	gr_beamX1->SetMarkerSize(0.7);
	gr_beamX2->SetMarkerSize(0.7);
	gr_beamX1->GetXaxis()->SetLimits(minS,maxS);
	gr_beamX1->SetMaximum(+1.2*max_gr_beamX);
	gr_beamX1->SetMinimum(-1.2*max_gr_beamX);
	gr_beamX1->GetXaxis()->SetTitle("S [m]");
	gr_beamX1->GetYaxis()->SetTitle("Aperture/Beam envelope [mm]");
	gr_beamX1->SetTitle(Form("#varepsilon_{X/Y} = %.f/%.f nm",emitX*1e9,emitY*1e9));
	gr_beamX1->GetXaxis()->CenterTitle(1);
	gr_beamX1->GetYaxis()->CenterTitle(1);
	gr_beamX1->GetXaxis()->SetTitleSize(0.05);
	gr_beamX1->GetYaxis()->SetTitleSize(0.05);
	gr_beamX1->GetXaxis()->SetLabelSize(0.05);
	gr_beamX1->GetYaxis()->SetLabelSize(0.05);
	gr_beamX1->GetXaxis()->SetTitleOffset(0.8);
	gr_beamX1->GetYaxis()->SetTitleOffset(0.8);
	gr_beamX1->Draw("APL");
	gr_beamX2->Draw("SAME & PL");
	// Y
	gr_beamY1->SetLineColor(kRed);
	gr_beamY2->SetLineColor(kRed);
	gr_beamY1->SetLineStyle(9);
	gr_beamY2->SetLineStyle(9);
	gr_beamY1->SetLineWidth(2);
	gr_beamY2->SetLineWidth(2);
	gr_beamY1->SetMarkerStyle(21);
	gr_beamY2->SetMarkerStyle(21);
	gr_beamY1->SetMarkerSize(0.5);
	gr_beamY2->SetMarkerSize(0.5);
	gr_beamY1->Draw("SAME & PL");
	gr_beamY2->Draw("SAME & PL");

	gPad->SetGrid();

	//- draw beam pipe
	const Int_t nAp = 23;
	//- original aperture
//	Double_t ss[] = {-15.270,-11.414,-11.364,-9.364,-9.2880,-7.4050,-7.3420,-0.800, 0.670, 0.670, 0.755, 0.755, 4.300, 4.450, 4.460, 4.660, 7.200, 10.50, 23.00, 23.50, 33.50}; // m
//	Double_t dx[] = { 0.1960, 0.1960, 0.1540, 0.154, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.044, 0.044, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m

	//- original aperture + narrower B2eR beam pipe
//	Double_t ss[] = {-70.0000,-16.0000,-15.0000,-11.4140,-11.3640,-9.3640,-9.2880,-9.2800,-7.4050,-7.3420,-0.800, 0.670, 0.670, 0.755, 0.755, 4.300, 4.450, 4.460, 4.660, 7.200, 10.50, 23.00, 23.50, 40.00}; // m
//	Double_t dx[] = {  0.1000,  0.1000,  0.1960,  0.1960,  0.1540, 0.1540, 0.1286, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.044, 0.044, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m
//	Double_t dy[] = {  0.1000,  0.1000,  0.1960,  0.1960,  0.1540, 0.1540, 0.1286, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.044, 0.044, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m

	//- srmask5
//	Double_t ss[] = {-15.270,-11.414,-11.364,-9.364,-9.2880,-7.4050,-7.3420,-0.800, 0.670, 0.670, 0.755, 0.755, 4.300, 4.453, 4.660, 7.200, 10.50, 23.00, 23.50, 33.50}; // m
//	Double_t dx[] = { 0.1960, 0.1960, 0.1540, 0.154, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.043, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m

	//- srmask5 + narrower B2eR beam pipe
	Double_t ss[] = {-70.0000,-16.0000,-15.0000,-11.4140,-11.3640,-9.3640,-9.2880,-9.2800,-7.4050,-7.3420,-0.800, 0.670, 0.670, 0.755, 0.755, 4.300, 4.453, 4.660, 7.200, 10.50, 23.00, 23.50, 40.00}; // m
	Double_t dx[] = {  0.1000,  0.1000,  0.1960,  0.1960,  0.1540, 0.1540, 0.1286, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.043, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m
	Double_t dy[] = {  0.1000,  0.1000,  0.1960,  0.1960,  0.1540, 0.1540, 0.1286, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.043, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m

	//- srmask5 + narrower B2eR beam pipe + Q0eF narrower (from Holger)
//	Double_t ss[] = {-70.0000,-16.0000,-15.0000,-11.4140,-11.3640,-9.3640,-9.2880,-9.2800,-7.4050,-7.3420,-0.800, 0.670, 0.670, 0.755, 0.755, 4.300, 4.453, 4.660, 7.200, 10.50, 23.00, 23.50, 40.00}; // m
//	Double_t dx[] = {  0.1000,  0.1000,  0.1960,  0.1960,  0.1540, 0.1540, 0.1286, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.043, 0.050, 0.050, 0.126, 0.126, 0.100, 0.100}; // m
//	Double_t dy[] = {  0.1000,  0.1000,  0.1960,  0.1960,  0.1540, 0.1540, 0.1286, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.043, 0.050, 0.050, 0.126, 0.126, 0.100, 0.100}; // m

	TGraph* gr_rx1 = new TGraph();
	TGraph* gr_rx2 = new TGraph();
	for(Int_t i = 0; i < nAp; i++)
	{
		gr_rx1->SetPoint(i,ss[i],+dx[i]/2*1000);
		gr_rx2->SetPoint(i,ss[i],-dx[i]/2*1000);
	}
	gr_rx1->SetLineWidth(2);
	gr_rx2->SetLineWidth(2);
	gr_rx1->Draw("SAME & L");
	gr_rx2->Draw("SAME & L");

	TLegend* leg2 = new TLegend(0.15,0.70,0.35,0.87);
	leg2->AddEntry(gr_beamX1,Form("Horizontal (%.1f #sigma_{X})",nSigmaX));
	leg2->AddEntry(gr_beamY1,Form("Vertical (%.1f #sigma_{Y})",nSigmaY));
	leg2->AddEntry(gr_rx1,"Beam pipe aperture");
	leg2->SetTextSize(0.05);
	leg2->Draw();

	//- get minimum aperture
	Double_t nSigmaMinX = 1e9, nSigmaMinXs = 0;
	Double_t nSigmaMinY = 1e9, nSigmaMinYs = 0;
	for(Int_t i = 0; i < nAp; i++)
	{
		Double_t nSigmaX = (dx[i]/2.)/TMath::Sqrt(emitX * gr_betaX->Eval(ss[i]));
		Double_t nSigmaY = (dy[i]/2.)/TMath::Sqrt(emitY * gr_betaY->Eval(ss[i]));
		if(nSigmaX < nSigmaMinX)	{nSigmaMinX = nSigmaX; nSigmaMinXs = ss[i];}
		if(nSigmaY < nSigmaMinY)	{nSigmaMinY = nSigmaY; nSigmaMinYs = ss[i];}
	}
	cout<<"--> nSigmaMinX = "<<nSigmaMinX<<" at s = "<<nSigmaMinXs<<" [m]"<<endl;
	cout<<"--> nSigmaMinY = "<<nSigmaMinY<<" at s = "<<nSigmaMinYs<<" [m]"<<endl;

	return;
}

void getOpticsParameters(TString opticsFileName, TGraph* gr_betaX, TGraph* gr_betaY, Bool_t oldLattice)
{
	cout<<endl<<"> function: "<<__FUNCTION__<<endl;
	cout<<"--> file name: "<<opticsFileName<<endl;

	ifstream opticsFileName_IO(opticsFileName.Data());
	Double_t alphax, alphay, betax, betay, etax, etay, etapx, etapy, k1l, s, length;	
	Int_t wordsToSkip, linesToSkip;

	if (!opticsFileName_IO.is_open()) 
	{
		cerr << "Unable to open the file: " << opticsFileName << endl;
		exit(-1);
	}

	Float_t maxS = 0, circ = 3834.013;
	string line;
	getline(opticsFileName_IO, line);
	linesToSkip = 52; while (linesToSkip-- > 0 && getline(opticsFileName_IO, line)){/* skip */}
	
	while (getline(opticsFileName_IO, line)) 
	{
		istringstream iss_IO(line);
		string element, word;

		if(iss_IO >> element)
		{
			if(oldLattice)
			{
				wordsToSkip = 1; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> s;
				iss_IO >> length;	
				iss_IO >> betax;
				iss_IO >> alphax;
				iss_IO >> betay;
				iss_IO >> alphay;
				wordsToSkip = 2; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> etax;
				iss_IO >> etapx;
				etay = 0;
				etapy = 0;
				wordsToSkip = 2; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> k1l;
			}
			else
			{
				wordsToSkip = 1; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> s;
				iss_IO >> betax;
				iss_IO >> alphax;
				wordsToSkip = 1; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> betay;
				iss_IO >> alphay;
				wordsToSkip = 7; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> etax;
				iss_IO >> etapx;
				iss_IO >> etay;
				iss_IO >> etapy;
				wordsToSkip = 15; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> length;	
				wordsToSkip = 3; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> k1l;
			}

			if(maxS < s) maxS = s;
			if(s < circ/2) s *= -1;
			else s = circ - s;
			gr_betaX->SetPoint(gr_betaX->GetN(),s+IP_posS,betax);
			gr_betaY->SetPoint(gr_betaY->GetN(),s+IP_posS,betay);
		}
	}
	opticsFileName_IO.close();
	gr_betaX->Sort();
	gr_betaY->Sort();
	cout<<"--> maxS = "<<maxS<<" [m]"<<endl;

	return;
}
