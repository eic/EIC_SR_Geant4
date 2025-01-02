#include "src/AnalysisSR.hh"

#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

int main(int argc, char* argv[])
{
    	time_t start_time, stop_time;
    	start_time = time(NULL);

	TString mainInDir, inputFileName, mainOutDir, outputFileName;
	Double_t minZ, maxZ, minVtxZ, maxVtxZ;
	bool flag;

	if(argc == 10)
	{
		mainInDir 	= argv[1];
		inputFileName 	= argv[2];
		mainOutDir 	= argv[3];
		outputFileName 	= argv[4];
		minZ		= atof(argv[5]);
		maxZ		= atof(argv[6]);
		minVtxZ		= atof(argv[7]);
		maxVtxZ		= atof(argv[8]);
		flag		= atoi(argv[9]);
		
		cout<<"\n\n=================================================================="<<endl;
		cout<<"[INFO] main ==> Input parameters"<<endl;
		cout<<"argv[1]  = "<<mainInDir<<endl;
		cout<<"argv[2]  = "<<inputFileName<<endl;
		cout<<"argv[3]  = "<<mainOutDir<<endl;
		cout<<"argv[4]  = "<<outputFileName<<endl;
		cout<<"argv[5]  = "<<minZ<<endl;
		cout<<"argv[6]  = "<<maxZ<<endl;
		cout<<"argv[7]  = "<<minVtxZ<<endl;
		cout<<"argv[8]  = "<<maxVtxZ<<endl;
		cout<<"argv[9]  = "<<flag<<endl;
		cout<<"==================================================================\n"<<endl;
	}
	else
	{
		cout<<"\n\n=================================================================="<<endl;
		cout<<"[ERROR] main ==> Wrong number of input parameters: argc = "<<argc<<endl;
		cout<<"Usage: [executable] [input file directory] [input file name] [output file directory] [output file name pattern] [min Z in cm] [max Z in cm] [min Vtx Z in cm] [max Vtx Z in cm] [flag]"<<endl;
		cout<<"Example: ./exe ../../geant4_sim/output output_1../output output_1 -1e9 1e9 -1e9 1e9 0"<<endl;
		cout<<"==================================================================\n"<<endl;

		return -1;
	}

    	AnalysisSR* ana = new AnalysisSR();
	ana->SetDataProdFlag(flag);

    	ana->SetOutputFileNameRoot(mainOutDir+"/"+outputFileName+".root");
    	ana->SetOutputFileNameHepmc(mainOutDir+"/"+outputFileName+".hepmc");
    	ana->Init(minZ,maxZ,minVtxZ,maxVtxZ);
    	ana->SetInputFileName(Form("%s/%s.root",mainInDir.Data(),inputFileName.Data()));
    	ana->Loop();
	ana->WriteHisto();

    	cout<<"[INFO] Input file name: "<<ana->GetInputFileName()<<endl;
    	if(!ana->GetDataProdFlag())
		cout<<"[INFO] Output ROOT file name: "<<ana->GetOutputFileNameRoot()<<endl;
    	cout<<"[INFO] Output HEPMC file name: "<<ana->GetOutputFileNameHepmc()<<endl;

    	ana->~AnalysisSR();

    	stop_time = time(NULL);
    	cout<<"[INFO] Running time is : "<<stop_time - start_time<<" [sec]"<<endl;

    	return 0;
}
