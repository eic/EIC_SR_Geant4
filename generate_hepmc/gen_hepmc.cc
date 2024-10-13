// C/C++
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

// ROOT
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TFile.h"
#include "TRandom3.h"
#include "Math/DistFunc.h"

// HepMC3
#include "HepMC3/GenEvent.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Print.h"

// Eigen library for matrix operations
#include </opt/homebrew/opt/eigen/include/eigen3/Eigen/Dense>

using namespace std;
using namespace HepMC3;

//-- Define global histograms and variables
// Initial from the HepMC file
TH1D* h_posx = new TH1D("h_posx","posx",2e2,-100,100);
TH1D* h_posy = new TH1D("h_posy","posy",2e2,-100,100);
TH1D* h_posz = new TH1D("h_posz","posz",2e2,-1000,1000);
TH1D* h_dirx = new TH1D("h_dirx","momx",40e3,-2,2);
TH1D* h_diry = new TH1D("h_diry","momy",40e3,-2,2);
TH1D* h_dirz = new TH1D("h_dirz","momz",40e3,-2,2);
// Converted
TH1D* h_mag = new TH1D("h_mag","mag",1e3,0,1e-4);
TH1D* h_phi = new TH1D("h_phi","phi",1e3,-TMath::TwoPi(),TMath::TwoPi());
TH1D* h_theta = new TH1D("h_theta","theta",1e5,-TMath::TwoPi(),TMath::TwoPi());
TH1D* h_r = new TH1D("h_r","r",2e5,30,32);
TH1D* h_psi = new TH1D("h_psi","psi",1e3,-TMath::TwoPi(),TMath::TwoPi());
TH2D* h_cor = new TH2D("h_cor","cor",5,0,5,5,0,5);
TH2D* h_err = new TH2D("h_err","err",5,0,5,5,0,5);
// Converted Sub-Sample
TH1D* h_ss_mag = new TH1D("h_ss_mag","mag",1e3,0,1e-4);
TH1D* h_ss_posz = new TH1D("h_ss_posz","posz",2e2,-1000,1000);
TH1D* h_ss_phi = new TH1D("h_ss_phi","phi",1e3,-TMath::TwoPi(),TMath::TwoPi());
TH1D* h_ss_theta = new TH1D("h_ss_theta","theta",1e5,-TMath::TwoPi(),TMath::TwoPi());
TH1D* h_ss_psi = new TH1D("h_ss_psi","psi",1e3,-TMath::TwoPi(),TMath::TwoPi());
TH2D* h_ss_cor = new TH2D("h_ss_cor","cor",5,0,5,5,0,5);
TH2D* h_ss_err = new TH2D("h_ss_err","err",5,0,5,5,0,5);
// Random
TH1D* h_rnd_mag = new TH1D("h_rnd_mag","mag",1e3,0,1e-4);
TH1D* h_rnd_posz = new TH1D("h_rnd_posz","posz",2e2,-1000,1000);
TH1D* h_rnd_phi = new TH1D("h_rnd_phi","phi",1e3,-TMath::TwoPi(),TMath::TwoPi());
TH1D* h_rnd_theta = new TH1D("h_rnd_theta","theta",1e5,-TMath::TwoPi(),TMath::TwoPi());
TH1D* h_rnd_psi = new TH1D("h_rnd_psi","psi",1e3,-TMath::TwoPi(),TMath::TwoPi());
TH2D* h_rnd_cor = new TH2D("h_rnd_cor","cor",5,0,5,5,0,5);
TH2D* h_rnd_err = new TH2D("h_rnd_err","err",5,0,5,5,0,5);
// All
TH2D* h_all_cor = new TH2D("h_all_cor","cor",5,0,5,5,0,5);
TH2D* h_all_err = new TH2D("h_all_err","err",5,0,5,5,0,5);

// Vectors with initial converted coordinates
vector<double> v_psi, v_posz, v_mag, v_theta, v_phi;
// Vectors with initial converted coordinates
vector<double> v_ss_psi, v_ss_posz, v_ss_mag, v_ss_theta, v_ss_phi;
// Vectors with random convered coordinates
vector<double> v_rnd_psi, v_rnd_posz, v_rnd_mag, v_rnd_theta, v_rnd_phi;
// Total number of gammas
Long64_t nTotalSimulatedPhotons = 0;
Long64_t nTotalGeneratedPhotons = 0;

// Functions
vector<double> precompute_cumulative(const TH1* hist);
double sample_from_histogram(const TH1* hist, const vector<double>& cumulative, double uniform_sample); 
vector<vector<double>> precompute_cumulative_for_histograms(const vector<TH1*>& histograms);
void generate_correlated_empirical_samples(const Eigen::MatrixXd& correlation_matrix, int num_samples, const vector<TH1*>& histograms); 
void readHepMCfile(TString fName);
void writeHepMCfile(TString fName);
double computeCorrelation(const vector<double>& x, const vector<double>& y); 
double computeCorrelationError(double r, int n);
void writeHisto(TString fName);
void calculateCorrelation(Eigen::MatrixXd &correlation_matrix, vector<double> v1, vector<double> v2, vector<double> v3, vector<double> v4, vector<double> v5, TH2D* hc, TH2D* he);
void calculateCorrelation(
	vector<double> v1, vector<double> v2, vector<double> v3, vector<double> v4, vector<double> v5, 
	vector<double> vv1, vector<double> vv2, vector<double> vv3, vector<double> vv4, vector<double> vv5, 
	TH2D* hc, TH2D* he);
double makeSubSample(double magMin, double magMax);

const double minGlobalPsi = -TMath::Pi(); // [rad] - minimum SR photon psi angle in the sample
const double maxGlobalPsi = TMath::Pi(); // [rad] - maximum SR photon psi angle in the sample
const double minGlobalMag = 1e-6; // [GeV] - minimum SR photon energy in the sample
const double minGlobalPosz = -800; // [mm] - minimum SR photon z position in the sample
const double maxGlobalPosz = +670; // [mm] - maximum SR photon z position in the sample
const double minGlobalTheta = -TMath::Pi(); // [rad] - minimum SR photon theta angle in the sample
const double maxGlobalTheta = TMath::Pi(); // [rad] - maximum SR photon theta angle in the sample
const double minGlobalPhi = -TMath::Pi(); // [rad] - minimum SR photon phi angle in the sample
const double maxGlobalPhi = TMath::Pi(); // [rad] - maximum SR photon phi angle in the sample
const double radiusGlobal = 31.0; // [mm] - beam pipe radius

// Main function
int main(int argc, char* argv[]) 
{
	const int num_samples = 1.7e6;  // Number of correlated samples to generate

	cout<<"============================="<<endl;
	cout<<"Input parameters:"<<endl;
	for(int i = 0; i < argc; i++)
	{
		cout<<"["<<i<<"] : "<<argv[i]<<endl;
	}
	cout<<"============================="<<endl<<endl;

	if(argc != 5)
	{
		cerr<<"Wrong number of input parameters"<<endl;
		cout<<"[1] - input dir name"<<endl;
		cout<<"[2] - input file name pattern"<<endl;
		cout<<"[3] - input file number"<<endl;
		cout<<"[4] - output dir name"<<endl;
		return -1;
	}

	TString inDirName = argv[1];
	TString inFileName = argv[2];
	Int_t inFileNum = atoi(argv[3]);
	TString outDirName = argv[4];
	cout<<"inDirName = "<<inDirName<<endl;
	cout<<"inFileName = "<<inFileName<<endl;
	cout<<"inFileNum = "<<inFileNum<<endl;
	cout<<"outDirName = "<<outDirName<<endl;
	
	vector<double> magCutMin;
	vector<double> magCutMax;
	for(double mag = 0; mag < 1e-3; mag += 1e-6)
	{
		magCutMin.push_back(mag);
		magCutMax.push_back(mag+1e-6);
	}

	// Loop over all HepMC files
	for(Int_t i = 0; i < inFileNum; i++)
	{
		TString fName = Form("%s/%s%d.hepmc",inDirName.Data(),inFileName.Data(),i+1);
		readHepMCfile(fName.Data());
	}

	// Correlation matrix
	Eigen::MatrixXd correlation_matrix(5, 5);

	// Define the correlation matrix
	calculateCorrelation(
		correlation_matrix,v_psi, v_posz, v_mag, v_theta, v_phi, h_cor, h_err);

	// Split the spectrum into sub-samples for accurate sampling
	for(int i = 0; i < magCutMin.size(); i++)
	{
		double fraction = makeSubSample(magCutMin[i],magCutMax[i]);
		if(fraction < 1e-5){continue;}
		cout<<"___________________________________________________________________________________________________"<<endl;
		// Create a sub-sample and calculate its fraction
		cout<<endl<<"==> magCutMin["<<i<<"] = "<<magCutMin[i]
			<<" | magCutMax["<<i<<"] = "<<magCutMax[i]
			<<" | fraction = "<<fraction<<endl;

		// Correlation matrix
		Eigen::MatrixXd correlation_matrix_ss(5, 5);

		// Define the correlation matrix
		calculateCorrelation(
			correlation_matrix_ss,v_ss_psi, v_ss_posz, v_ss_mag, v_ss_theta, v_ss_phi, h_ss_cor, h_ss_err);

		// Create histograms for each variable's empirical distribution
		vector<TH1*> histograms_ss = {h_ss_psi, h_ss_posz, h_ss_mag, h_ss_theta, h_ss_phi};
		// Store vectors in an array for convenience
		vector<vector<double>*> vectors_ss = {&v_ss_psi, &v_ss_posz, &v_ss_mag, &v_ss_theta, &v_ss_phi};
		// Fill histograms
		for (int i = 0; i < histograms_ss.size(); i++)
		{
			for (int j = 0; j < vectors_ss[i]->size(); j++)
				histograms_ss[i]->Fill(vectors_ss[i]->at(j)); 
		}

    		// Generate correlated empirical samples
		int num_samples_ss = static_cast<int>(num_samples * fraction);
		cout<<"Number of samples to be generated = "<<num_samples_ss<<endl;
    		generate_correlated_empirical_samples(correlation_matrix_ss, num_samples_ss, histograms_ss);

		// Clean up the histograms
		for (TH1* hist : histograms_ss) 
		{
			hist->Reset();
		}
		cout<<"________________________________________________________________________________________"<<endl;
	}

	// Store histograms in an array for convenience
	vector<TH1*> histograms_rnd = {h_rnd_psi, h_rnd_posz, h_rnd_mag, h_rnd_theta, h_rnd_phi};
	// Store vectors in an array for convenience
    	vector<vector<double>*> vectors_rnd = {&v_rnd_psi, &v_rnd_posz, &v_rnd_mag, &v_rnd_theta, &v_rnd_phi};

	// Fill histograms
	for (int i = 0; i < histograms_rnd.size(); i++)
		for (int j = 0; j < vectors_rnd[i]->size(); j++)
			histograms_rnd[i]->Fill(vectors_rnd[i]->at(j));

	// Correlation matrix
	Eigen::MatrixXd correlation_matrix_rnd(5, 5);

	// Calculate the correlation matrix for the new data set
	calculateCorrelation(
		correlation_matrix_rnd,v_rnd_psi, v_rnd_posz, v_rnd_mag, v_rnd_theta, v_rnd_phi, h_rnd_cor, h_rnd_err);

	// Calculate the correlation matrix between sim and gen data sets
	calculateCorrelation(
		v_psi, v_posz, v_mag, v_theta, v_phi,
		v_rnd_psi, v_rnd_posz, v_rnd_mag, v_rnd_theta, v_rnd_phi,
		h_all_cor, h_all_err);

	// Write all data into the output hepmc file
	writeHepMCfile(Form("%s/hepmc_output.hepmc",outDirName.Data()));

	// Write all histograms into the output file
	writeHisto(Form("%s/hepmc_output.root",outDirName.Data()));

	cout<<endl;
	cout<<"#################################################"<<endl;
	cout<<"nTotalSimulatedPhotons = "<<nTotalSimulatedPhotons<<endl;
	cout<<"nTotalGeneratedPhotons = "<<nTotalGeneratedPhotons<<endl;
	cout<<"#################################################"<<endl;
	cout<<endl;

    	return 0;
}

// Create a sub-sample with the cut
double makeSubSample(double magMin, double magMax)
{
	// Reset the vectors
	v_ss_psi.clear();
	v_ss_posz.clear();
	v_ss_mag.clear();
	v_ss_theta.clear();
	v_ss_phi.clear();

	Long64_t selected = 0, total = 0;

	for(int i = 0; i < v_mag.size(); i++)
	{
		total++;
		if(v_mag[i] < magMin || magMax <= v_mag[i]){continue;}

		// Fill the sub-sample
		v_ss_psi.push_back(v_psi[i]);
		v_ss_posz.push_back(v_posz[i]);
		v_ss_mag.push_back(v_mag[i]);
		v_ss_theta.push_back(v_theta[i]);
		v_ss_phi.push_back(v_phi[i]);
		selected++;
	}

	return static_cast<double>(selected) / total;
}

// Precompute the cumulative distribution
vector<double> precompute_cumulative(const TH1* hist) 
{
    	int nbins = hist->GetNbinsX();
    	vector<double> cumulative(nbins + 1, 0.0);
    
    	for (int i = 1; i <= nbins; i++) 
    	{
        	cumulative[i] = cumulative[i-1] + hist->GetBinContent(i);
    	}

    	// Normalize to ensure the last element is exactly 1.0
    	double total = cumulative[nbins];
    	for (int i = 1; i <= nbins; i++) 
    	{
        	cumulative[i] /= total;
    	}

    	return cumulative;
}

// Function to sample from the histogram using a uniform sample and binary search
double sample_from_histogram(const TH1* hist, const vector<double>& cumulative, double uniform_sample) 
{
	int nbins = hist->GetNbinsX();
	auto it = lower_bound(cumulative.begin() + 1, cumulative.begin() + nbins + 1, uniform_sample);

	// Find the corresponding bin
	int bin_index = distance(cumulative.begin(), it);
	return hist->GetBinCenter(bin_index);
}

// Function to create an array of cumulative distributions for multiple histograms
vector<vector<double>> precompute_cumulative_for_histograms(const vector<TH1*>& histograms)
{
    	vector<vector<double>> cumulative_array;

    	// Loop over all histograms and compute their cumulative distributions
    	for (const auto& hist : histograms) 
    	{
        	cumulative_array.push_back(precompute_cumulative(hist));
    	}

    	return cumulative_array;
}

// Function to generate correlated empirical samples using histograms of empirical data distributions 
// and a given correlation matrix 
void generate_correlated_empirical_samples(
	const Eigen::MatrixXd& correlation_matrix, int num_samples, const vector<TH1*>& histograms) 
{
	// Perform Cholesky decomposition of the correlation matrix
    	Eigen::LLT<Eigen::MatrixXd> chol(correlation_matrix);
    	Eigen::MatrixXd L = chol.matrixL();

    	TRandom3 randGen(time(0));  // ROOT random generator with random seed

    	Eigen::MatrixXd Z(num_samples, correlation_matrix.rows());

    	// Map empirical data to uniform space
	// Generate independent uniform samples for each variable
    	for (int i = 0; i < num_samples; i++) 
	{
        	for (int j = 0; j < correlation_matrix.rows(); j++) 
		{
            		// Assume that the value is sampled from the empirical distribution
            		double sample = randGen.Rndm();  // Draw a random uniform sample
            		Z(i, j) = sample;  // Store uniform samples directly
        	}
    	}

    	// Introduce correlation between uniform samples using Cholesky decomposition
    	Eigen::MatrixXd correlated_uniforms = Z * L.transpose();

    	// Map uniform samples back to empirical space
    	Eigen::MatrixXd empirical_samples(num_samples, correlation_matrix.rows());

	// Precompute cumulative distributions for all histograms
	vector<vector<double>> cumulative_distributions = precompute_cumulative_for_histograms(histograms);

	int i, nFills = 0;
	for(i = 0; i < num_samples; i++)
	{
            	if(i%1000 == 0)
              	{
                     	printf("\r[INFO] Progress: %.2f [%%]",100.0*i/num_samples);
                    	fflush(stdout);
             	}
        	for (int j = 0; j < correlation_matrix.rows(); j++) 
		{
			// Fill vectors
            		double uniform_sample = correlated_uniforms(i, j);  // Already uniform
            		empirical_samples(i, j) = sample_from_histogram(
							histograms[j], cumulative_distributions[j], uniform_sample);
			switch (j) 
			{
    				case 0:
					v_rnd_psi.push_back(empirical_samples(i, j));
					break;
				case 1:
					v_rnd_posz.push_back(empirical_samples(i, j));
					break;
				case 2:
					v_rnd_mag.push_back(empirical_samples(i, j));
					break;
				case 3:
					v_rnd_theta.push_back(empirical_samples(i, j));
					break;
				case 4:
					v_rnd_phi.push_back(empirical_samples(i, j));
					break;
			}
        	}
		nFills++;
    	}
	printf("\r[INFO] Progress: %.3f [%%]\n",100.0*i/num_samples);
	cout<<"nFills = "<<nFills<<" ("<<100.0 * static_cast<double>(nFills)/num_samples<<"%)"<<endl;

    	// Output the first few samples (just to verify)
    	cout << "First 5 correlated samples: " << endl;
    	cout << empirical_samples.block(0, 0, 5, 5) << endl;

	return;
}

// Function to write data into a HepMC file
void writeHepMCfile(TString fName)
{
	HepMC3::WriterAscii* fHepMcAsciiOutput = new HepMC3::WriterAscii(fName.Data());
	if(!fHepMcAsciiOutput)
	{
		cerr<<"Cannot open the output file: "<<fName<<endl;
		return;
	}
	shared_ptr<GenRunInfo> run = make_shared<GenRunInfo>();
	vector<string> weight_names = {"NormFactor"};
	run->set_weight_names(weight_names);
	fHepMcAsciiOutput->set_run_info(run);
	fHepMcAsciiOutput->write_run_info();

	for(int i = 0; i < v_rnd_psi.size(); i++)
	{
		// Truncation: truncate the generated variable if it falls outside the histogram range.
		if(	v_rnd_mag[i]   > minGlobalMag   &&
			v_rnd_psi[i]   > minGlobalPsi   && v_rnd_psi[i]   < maxGlobalPsi  &&
			v_rnd_posz[i]  > minGlobalPosz  && v_rnd_posz[i]  < maxGlobalPosz &&
			v_rnd_phi[i]   > minGlobalPhi   && v_rnd_phi[i]   < maxGlobalPhi  &&
			v_rnd_theta[i] > minGlobalTheta && v_rnd_theta[i] < maxGlobalTheta)
		{
			HepMC3::GenEvent hepmcevt(HepMC3::Units::GEV,HepMC3::Units::MM);	
			hepmcevt.set_run_info(run);
			vector<double> weights = {1};
			hepmcevt.weights() = weights;

			double x = radiusGlobal * cos(v_rnd_psi[i]);
			double y = radiusGlobal * sin(v_rnd_psi[i]);
			double z = v_rnd_posz[i];

			double px = v_rnd_mag[i] * sin(v_rnd_theta[i]) * cos(v_rnd_phi[i]); 
			double py = v_rnd_mag[i] * sin(v_rnd_theta[i]) * sin(v_rnd_phi[i]); 
			double pz = v_rnd_mag[i] * cos(v_rnd_theta[i]);	

			double pinx = v_rnd_mag[i] * x/sqrt(x*x+y*y+z*z);
			double piny = v_rnd_mag[i] * y/sqrt(x*x+y*y+z*z);
			double pinz = v_rnd_mag[i] * z/sqrt(x*x+y*y+z*z);

			// incoming particle
			GenParticlePtr pin = make_shared<GenParticle>( 
					FourVector(pinx,piny,pinz,v_rnd_mag[i]),22,4);	
			pin->set_generated_mass(0.);
			hepmcevt.add_particle(pin);

			// outcoming particle
			GenParticlePtr pout = make_shared<GenParticle>(
					FourVector(px,py,pz,v_rnd_mag[i]),22,1);
			pout->set_generated_mass(0.);
			hepmcevt.add_particle(pout);

			// vertex
			GenVertexPtr vertex = make_shared<GenVertex>(
					FourVector(x,y,z,0.));
			vertex->add_particle_in(pin);
			vertex->add_particle_out(pout);

			// add the vertex to the event
			hepmcevt.add_vertex(vertex);

			// set the event number
			hepmcevt.set_event_number(nTotalGeneratedPhotons);

			// write an event
			fHepMcAsciiOutput->write_event(hepmcevt);

			nTotalGeneratedPhotons++;
		}
	}
	if(fHepMcAsciiOutput){delete fHepMcAsciiOutput;}
	cout<<"Output file name "<<fName<<endl;

	return;
}

// Function to read a HepMC file
void readHepMCfile(TString fName)
{
	cout<<"Input file name "<<fName<<endl;
	Int_t nEvents = 0, nVtx = 0, nGammas = 0;

	HepMC3::ReaderAscii* fHepMcAsciiInput = new HepMC3::ReaderAscii(fName.Data());
	HepMC3::GenEvent hepmcevt(HepMC3::Units::GEV,HepMC3::Units::MM);
	if(fHepMcAsciiInput->failed()){cout<<"ERROR:: No available events in the file: "<<fName<<endl; return;}
	while(!fHepMcAsciiInput->failed())
	{
		fHepMcAsciiInput->read_event(hepmcevt);

		Double_t mag, phi, theta, r, psi;
		HepMC3::FourVector mom;
		HepMC3::FourVector pos;

		for (const auto& vertex : hepmcevt.vertices())
		{
			// outgoing particles
			for(const auto& particle : vertex->particles_out()) 
			{
				// gamma
				if (!particle->end_vertex() && particle->status() == 1 && particle->pid() == 22) 
				{
					pos = vertex->position();
					mom = particle->momentum();
					mag = mom.length();
					phi = mom.phi();
					theta = mom.theta();
					r = pos.perp();
					psi = pos.phi();

					h_posx->Fill(pos.x());
					h_posy->Fill(pos.y());
					h_posz->Fill(pos.z());
					h_dirx->Fill(mom.x()/mag);
					h_diry->Fill(mom.y()/mag);
					h_dirz->Fill(mom.z()/mag);
					h_mag->Fill(mag);
					h_phi->Fill(phi);
					h_theta->Fill(theta);
					h_r->Fill(r);
					h_psi->Fill(psi);

					v_psi.push_back(psi);
					v_posz.push_back(pos.z());
					v_mag.push_back(mag);
					v_phi.push_back(phi);
					v_theta.push_back(theta);

					nGammas++;
				}
			}
			nVtx++;
		}
		nEvents++;
	}
	cout<<"nEvents = "<<nEvents<<" | nVtx = "<<nVtx<<" | nGammas = "<<nGammas<<endl;
	nTotalSimulatedPhotons += nGammas;

	fHepMcAsciiInput->close();
	delete fHepMcAsciiInput;

	return;
}

// Pearson Correlation Coefficient Calculation
double computeCorrelation(const vector<double>& x, const vector<double>& y) 
{
	if (x.size() > y.size()) 
	{
        	cerr << "Error: Vectors must be of the same length or at least the first vector size should me smaller." << endl;
		return 0.0;
    	}

    	int n = x.size();
    	double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0, sumY2 = 0;

    	for (int i = 0; i < n; i++) 
	{
		sumX += x[i];
		sumY += y[i];
		sumXY += x[i] * y[i];
		sumX2 += x[i] * x[i];
		sumY2 += y[i] * y[i];
    	}

    	double numerator = (n * sumXY) - (sumX * sumY);
    	double denominator = TMath::Sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));

    	return (denominator != 0) ? numerator / denominator : 0.0; // Avoid division by zero
}

// Fisher Transformation
double computeCorrelationError(double r, int n) 
{
	if (n <= 3) 
	{
        	cerr << "Error: Not enough data points." << endl;
        	return 0.0;
    	}

    	// Fisher transformation
    	double z = 0.5 * TMath::Log((1 + r) / (1 - r));
    	double SE_z = 1.0 / TMath::Sqrt(n - 3);

    	// Confidence interval for z
    	double z_critical = 1.96; // for 95% confidence
    	double z_lower = z - z_critical * SE_z;
    	double z_upper = z + z_critical * SE_z;

    	// Back transformation to r
    	double r_lower = TMath::TanH(z_lower);
    	double r_upper = TMath::TanH(z_upper);

    	// Standard error
    	return (r_upper - r_lower) / 2; // Approximation for standard error
}

void calculateCorrelation(
	Eigen::MatrixXd &correlation_matrix, 
	vector<double> v1, vector<double> v2, vector<double> v3, vector<double> v4, vector<double> v5, 
	TH2D* hc, TH2D* he)
{
    	const Int_t nVectors = 5;
	TString v_name[nVectors] = {"psi","posz","mag","theta","phi"};
	hc->Reset();
	he->Reset();

	// Store vectors in an array for convenience
    	vector<vector<double>*> vectors = {&v1, &v2, &v3, &v4, &v5};

    	for (int i = 0; i < nVectors; i++) 
	{
        	for (int j = 0; j < nVectors; j++) 
		{
            		double correlation = computeCorrelation(*vectors[i], *vectors[j]);
			double error = computeCorrelationError(correlation, vectors[i]->size());
			hc->SetBinContent(i+1,j+1,correlation);
			he->SetBinContent(i+1,j+1,error);

			correlation_matrix(i,j) = correlation;
        	}
    	}

	// Set labels
	for(int i = 0; i < nVectors; i++)
	{
		hc->GetXaxis()->SetBinLabel(i+1,v_name[i].Data());
		hc->GetYaxis()->SetBinLabel(i+1,v_name[i].Data());
		he->GetXaxis()->SetBinLabel(i+1,v_name[i].Data());
		he->GetYaxis()->SetBinLabel(i+1,v_name[i].Data());
	}

	// Print the correlation matrix
	cout<<"============================================"<<endl;
	cout<<"Correlation Matrix"<<endl;
	cout<<correlation_matrix<<endl;
	cout<<"============================================"<<endl;

	return;
}

void calculateCorrelation(
	vector<double> v1, vector<double> v2, vector<double> v3, vector<double> v4, vector<double> v5, 
	vector<double> vv1, vector<double> vv2, vector<double> vv3, vector<double> vv4, vector<double> vv5, 
	TH2D* hc, TH2D* he)
{
    	const Int_t nVectors = 5;
	Eigen::MatrixXd correlation_matrix(nVectors, nVectors);

	TString v_name[nVectors] = {"psi","posz","mag","theta","phi"};
	hc->Reset();
	he->Reset();

	// Store vectors in an array for convenience
    	vector<vector<double>*> vectors1 = {&v1, &v2, &v3, &v4, &v5};
    	vector<vector<double>*> vectors2 = {&vv1, &vv2, &vv3, &vv4, &vv5};

    	for (int i = 0; i < nVectors; i++) 
	{
        	for (int j = 0; j < nVectors; j++) 
		{
            		double correlation = computeCorrelation(*vectors1[i], *vectors2[j]);
			double error = computeCorrelationError(correlation, vectors1[i]->size());
			hc->SetBinContent(i+1,j+1,correlation);
			he->SetBinContent(i+1,j+1,error);

			correlation_matrix(i,j) = correlation;
        	}
    	}

	// Set labels
	for(int i = 0; i < nVectors; i++)
	{
		hc->GetXaxis()->SetBinLabel(i+1,v_name[i].Data());
		hc->GetYaxis()->SetBinLabel(i+1,v_name[i].Data());
		he->GetXaxis()->SetBinLabel(i+1,v_name[i].Data());
		he->GetYaxis()->SetBinLabel(i+1,v_name[i].Data());
	}

	// Print the correlation matrix
	cout<<"============================================"<<endl;
	cout<<"Correlation Matrix"<<endl;
	cout<<correlation_matrix<<endl;
	cout<<"============================================"<<endl;

	return;
}

void writeHisto(TString fName)
{
	TFile* f = new TFile(fName.Data(), "RECREATE");
	f->cd();

	h_posx->Write();
	h_posy->Write();
	h_posz->Write();
	h_dirx->Write();
	h_diry->Write();
	h_dirz->Write();

	h_mag->Write();
	h_phi->Write();
	h_theta->Write();
	h_r->Write();
	h_psi->Write();
	h_cor->Write();
	h_err->Write();

	h_rnd_mag->Write();
	h_rnd_phi->Write();
	h_rnd_theta->Write();
	h_rnd_posz->Write();
	h_rnd_psi->Write();
	h_rnd_cor->Write();
	h_rnd_err->Write();

	h_all_cor->Write();
	h_all_err->Write();

	f->Close();
	cout<<"Output file name: "<<f->GetName()<<endl;

	return;
}


