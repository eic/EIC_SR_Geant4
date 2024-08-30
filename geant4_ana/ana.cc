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

	TString mainInDir, inputFileName, mainOutDir, outputFileName, treeName;
	Double_t minZ, maxZ;
	Float_t r_target_mm;

	if(argc == 9)
	{
		mainInDir 	= argv[1];
		inputFileName 	= argv[2];
		mainOutDir 	= argv[3];
		outputFileName 	= argv[4];
		treeName 	= argv[5]; 
		minZ		= atof(argv[6]);
		maxZ		= atof(argv[7]);
		r_target_mm	= atof(argv[8]);
		
		cout<<"\n\n=================================================================="<<endl;
		cout<<"[INFO] main ==> Input parameters"<<endl;
		cout<<"argv[1] = "<<mainInDir<<endl;
		cout<<"argv[2] = "<<inputFileName<<endl;
		cout<<"argv[3] = "<<mainOutDir<<endl;
		cout<<"argv[4] = "<<outputFileName<<endl;
		cout<<"argv[5] = "<<treeName<<endl;
		cout<<"argv[6] = "<<minZ<<endl;
		cout<<"argv[7] = "<<maxZ<<endl;
		cout<<"argv[8] = "<<r_target_mm<<endl;
		cout<<"==================================================================\n"<<endl;
	}
	else
	{
		cout<<"\n\n=================================================================="<<endl;
		cout<<"[ERROR] main ==> Wrong number of input parameters: argc = "<<argc<<endl;
		cout<<"Usage: [executable] [input file directory] [input file name] [output file directory] [output file name pattern] [input tree name] [min Z in cm] [max Z in cm] [r_target in mm]"<<endl;
		cout<<"Example: ./exe ../../geant4_sim/output output_*.root ../output output tree -10000 10000 0"<<endl;
		cout<<"==================================================================\n"<<endl;

		return -1;
	}

    	AnalysisSR* ana = new AnalysisSR();

    	ana->SetInputFileName(mainInDir+"/"+inputFileName);
    	ana->SetOutputFileNameRoot(mainOutDir+"/"+outputFileName+".root");
    	ana->SetOutputFileNameHepmc(mainOutDir+"/"+outputFileName+".hepmc");
    	ana->Init(treeName,minZ,maxZ);

    	cout<<"[INFO] Input file name: "<<ana->GetInputFileName()<<endl;
    	cout<<"[INFO] Output ROOT file name: "<<ana->GetOutputFileNameRoot()<<endl;
    	cout<<"[INFO] Output HEPMC file name: "<<ana->GetOutputFileNameHepmc()<<endl;
	cout<<"[INFO] Target radius = "<<r_target_mm<<" [mm]"<<endl;

    	ana->Loop(r_target_mm);
    	ana->~AnalysisSR();

    	stop_time = time(NULL);
    	cout<<"[INFO] Running time is : "<<stop_time - start_time<<" [sec]"<<endl;

    	return 0;
}
