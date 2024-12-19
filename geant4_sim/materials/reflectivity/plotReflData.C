void PrintReflData(	
	string name, 
	vector<pair<double, 
	vector<double>>> dataset,
	vector<double> angle, 
	vector<double> energy);
vector<pair<double, vector<double>>> LoadReflData(
	string filename,
	vector<double> &angle,
	vector<double> &energy);


void plotReflData()
{
	map<string, vector<pair<double, vector<double>>>> reflectDataMap;
	map<string, vector<double>> grazingAngleMap;
	map<string, vector<double>> photonEnergyMap;

	const Int_t nFiles = 2;
	// TString fName[nFiles] = {"Cu_30000.0nmCu.csv","../02-Copper.csv"};
	// TString fName[nFiles] = {"Cu_30000.0nmCu.csv","Cu_4000000.0nmCu.csv"};
	TString fName[nFiles] = {"Cu_1000.0nmTiZrV.csv","../02-Copper.csv"};
	Int_t nGraphs[nFiles];

	TGraph* grReflData[nFiles][100];

	for(size_t i = 0; i < nFiles; i++)
	{
		reflectDataMap[fName[i].Data()] = LoadReflData(
			fName[i].Data(),
			grazingAngleMap[fName[i].Data()],
			photonEnergyMap[fName[i].Data()]);

		PrintReflData(
			fName[i].Data(),
			reflectDataMap[fName[i].Data()],
			grazingAngleMap[fName[i].Data()],
			photonEnergyMap[fName[i].Data()]);

		nGraphs[i] = 0;
		for(size_t ii = 0; ii < reflectDataMap[fName[i].Data()].size(); ii++)
		{
			grReflData[i][ii] = new TGraph();
			grReflData[i][ii]->SetName(Form("%zu: %.3f [rad]",i,grazingAngleMap[fName[i].Data()].at(ii)));
			nGraphs[i]++;

			for(size_t jj = 0; jj < reflectDataMap[fName[i].Data()].at(ii).second.size(); jj++)
			{
				grReflData[i][ii]->SetPoint(
					grReflData[i][ii]->GetN(),
					photonEnergyMap[fName[i].Data()].at(jj),
					reflectDataMap[fName[i].Data()].at(ii).second.at(jj));
			}
		}
	}

	TCanvas* cc = new TCanvas("cc","cc",1400,900);
	cc->cd();
	TPad *pad = new TPad("pad","pad",0,0,1,1);
        pad->SetBottomMargin(0.12);
        pad->SetTopMargin(0.12);
        pad->SetRightMargin(0.16);
        pad->SetLeftMargin(0.12);
        pad->Draw();
        pad->cd();

	TString title;
	TLegend* leg[nFiles];
	
	for(size_t i = 0; i < nFiles; i++)
	{
		leg[i] = new TLegend(0.85,i*1./nFiles,1.0,(i+1)*1./nFiles);

		title += Form("(%zu: %s) ",i,fName[i].Data());
		for(size_t ii = 0; ii < nGraphs[i]; ii++)
		{
			grReflData[i][ii]->SetMinimum(0);
			grReflData[i][ii]->SetMaximum(1);
			grReflData[i][ii]->GetXaxis()->SetTitle("Energy [eV]");
			grReflData[i][ii]->GetYaxis()->SetTitle("Reflectivity");
			grReflData[i][ii]->GetXaxis()->SetTitleOffset(1.4);
			grReflData[i][ii]->GetYaxis()->SetTitleOffset(1.4);
			grReflData[i][ii]->GetXaxis()->CenterTitle(1);
			grReflData[i][ii]->GetYaxis()->CenterTitle(1);

			if(i == 0)
			{
				grReflData[i][ii]->SetLineStyle(ii%10+1);
				grReflData[i][ii]->SetLineColor(ii%4+1);
				grReflData[i][ii]->SetLineWidth(2);
				if(ii == 0)
					grReflData[i][ii]->Draw("AL");
				else
					grReflData[i][ii]->Draw("SAME & L");
			
				leg[i]->AddEntry(grReflData[i][ii],grReflData[i][ii]->GetName(),"l");
			}
			else
			{
				grReflData[i][ii]->SetMarkerStyle(ii+20);
				grReflData[i][ii]->SetMarkerSize(0.7);
				grReflData[i][ii]->SetMarkerColorAlpha(kBlack,0.5);
				grReflData[i][ii]->Draw("SAME & P");

				leg[i]->AddEntry(grReflData[i][ii],grReflData[i][ii]->GetName(),"p");
			}
		}
		leg[i]->Draw();
	}
	grReflData[0][0]->SetTitle(title.Data());
	gPad->SetGrid();
	gPad->SetLogx();

	cc->SaveAs("PicPlotReflData.png");

	return;
}

void PrintReflData(	
	string name, 
	vector<pair<double, 
	vector<double>>> dataset,
	vector<double> angle, 
	vector<double> energy)
{
	cout<<"\n\n=================================================================="<<endl;
	cout<<name<<endl;

	// print column names
	printf("Angl/Ene : ");
	for(int j = 0; j < angle.size(); ++j){printf("%.2e  ",angle.at(j));}
	printf("\n");

	printf("Angl/Ene : ");
    	for(int j = 0; j < dataset.size(); ++j)
    	{
        	printf("%.2e  ",dataset.at(j).first);
    	}
	printf("\n");

	// print data
    	for(int i = 0; i < dataset.at(0).second.size(); ++i)
    	{
		printf("%.2e  : ",energy.at(i));
        	for(int j = 0; j < dataset.size(); ++j)
        	{
           		printf("%.2e  ",dataset.at(j).second.at(i));
        	}
		printf("\n");
    	}
	cout<<"==================================================================\n\n"<<endl;

	return;
}

vector<pair<double, vector<double>>> LoadReflData(
	string filename,
	vector<double> &angle,
	vector<double> &energy)
{
	// Reads a CSV file into a vector of <string, vector<double>> pairs where
	// each pair represents <column name, column values>

	vector<pair<double, vector<double>>> dataset;

	// Create an input filestream
    	ifstream myFile(filename);

    	// make sure the file is open
    	if(!myFile.is_open()) 
	{
		cerr<<"Cannot open or find the file"<<endl;
		exit(-1);
	}

    	// Helper vars
    	string line, colname;
    	double val;

    	// Read the column names
    	if(myFile.good())
    	{
        	// Extract the first line in the file
        	getline(myFile, line);

        	// Create a stringstream from line
        	stringstream ss(line);

        	// Extract each column name
		getline(ss, colname, ','); // skip the first column
        	while(getline(ss, colname, ','))
		{
			val = stod(colname);
			// Initialize and add <colname, double vector> pairs to dataset
            		dataset.push_back({val, vector<double> {}});
			angle.push_back(val);
        	}
    	}

    	// Read data, line by line
    	while(getline(myFile, line))
    	{
        	// Create a stringstream of the current line
        	stringstream ss(line);
        
        	// Keep track of the current column index
        	int colIdx = 0;
        
        	// Extract each value
        	while(ss >> val)
		{
            		// Add the current integer to the 'colIdx' column's values vector
            		if(colIdx == 0) energy.push_back(val);
			else dataset.at(colIdx-1).second.push_back(val);
            
            		// If the next token is a comma, ignore it and move on
            		if(ss.peek() == ',') ss.ignore();
            
            		// Increment the column index
            		colIdx++;
        	}
    	}

    	// Close the file
    	myFile.close();

	return dataset;
}


