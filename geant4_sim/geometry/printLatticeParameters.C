void printSurveyParameters(TString surveyFileName, TString startElementName, TString endElementName);
void printOpticsParameters(TString opticsFileName, TString startElementName, TString endElementName);
void printOpticsParameters_old(TString opticsFileName, TString startElementName, TString endElementName);

void printLatticeParameters()
{
	TString surveyFileName, opticsFileName;
	Bool_t oldLattice;

	//_______________________________ OLD LATTICE ______________________________________//
//	oldLattice = true;

	//- Aug. 2022 - v5.6 lattice
//	surveyFileName = "./lattice_v5.6/esr-survey-elke.tfs";
//	opticsFileName = "./lattice_v5.6/esr-twiss-elke.tfs";

	//- lists of elements (FWD)
//	const Int_t nElements = 5;
//	TString startElementName[nElements] = {	
//		"\"Q2EF_6\"", "\"OWW\"", "\"O2EF_6\"", "\"O1EF_6\"","\"O0EF_6\""};
//	TString stopElementName[nElements] = {
//		"\"OWW\"", "\"D1EF_6\"", "\"Q1EF_6\"", "\"Q0EF_6\"","\"IP6\""};

	//- lists of elements (BWD)
//	const Int_t nElements = 4;
//	TString startElementName[nElements] = {	
//		"\"O2ER_6\"", "\"O1ER_6\"", "\"O0ER_6\"","\"RING$START\""};
//	TString stopElementName[nElements] = {
//		"\"D2ER_6\"", "\"Q2ER_6\"", "\"Q1ER_6\"","\"IP6\""};

	//_______________________________ NEW LATTICE ______________________________________//
	oldLattice = false;

	//- Sep. 2023 - so-called new lattice
	// surveyFileName = "./lattice_v6.X/survey_ir_6m.tfs";
	// opticsFileName = "./lattice_v6.X/ir6forward.18GeV.twi";

	//- Jan. 2024 - v6.2 lattice
	surveyFileName = "./lattice_v6.2/esr-survey-doug.tfs";
	opticsFileName = "./lattice_v6.2/esr-ring-norad.tfs";

	//- lists of elements (FWD)
//	const Int_t nElements = 7;
//	TString startElementName[nElements] = {	
//		"\"SQ2EF_6\"", "\"OWW_SH\"", "\"ODB23\"", "\"ODB23\"", "\"MCOLL_MASK\"", "\"O1EF_6\"","\"O0EF_6\""};
//	TString stopElementName[nElements] = {
//		"\"OWW_SH\"", "\"D2EF_6\"", "\"D2EF_6\"", "\"D1EF_6\"", "\"Q1EF_6\"", "\"Q0EF_6\"","\"IP6\""};

	//- lists of elements (BWD)
	const Int_t nElements = 5;
	TString startElementName[nElements] = {	
		"\"OWW_SH\"","\"O2ER_6\"", "\"O1ER_6\"", "\"O0ER_6\"","\"RING$START\""};
	TString stopElementName[nElements] = {
		"\"Q3ER_6\"","\"D2ER_6\"", "\"Q2ER_6\"", "\"Q1ER_6\"","\"IP6\""};

	//- get parameters
	for(Int_t i = 0; i < nElements; i++)
	{
		cout<<"=============================================================="<<endl;
		cout<<"> element: "<<i<<endl;
		printSurveyParameters(surveyFileName,startElementName[i],stopElementName[i]);
		if(oldLattice)
			printOpticsParameters_old(opticsFileName,startElementName[i],stopElementName[i]);
		else
		printOpticsParameters(opticsFileName,startElementName[i],stopElementName[i]);
		cout<<"=============================================================="<<endl;
	}

	return;
}

void printOpticsParameters(TString opticsFileName, TString startElementName, TString endElementName)
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
				end_iss_IO >> betax;
				end_iss_IO >> alphax;
				wordsToSkip = 1; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> betay;
				end_iss_IO >> alphay;
				wordsToSkip = 7; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> etax;
				end_iss_IO >> etapx;
				end_iss_IO >> etay;
				end_iss_IO >> etapy;
				wordsToSkip = 15; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
				end_iss_IO >> length;	
				wordsToSkip = 3; while (wordsToSkip-- > 0 && end_iss_IO >> word){/* skip */}
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
