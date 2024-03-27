void getOpticsParameters(TString opticsFileName, TGraph* gr_betaX, TGraph* gr_betaY);

void plotBeamEnvelope()
{
	const Float_t minS = -15; // m
	const Float_t maxS = +33; // m
	const Float_t emitX = 24.0e-9; // m
	const Float_t emitY = 2.0e-9; // m
	const Float_t max_gr_beamX = 100; // mm
	const Float_t max_gr_beamY = 100; // mm
	const Float_t nSigmaX = 15; 
	const Float_t nSigmaY = 10;
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
	opticsFileName = "./lattice_v6.2/esr-ring-norad.tfs";

	//- get parameters
	getOpticsParameters(opticsFileName,gr_betaX,gr_betaY);

	//- draw parameters
	TCanvas* c1 = new TCanvas("c1","c1",1200,900);
	c1->Divide(1,2);

	c1->cd(1);
	gr_betaX->SetLineColor(kBlue);
	gr_betaX->SetLineWidth(2);
	gr_betaX->SetMarkerStyle(20);
	gr_betaX->GetXaxis()->SetLimits(minS,maxS);
	gr_betaX->Draw("APL");

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
	gr_beamX1->SetLineColor(kBlue);
	gr_beamX2->SetLineColor(kBlue);
	gr_beamX1->SetLineWidth(2);
	gr_beamX2->SetLineWidth(2);
	gr_beamX1->SetMarkerStyle(20);
	gr_beamX2->SetMarkerStyle(20);
	gr_beamX1->GetXaxis()->SetLimits(minS,maxS);
	gr_beamX2->GetXaxis()->SetLimits(minS,maxS);
	gr_beamX1->SetMaximum(+1.2*max_gr_beamX);
	gr_beamX1->SetMinimum(-1.2*max_gr_beamX);
	gr_beamX1->Draw("APL");
	gr_beamX2->Draw("SAME & PL");

	//- draw beam pipe
	const Int_t nAp = 21;
	Double_t ss[] = {-15.270,-11.414,-11.364,-9.364,-9.2880,-7.4050,-7.3420,-0.800, 0.670, 0.670, 0.755, 0.755, 4.300, 4.450, 4.460, 4.660, 7.200, 10.50, 23.00, 23.50, 33.50}; // m
	Double_t dx[] = { 0.1960, 0.1960, 0.1540, 0.154, 0.1286, 0.1286, 0.1114, 0.062, 0.062, 0.060, 0.060, 0.062, 0.062, 0.044, 0.044, 0.054, 0.054, 0.126, 0.126, 0.100, 0.100}; // m
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

	//- get minimum aperture
	Double_t nSigmaMinX = 1e9, nSigmaMinXs = 0;
	for(Int_t i = 0; i < nAp; i++)
	{
		Double_t nSigma = (dx[i]/2.)/TMath::Sqrt(emitX * gr_betaX->Eval(ss[i]));
		if(nSigma < nSigmaMinX)	{nSigmaMinX = nSigma; nSigmaMinXs = ss[i];}
	}
	cout<<"--> nSigmaMinX = "<<nSigmaMinX<<" at s = "<<nSigmaMinXs<<" [m]"<<endl;

	return;
}

void getOpticsParameters(TString opticsFileName, TGraph* gr_betaX, TGraph* gr_betaY)
{
	cout<<endl<<"> function: "<<__FUNCTION__<<endl;
	cout<<"--> file name: "<<opticsFileName<<endl;

	ifstream opticsFileName_IO(opticsFileName.Data());
	Double_t alphax, alphay, betax, betay, etax, etay, etapx, etapy, k1l, s, length;	
	Int_t wordsToSkip;

	if (!opticsFileName_IO.is_open()) 
	{
		cerr << "Unable to open the file: " << opticsFileName << endl;
		exit(-1);
	}

	Float_t maxS = 0, circ = 3834.0;
	string line;
	getline(opticsFileName_IO, line);
	while (getline(opticsFileName_IO, line)) 
	{
		istringstream iss_IO(line);
		string element, word;

		if(iss_IO >> element)
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
	
			if(maxS < s) maxS = s;
			if(s < circ/2) s *= -1;
			else s = circ - s;
			gr_betaX->SetPoint(gr_betaX->GetN(),s,betax);
			gr_betaY->SetPoint(gr_betaY->GetN(),s,betay);
		}
	}
	opticsFileName_IO.close();
	gr_betaX->Sort();
	gr_betaY->Sort();
	cout<<"--> maxS = "<<maxS<<" [m]"<<endl;

	return;
}

void printOpticsParameters_old(TString opticsFileName, TString startElementName, TString endElementName)
{
	cout<<endl<<"> function: "<<__FUNCTION__<<endl;
	cout<<opticsFileName<<" : "<<startElementName<<" - "<<endElementName<<endl<<endl;

	ifstream opticsFileName_IO(opticsFileName.Data());
	Double_t alphax, alphay, betax, betay, etax, etay, etapx, etapy, k1l, s, length;	
	Int_t wordsToSkip;

	if (!opticsFileName_IO.is_open()) 
	{
		cerr << "Unable to open the file: " << opticsFileName << endl;
		exit(-1);
	}

	string start_line, end_line;
	getline(opticsFileName_IO, start_line);
	while (getline(opticsFileName_IO, end_line)) 
	{
		istringstream start_iss_IO(start_line);
		istringstream end_iss_IO(end_line);
		string start_element, end_element, word;

		if(start_iss_IO >> start_element && end_iss_IO >> end_element)
		{
			if(start_element == startElementName && end_element == endElementName)
			{
				// cout<<start_line<<endl;
				// cout<<end_line<<endl;

				wordsToSkip = 1; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> s;
				end_iss_IO >> length;	
				end_iss_IO >> betax;
				end_iss_IO >> alphax;
				end_iss_IO >> betay;
				end_iss_IO >> alphay;
				wordsToSkip = 2; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> etax;
				end_iss_IO >> etapx;
				etay = 0;
				etapy = 0;
				wordsToSkip = 2; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> k1l;
				break;
			}
		}
		start_line = end_line;
	}
	opticsFileName_IO.close();

	printf("> s = %.12f\n",s);
	printf("> length = %.12f\n",length);
	printf("> betax = %.12f\n",betax);
	printf("> alphax = %.12f\n",alphax);
	printf("> betay = %.12f\n",betay);
	printf("> alphay = %.12f\n",alphay);
	printf("> etax = %.12e\n",etax);
	printf("> etapx = %.12e\n",etapx);
	printf("> etay = %.12e\n",etay);
	printf("> etapy = %.12e\n",etapy);
	printf("> k1l = %.12f\n",k1l);

	return;
}

void printSurveyParameters(TString surveyFileName, TString startElementName, TString endElementName)
{	
	cout<<"> function: "<<__FUNCTION__<<endl;
	cout<<surveyFileName<<" : "<<startElementName<<" - "<<endElementName<<endl<<endl;

	ifstream surveyFileName_IO(surveyFileName.Data());
	Double_t start_posX, start_posY, start_posZ;	
	Double_t end_posX, end_posY, end_posZ, end_theta, end_length, end_angle;	
	Int_t wordsToSkip;

	if (!surveyFileName_IO.is_open()) 
	{
		cerr << "Unable to open the file: " << surveyFileName << endl;
		exit(-1);
	}

	string start_line, end_line;
	getline(surveyFileName_IO, start_line);
	while (getline(surveyFileName_IO, end_line)) 
	{
		istringstream start_iss_IO(start_line);
		istringstream end_iss_IO(end_line);
		string start_element, end_element, word;

		if(start_iss_IO >> start_element && end_iss_IO >> end_element)
		{
			if(start_element == startElementName && end_element == endElementName)
			{
				// cout<<start_line<<endl;
				// cout<<end_line<<endl;

				wordsToSkip = 4; while (wordsToSkip-- > 0 && start_iss_IO >> word){/* skip */}
				start_iss_IO >> start_posX;
				start_iss_IO >> start_posY;
				start_iss_IO >> start_posZ;

				wordsToSkip = 2; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> end_length;
				end_iss_IO >> end_angle;
				end_iss_IO >> end_posX;
				end_iss_IO >> end_posY;
				end_iss_IO >> end_posZ;
				end_iss_IO >> end_theta;
				break;
			}
		}
		start_line = end_line;
	}
	surveyFileName_IO.close();

	printf("> posx1 = %.12f\n",start_posX);
	printf("> posy1 = %.12f\n",start_posY);
	printf("> posz1 = %.12f\n",start_posZ);
	printf("> posx2 = %.12f\n",end_posX);
	printf("> posy2 = %.12f\n",end_posY);
	printf("> posz2 = %.12f\n",end_posZ);
	printf("> theta = %.12f\n",end_theta);
	printf("> length = %.12f\n",end_length);
	printf("> angle = %.12f\n",end_angle);

	return;
}
