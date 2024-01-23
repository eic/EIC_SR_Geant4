void printSurveyParameters(TString surveyFileName, TString startElementName, TString stopElementName);
void printOpticsParameters(TString opticsFileName, TString elementName);

void printLatticeParameters()
{
	TString surveyFileName, opticsFileName;

	surveyFileName = "./lattice_v6.X/survey_ir_6m.tfs";
	opticsFileName = "./lattice_v6.X/ir6forward.18GeV.twi";

	// get parameters
	printSurveyParameters(surveyFileName,"\"SQ2EF_6\"",	"\"OWW_SH\"");
	printOpticsParameters(opticsFileName,			"\"OWW_SH\"");

	printSurveyParameters(surveyFileName,"\"OWW_SH\"",	"\"D2EF_6\"");
	printOpticsParameters(opticsFileName,			"\"D2EF_6\"");

	printSurveyParameters(surveyFileName,"\"ODB23\"",	"\"D2EF_6\"");
	printOpticsParameters(opticsFileName,			"\"D2EF_6\"");

	printSurveyParameters(surveyFileName,"\"ODB23\"",	"\"D1EF_6\"");
	printOpticsParameters(opticsFileName,			"\"D1EF_6\"");

	printSurveyParameters(surveyFileName,"\"MCOLL_MASK\"",	"\"Q1EF_6\"");
	printOpticsParameters(opticsFileName,			"\"Q1EF_6\"");

	printSurveyParameters(surveyFileName,"\"O1EF_6\"",	"\"Q0EF_6\"");
	printOpticsParameters(opticsFileName,			"\"Q0EF_6\"");

	return;
}

void printOpticsParameters(TString opticsFileName, TString elementName)
{
	cout<<"=============================================================="<<endl;
	cout<<"> function: "<<__FUNCTION__<<endl;
	cout<<opticsFileName<<" : "<<elementName<<endl<<endl;

	ifstream opticsFileName_IO(opticsFileName.Data());
	Double_t alphax, alphay, betax, betay, etax, etay, etapx, etapy, k1l;	
	Int_t wordsToSkip;

	if (!opticsFileName_IO.is_open()) 
	{
		cerr << "Unable to open the file: " << opticsFileName << endl;
		exit(-1);
	}

	string line;
	while (getline(opticsFileName_IO, line)) 
	{
		istringstream iss_IO(line);
		string element, word;

		if(iss_IO >> element)
		{
			if(element == elementName)
			{
				// cout<<line<<endl;

				wordsToSkip = 2; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
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
				wordsToSkip = 4; while (wordsToSkip-- > 0 && iss_IO >> word){/* skip */}
				iss_IO >> k1l;
				break;
			}
		}
	}
	opticsFileName_IO.close();

	printf("> betax = %.12f\n",betax);
	printf("> alphax = %.12f\n",alphax);
	printf("> betay = %.12f\n",betay);
	printf("> alphay = %.12f\n",alphay);
	printf("> etax = %.12f\n",etax);
	printf("> etapx = %.12f\n",etapx);
	printf("> etay = %.12f\n",etay);
	printf("> etapy = %.12f\n",etapy);
	printf("> k1l = %.12f\n",k1l);
	cout<<"=============================================================="<<endl;

	return;
}

void printSurveyParameters(TString surveyFileName, TString startElementName, TString endElementName)
{	
	cout<<"=============================================================="<<endl;
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
	cout<<"=============================================================="<<endl;

	return;
}
