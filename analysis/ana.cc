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
//    	TString mainInDir   = "/Users/andrii/geant4_workdir/EIC_SR_Geant4/output_noEcut";
//    	TString outputFileName = "output_noEcut";
    	TString mainInDir   = "/Users/andrii/geant4_workdir/EIC_SR_Geant4/output_5keVcut";
    	TString outputFileName = "output_5keVcut";

    	TString mainOutDir  = "../output";
    	TString inputFileName = "output_*.root";
	TString treeName = "tree";
    	//______________________________________________________________________________________________________//

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
