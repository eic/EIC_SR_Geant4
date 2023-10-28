#include "src/AnalysisSR.hh"

#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

int main()
{
    	time_t start_time, stop_time;
    	start_time = time(NULL);

    	//----------------------------------------------- SETTINGS ---------------------------------------------//
    	//______________________________________________________________________________________________________//
    	TString mainInDir   = "../output";
    	TString mainOutDir  = "./output";
    	TString inputFileName = "output_*.root";
    	TString outputFileName = "output_ana.root";
	TString treeName = "tree";
    	//______________________________________________________________________________________________________//

    	AnalysisSR* ana = new AnalysisSR();

    	ana->SetInputFileName(mainInDir+"/"+inputFileName);
    	ana->SetOutputFileName(mainOutDir+"/"+outputFileName);
    	ana->Init(treeName);

    	cout<<"[INFO] Input file name: "<<ana->GetInputFileName()<<endl;
    	cout<<"[INFO] Output file name: "<<ana->GetOutputFileName()<<endl;

    	ana->Loop();
    	ana->~AnalysisSR();

    	stop_time = time(NULL);
    	cout<<"[INFO] Running time is : "<<stop_time - start_time<<" [sec]"<<endl;

    	return 0;
}
