#include "src/AnalysisSR.hh"

#include <ctime>
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

	if(argc == 6)
	{
		mainInDir 	= argv[1];
		inputFileName 	= argv[2];
		mainOutDir 	= argv[3];
		outputFileName 	= argv[4];
		treeName 	= argv[5]; 
		
		cout<<"\n\n=================================================================="<<endl;
		cout<<"[INFO] main ==> Input parameters"<<endl;
		cout<<"argv[1] = "<<mainInDir<<endl;
		cout<<"argv[2] = "<<inputFileName<<endl;
		cout<<"argv[3] = "<<mainOutDir<<endl;
		cout<<"argv[4] = "<<outputFileName<<endl;
		cout<<"argv[5] = "<<treeName<<endl;
		cout<<"==================================================================\n"<<endl;
	}
	else
	{
		cout<<"\n\n=================================================================="<<endl;
		cout<<"[ERROR] main ==> Wrong number of input parameters: argc = "<<argc<<endl;
		cout<<"Usage: [executable] [input file directory] [input file name] [output file directory] [output file name pattern] [input tree name]"<<endl;
		cout<<"Example: ./exe ../../geant4_sim/output output_*.root ../output output tree"<<endl;
		cout<<"==================================================================\n"<<endl;

		return -1;
	}

    	AnalysisSR* ana = new AnalysisSR();

    	ana->SetInputFileName(mainInDir+"/"+inputFileName);
    	ana->SetOutputFileNameRoot(mainOutDir+"/"+outputFileName+".root");
    	ana->SetOutputFileNameHepmc(mainOutDir+"/"+outputFileName+".hepmc");
    	ana->Init(treeName);

    	cout<<"[INFO] Input file name: "<<ana->GetInputFileName()<<endl;
    	cout<<"[INFO] Output ROOT file name: "<<ana->GetOutputFileNameRoot()<<endl;
    	cout<<"[INFO] Output HEPMC file name: "<<ana->GetOutputFileNameHepmc()<<endl;

    	ana->Loop();
    	ana->~AnalysisSR();

    	stop_time = time(NULL);
    	cout<<"[INFO] Running time is : "<<stop_time - start_time<<" [sec]"<<endl;

    	return 0;
}
