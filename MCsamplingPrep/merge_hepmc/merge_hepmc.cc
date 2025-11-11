#include <HepMC3/ReaderAscii.h>
#include <HepMC3/WriterAscii.h>
#include <HepMC3/GenEvent.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char* argv[]) 
{
	if (argc != 5) 
	{
		cerr << "Usage: " << argv[0] << " output_file.hepmc input_file_ n_files sphx" << endl;
		return -1;
    	}

    	string output_filename = argv[1];
	vector<string> input_filenames;

	for (int i = 0; i <= atoi(argv[3]); ++i) 
	{
        	input_filenames.push_back(argv[2]+to_string(i)+argv[4]);
    	}

    	// Create a HepMC3 writer for the output file
    	HepMC3::WriterAscii writer(output_filename);

	int event_count = 0;  // Counter for the number of events written

    	for (const auto& input_filename : input_filenames) 
	{
		// Check if file exists and can be opened
		ifstream infile(input_filename);
		if (!infile.is_open())
		{
			cerr << "Warning: Cannot open input file '" << input_filename << "'. Skipping." << endl;
			continue;  // Skip this file
		}
		infile.close();

		cout << "Processing file: " << input_filename << endl;

        	// Create a HepMC3 reader for the current input file
        	HepMC3::ReaderAscii reader(input_filename);

        	// Container for events
        	HepMC3::GenEvent event;

        	while (!reader.failed()) 
		{
            		// Read an event from the input file
            		reader.read_event(event);

            		if (reader.failed()) 
			{
                		break;  // Exit if the read operation failed (e.g., end of file)
            		}

			// Set the event number to ensure uniqueness
			event.set_event_number(event_count);

            		// Write the event to the output file
			cout <<"["<<event_count<<"] : ";
            		writer.write_event(event);
			event_count++;  // Increment the event counter
        	}

        	// Close the current reader
        	reader.close();
    	}

    	// Close the writer
    	writer.close();

    	cout << "Merging complete. Output written to " << output_filename << " with " << event_count << " events." << endl;

    	return 0;
}
