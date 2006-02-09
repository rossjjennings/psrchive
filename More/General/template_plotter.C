#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cpgplot.h>
#include <time.h>
#include <sstream>
#include <map>

#include "Pulsar/Plotter.h"
#include "Reference.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/StandardSNR.h"

#include "Error.h"
#include "RealTimer.h"

#include "dirutil.h"
#include "string_utils.h"
#include "genutil.h"
#include "slalib.h"

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

using namespace std;
using namespace Pulsar;

///////////////////////////////////////////////////////////////////
// psrchive_template.C
//
// Written by: Albert Teoh
// 
// This is a template application, initially programmed for plotting
// profiles. However, this can be changed to do anything you like
// as long as you know how to call various library routines.
//
// Some annotations are provided to assist you in making these changes
// but these can only help so much. Please let me know if you need
// any assistance with this.
//
// ateoh@swin.edu.au or albert.teoh@csiro.au

//////////////////////////
// Method definitions

// Initialise any parameters or states here
void init();

// Prints usage information
void usage();

// You shouldn't need to change this. This reads all the archive files that you
// provide on the command line.
vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
                 bool breakup_archive);

// Plots the profile on PGPLOT
void plotProfile(const Profile * profile, Pulsar::Plotter plotter);


// Parses the various input argument flags e.g. -h -v etc.
void parseParameters(int argc, char **argv, string &plot_device);

// Sets the viewport to where the profile should be displayed
// on the PGPLOT window
void goToProfileViewPort();

// Parses a string to a number and checks if that value is a positive value
// 
// Given:
//     optionName   The name of the option used over the command line. e.g. -ps
//     numberString The input string accompanied with the option. e.g. -pr 20	
//                  where 20 is the numberString
//     value        The output value once the string has been parsed.
void parseAndValidatePositiveDouble(string optionName, char * numberString, double &value);
void parseAndValidatePositiveInt(string optionName, char * numberString, int &value);

// Parses a string to a number
// Returns the numerical value in "value"
void parseAndValidateDouble(string optionName, char * numberString, double &value);
void parseAndValidateInt(string optionName, char * numberString, int &value);


///////////////////////////
/// Global variables

bool verbose = false;
bool force = false;
int beginFilenamesIndex;

/////////////////////////////
// Constants
const float PLOT_CHAR_HEIGHT = 0.8;

// Approx. number of ticks to be drawn on the profile
const int NUM_VERT_PROFILE_TICKS = 4;

const int NUMERIC_CHARS_LEN = 13;

const	char NUMERIC_CHARS [NUMERIC_CHARS_LEN] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'e', '.'};


void usage (bool verbose_usage)
{
  cout << "Usage instructions: " << endl;

  // Add your usage instructions here:
  cout << "This is a template application for developing your own psrchive app" << endl;
}


void init() {
  force = false;
  verbose = false;
}

// Parses the input flags and parameters provided over the command line.
//
// Adding a new input argument
// ---------------------------
// If you have created a new input e.g. -e <file extension>, then you 
// will need to add a new input argument to the parseParameters function:
// void parseParameters(int argc, char **argv, string &plot_device, string &extension) {
// Then set this variable inside the function using the argv array.
// You will need to then change the method definition above to be consistent
// with this change.
// Be sure to call parseParameters with the new variable:
// string ext;
// parseParameters(argc, argv, plot_device, ext);

// If you wish to pass in numbers as arguments, you can use the 
// parseAndValidate* methods provided (defined above)
void parseParameters(int argc, char **argv, string &plot_device) {

  string optionArg;
  
  // If no command line argument was provided, then print out usage
  if (argc <= 1) {
    usage(false);
    exit(0);
  }
  
  // Get command line options and arguments as necessary
  for (int i = 1; i < argc; i++) {
  
    // help! 
    if (strcmp(argv[i], "-h") == 0) {
      usage(false);
      exit(0);
     }
    else if (strcasecmp(argv[i], "--help") == 0) {
      usage(true);
      exit(0);
     }
    
    // verbose
    else if (strcmp(argv[i], "-v") == 0) {
      verbose = true;
    }
    
    // graph device
    else if (strcmp(argv[i], "-g") == 0) { 
      i++; // jump to the next argument which is the plot device name
      
      plot_device = argv[i];
      
      if (verbose) {
        cout << "Using plot device " << plot_device << endl;
      }
    }
    
    // force - Don't prompt the user about anything
    else if (strcmp(argv[i], "-f") == 0 || strcasecmp(argv[i], "--force") == 0) {
      if (verbose) {
        cout << "Setting force on" << endl;
      }
      force = true;  
    }

    else {
      beginFilenamesIndex = i;
      break;
    }
  }
}


void cpg_next ()
{
  cpgsch (1);
  cpgsls (1);
  cpgslw (1);
  cpgsci (1);
  //cpgsvp (0.1, 0.9, 0.15, 0.9);
  cpgsvp (0, 1, 0, 1);
  cpgpage ();
}

int main (int argc, char** argv) 
{
  RealTimer clock;
  double elapsed;
  
  float lineHeight = 0.16;
  
  char c, d;
  string plot_device = "/xs";
  
  int lgraph;
  string option;
  
  vector<string> filenames;

  // bools of whether archive should be broken up into its subints
  vector<int> breakup_archives; 
    
  Pulsar::Plotter plotter;

  // Get the command line parameters
  parseParameters(argc, argv, plot_device); 

  Pulsar::Archive::verbose = 0;

  if (cpgopen(plot_device.c_str()) < 0) {
    cout << "Error: Could not open plot device" << endl;
    return -1;
  }

  // Read in the files
  for( unsigned i=beginFilenamesIndex; i<argc; i++){
    filenames.push_back( argv[i] );
    breakup_archives.push_back( false );
  }


  if (filenames.size() == 0) {          
    cerr << "pdmp: please specify filename[s]" << endl;
    return -1;
  }
  
  clock.start();
  
  cpgask(!force);
    
  // Foreach input archive file
  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {
    
    // Read in the archive files
    vector<Reference::To<Pulsar::Archive> > archives = 
      get_archives(filenames[ifile],breakup_archives[ifile]);

    for( unsigned iarch = 0; iarch < archives.size(); iarch++){
    
      // reset global variables    
      init();
    
      Reference::To<Pulsar::Archive> archive = archives[iarch];
            
      cout << endl << 
        "Working on archive " << archive->get_source() << 
        ": " << archive->get_filename() << 
        endl << endl;

      cpg_next();

      // Don't override my viewport settings!
      plotter.set_svp(false);
      
      // Make a new copy so the original archive isn't changed accidentally
      Reference::To<Pulsar::Archive> copy = archive->clone();
      
      goToProfileViewPort();
      
      // Do the plotting. There are two ways to do this. The simplest and easiest
      // is this (you can scrunch it on any dimension first if you like):
      plotter.singleProfile(copy);
      
      // However, if you want more flexibility, you can call the plotProfile method
      // that is defined below. So for example:
      // plotProfile(copy->get_Profile(0,0,0), plotter);
      // You can alter plotProfile to suit your needs whereas singleProfile will
      // stick to a specific output format on pgplot.
      
    }
  }
  catch (Error& error) {
    cerr << "Caught Error on file '" << filenames[ifile] 
     << "': " << error << endl;
  }
  catch (string& error) {
    cerr << error << endl;
  }
  
  cpgend();
  
  clock.stop();
  
  elapsed = clock.get_elapsed();

  printf ("\nThis application took %.2lf seconds to compute results.\n", elapsed  );  
  
  return 0;
}

// Reads in the archive files into something psrchive can understand
vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
                 bool breakup_archive) {
  vector<Reference::To<Pulsar::Archive> > archives;
  Reference::To<Pulsar::Archive> arch( Pulsar::Archive::load(filename) );
    
  if( !breakup_archive ){
    archives.push_back( arch );
    return archives;
  }
  
  for( unsigned isub=0; isub<arch->get_nsubint(); isub++){
    vector<unsigned> subints(1,isub);
    archives.push_back( arch->extract(subints) );
    archives.back()->set_filename( arch->get_filename() + 
           " subint " + make_string(isub) );
  }
  
  return archives;
}

// Method to plot a profile which gives some flexibility on labelling the plot
void plotProfile(const Profile * profile, Plotter plotter) {

  cpgsch(0.7);
  cpgslw(1);
  goToProfileViewPort();
  plotter.set_char_height(PLOT_CHAR_HEIGHT);
  plotter.plot (profile);
  double min, max;
  
  min = profile->min();
  max = profile->max();
  
  int div = 2;
  
  double tickSpace = cpgrnd((max-min)/(float)(NUM_VERT_PROFILE_TICKS-1), &div);
    
  cpgbox ("BINTS", 0.0, 0, "BNTSI", tickSpace, 0);
}

// Allows you to determine the location and dimensions of your plot on the pgplot window
void goToProfileViewPort() {
  // Fill most of the window from x1 = 0.1 to x2 = 0.9 and y1 = 0.1 to y2 = 0.9
  // where these values range from 0 to 1
  cpgsvp(0.1, 0.9, 0.1, 0.9);
}


/////////////////////////////////////////////
// Parsing and validating routines
//

bool isNumber(char * str) {
	
	for (int i = 0; str[i] != '\0'; i++) {
		for (int j = 0; ; j++) {
			if (str[i] == NUMERIC_CHARS[j]) {
				break;
			}
			
			// Reached the end which means it's not a number
			if (j == NUMERIC_CHARS_LEN - 1) {
				return false;
			}
		}
	}
	
	return true;
}

void parseAndValidateDouble(string optionName, char * numberString, double &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}
			
	value = atof(numberString);

}

void parseAndValidateInt(string optionName, char * numberString, int &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}
			
	value = atoi(numberString);
}

void parseAndValidatePositiveDouble(string optionName, char * numberString, double &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}
			
	value = atof(numberString);

	if (value <= 0 ) {
		cerr << endl << "Please provide a positive float for value: " << optionName << endl;
		exit(1);
	}

}
void parseAndValidatePositiveInt(string optionName, char * numberString, int &value) {

	string str = numberString;

	if (!isNumber(numberString)) {
		cerr << endl << "Error: " << str << " is an invalid number for " << optionName << " option" << endl;
		exit(1);
	}
			
	value = atoi(numberString);

	if (value <= 0 ) {
		cerr << endl << "Please provide a positive int for value: " << optionName << endl;
		exit(1);
	}

}
