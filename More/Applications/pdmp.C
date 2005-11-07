//
// $Id: pdmp.C,v 1.5 2005/11/07 05:44:18 ateoh Exp $
//
// Searches over trial DM and Period ranges and obtains the optimal 
// DM and Period giving the highest S/N, plots SNR vs Period vs DM, 
// Phase vs Time, Phase vs Frequency
//

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cpgplot.h>
#include <time.h>
#include <sstream>
#include <map>

#include "Reference.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Plotter.h"

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

#define F77_smooth_mw F77_FUNC_(smooth_mw,SMOOTH_MW)
  
extern "C" void F77_smooth_mw (float* period, int* nbin, int* maxw, float* rms,
                               int * kwmax, float * snrmax, float * smmax,
                               float * workspace); 


//////////////////////////
// Method definitions

// Initialises all the global variables to default values.
void init();

vector<Reference::To<Pulsar::Archive> > get_archives(string filename,
						     bool breakup_archive);

// Finds the optimal DM and Period
// If any of the offset, half-range and step values are < 0, 
// a natural default value will be computed and used.
// If these values are 0, then it indicates that there is only one
// bin for that respective value. ie. if periodStep_us == 0, then it 
// means that there is only one subint bin.
// 
// 
// PRECONDITIONS: 
//   * refP is Topocentric
//   * refP, periodOffset_us, periodStep_us and periodHalfRange_us are all in microseconds
void solve_and_plot(const Archive* archive, double dmOffset, double dmStep, double dmHalfRange, 
	double periodOffset_us, double periodStep_us, double periodHalfRange_us, Plotter plotter);

// Default call to solve_and_plot above
// archive: The archive data
// refDM: The reference DM
// refP: The reference Period in microseconds
void solve_and_plot(const Archive* archive, Plotter plotter);
	
// Gets the natural DM step if none was provided
// Precondition: nchan > 1
// Returns: Natural DM step
double getNaturalDMStep(const Archive * archive, double dmHalfRange);

// Gets the natural DM half range if none was provided
// Precondition: nchan > 1
// Returns: Natural DM half range 
double getNaturalDMHalfRange(const Archive * archive, double dmStep);

// Gets the natural Period step in microseconds if none was provided
// Precondition: nsub > 1
// Returns: Natural period step in microseconds
double getNaturalPeriodStep(const Archive * archive, double periodHalfRange_us);

// Gets the natural Period half range in microseconds if none was provided
// Precondition: nsub > 1
// Returns: Natural period half range in microseconds
double getNaturalperiodHalfRange(const Archive * archive, double periodStep_us);

// Places focus on the header section of the window for writing pgplot text
void goToHeaderViewPort();

// Sets the viewport to where the phase vs. frequency plot 
// should be displayed on the PGPLOT window
void goToPhaseFreqViewPort();

// Sets the viewport to where best values 
// should be displayed on the PGPLOT window
void goToBestValuesViewPort();

// Sets the viewport to where the dDM vs dP vs SNR plot 
// should be displayed on the PGPLOT window
void goToDMPViewPort();

// Sets the viewport to where the phase vs. time plot 
// should be displayed on the PGPLOT window
void goToPhaseTimeViewPort();

// Sets the viewport to where the profile should be displayed
// on the PGPLOT window
void goToProfileViewPort();

// Viewport for the phase vs frequency/time plot titles
void goToPhaseFreqTitleViewPort();
void goToPhaseTimeTitleViewPort();

// Partially scrunches the archive based on the maxChannels and maxSubints
// global variables. If the number of subints and channels are larger than
// the user's maximum subints and channels, pdmp will scrunch it to that level
void scrunchPartially(Archive * scrunchedCopy);

// Get the SNR for the archive.
float getSNR(const Archive * archive, float rms);

// Get the RMS of the archive
float getRMS (const Archive * archive);

// Parse the command line parameters and set the values passed through as arguments
void parseParameters(int argc, char **argv, double &periodOffset_us, double &periodStep_us, double &periodHalfRange_us, 
	                   double &dmOffset, double &dmStep, double &dmHalfRange, 
									   string &plot_device, pgplot::ColourMap::Name &colour_map);

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

// Partially scrunch over time
void partialTimeScrunch(Archive * archive);

// Partially scrunch over frequency
void partialFrequencyScrunch(Archive * archive);

// Checks if the string is a valid number (any of float, double, int, etc...)
bool isNumber(char * str);

// Returns the reference period of the central subint in seconds. 
double getPeriod(const Archive * archive);

// Returns the reference DM
double getDM(const Archive * archive);

// Gets the period error (milliseconds) based on the best pulse width
double computePeriodError(const Archive * archive);

// Gets the dm error based on the best pulse width
double computeDMError(const Archive * archive);

// slaPvobs written by P.T.Wallace
//  Position and velocity of an observing station.
//
//  (double precision)
//
//  Given:
//     p     double     latitude (geodetic, radians)
//     h     double     height above reference spheroid (geodetic, metres)
//     stl   double     local apparent sidereal time (radians)
//
//  Returned:
//     pv    double[6]  position/velocity 6-vector (au, au/s, true
//                                         equator and equinox of date)
void slaPvobs(double p, double h, double stl, double pv[6] );

// slaGeoc written by P.T.Wallace
//  Convert geodetic position to geocentric.
//
//  (double precision)
//
//  Given:
//     p     double     latitude (geodetic, radians)
//     h     double     height above reference spheroid (geodetic, metres)
//
//  Returned:
//     *r    double     distance from Earth axis (AU)
//     *z    double     distance from plane of Earth equator (AU)
//
//  Notes:
//
//     1)  Geocentric latitude can be obtained by evaluating atan2(z,r).
//
//     2)  IAU 1976 constants are used.
void slaGeoc ( double p, double h, double *r, double *z );

// Gets the doppler factor, used by multiplying with Topocentric 
// period to obtain the Barycentric period
double getDopplerFactor(const Archive * archive);

// Initialise the site code to index mapping
void initSiteCode2Index();

// Gets the site code index that refers to the site code.
// e.g. '7' will return 6. 'a' will return 9. 
// If the site code doesn't exist, an error message will be
// printed out and then defaulted to Parkes.
int getSiteCodeIndex(char site_code);

// Calculates the sidereal time
double lmst(double mjd,double olong,double *tsid,double *tsid_der);

// Emulates the fortran mod function. % is restricted to integer arguments
double fortran_mod(double a,double p);

//////////////////////////////////
///
/// Drawing and printing methods
///
//////////////////////////////////

// Print the header section on PGPLOT
void printHeader(const Archive * archive,
                 double periodOffset_us,       // period offset in microseconds
								 double periodStep_us,         // period step in microseconds
								 double periodHalfRange_us,    // p half range in microseconds
								 double dmOffset,           // dm offset
								 double dmStep,             // dm step
								 double dmHalfRange);       // dm half range

// Plots the profile of the first subint, channel and polarisation.
void plotProfile(const Profile * profile, Plotter plotter);

// Plots the original and corrected phase vs. time 
void plotPhaseTime(const Archive * archive, Plotter plotter);

// Plots the original and corrected phase vs. freq
void plotPhaseFreq(const Archive * archive, Plotter plotter);

// Finds the best gradient that fits the corrected set of 
// profiles for each frequency channel
void drawBestFitPhaseFreq(const Archive * archive, Plotter plotter);

// Finds the best gradient that fits the correct set of
// profiles for each subintegration
void drawBestFitPhaseTime(const Archive * archive, Plotter plotter);

// These methods help to get the number after the decimal point
// Obtained from http://www.merrymeet.com/minow/sunclock/Astro.java
// Written by Martin Minow

// Returns the integer part of value
double TRUNC(double value);

// Returns the fractional part of value
double FRAC(double value);

void drawBestValuesCrossHair( const Archive * archive,
     double periodOffset_us,    // period offset in microseconds
		 double periodStep_us,      // period step in microseconds
		 double periodHalfRange_us, // period half range in microseconds
     double dmOffset,           // dm offset
		 double dmStep,             // dm step
		 double dmHalfRange);       // dm half range

// Prints the results onto the console and PGPLOT
void printResults(const Archive * archive);

// Writes important information into output files
void writeResultFiles(Archive * archive);

//////////////////////////////////
///
/// Constant definitions
///
//////////////////////////////////

// This DM constant is in milliseconds.
const double DMCONST = 4.15 * pow((double)10, (double)6);

const double MILLISEC = 1000;

const double MICROSEC = 1000000;

const unsigned FIRST_SUBINT = 0;

const unsigned FIRST_CHAN = 0;

const unsigned FIRST_POL = 0;

const float DEFAULT_BASELINE_WIDTH = 0.4;

const int NUMERIC_CHARS_LEN = 13;

const	char NUMERIC_CHARS [NUMERIC_CHARS_LEN] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', 'e', '.'};

const float PLOT_CHAR_HEIGHT = 0.8;

const float HEADER_X = 0.01; // x ordinate of the header.

const float HEADER_Y = 0.65; // y ordinate of the header.

// Approx. number of ticks to be drawn on the profile
const int NUM_VERT_PROFILE_TICKS = 4;

const unsigned DEFAULT_BEST_FIT_LINE_COLOUR = 2;

const unsigned BEST_FIT_LINE_WIDTH = 8;

// Parkes. Note: this is an array index = 7-1						
const unsigned DEFAULT_SITE_CODE_INDEX = 6;

// Display every this many percent on the progress
const unsigned SHOW_EVERY_PERCENT_COMPLETE = 5;

//////////////////////////////////
///
/// Global variables
///
//////////////////////////////////

// The Barycentric period in seconds
double dopplerFactor;

// Best values
double bestSNR;

// best period value in microseconds
double bestPeriod_bc_us;
double bestDM;
double bestFreq;
unsigned bestPulseWidth;

Reference::To<Pulsar::Profile> bestProfile;

// Errors
double periodError_ms;
double dmError;
double freqError;

// Coords of header text
float linex = HEADER_X;
float liney = HEADER_Y;

bool verbose = false;
bool force = false;
bool silent = false;

// If archive contains more than maxChannels and maxSubints, pdmp
// will partially scrunch it to improve performance
// These values are set by the -ms and -mc flags on command line
int maxChannels = -1;
int maxSubints = -1;


// The standard profile filename to compare against to compute SNR
string standardProfileFilename = "";
bool useStandardProfile = false;

Reference::To<Pulsar::Profile> std_prof;
Reference::To<StandardSNR> stdSNR;

int beginFilenamesIndex = -1;

unsigned bestFitLineColour = DEFAULT_BEST_FIT_LINE_COLOUR;

// The mappings between the site code and the array index used to get
// the site's x,y,z coordinates and name
map<char, int> siteCode2Index;

// A convenience type for the mapping
typedef pair<char, int> entry;

void usage (bool verbose_usage)
{
  cout << 
		"\nA program that searches a specified range of barycentric \n"
		"period and DM to find values giving the highest S/N ratio.\n"
    "\nUsage: pdmp [options] file1 [file2 ...]\n"
    "\n"
    "Preprocessing options:\n"
    " -do  <dm offset>               DM offset in pc/cm^3      (default=0)\n"
    " -dr  <dm half-range>           DM half-range in pc/cm^3  (default=natural)\n"
    " -ds  <dm step>                 DM step in pc/cm^3        (default=natural)\n"
		" -f , --force                   Force the program to compute without prompting\n"
		" -mc, --maxchannels <max chan>  Archive frequency channels will be \n"
		"                                partially scrunched to <= this maximum\n"
		"                                before computing \n"
		" -ms, --maxsubints <max subint> Archive subints will be \n"
		"                                partially scrunched to <= this maximum\n"
		"                                before computing\n"
    " -po  <period offset>           Period offset in us       (default=0)\n"
    " -pr  <period half-range>       Period half-range in us   (default=natural)\n"
    " -ps  <period step>             Period step in us         (default=natural)\n"
		" -s   <profile file>            Use <profile file> as a standard profile to\n"
		"                                compare with\n"
    "\n"
    "Selection & configuration options:\n"
    " -g <dev>   Manually specify a plot device\n"
    "\n"
    "Other plotting options: \n"
    " -c <index>  Select a colour map for PGIMAG style plots\n"
    "             The available indices are:\n"
    "               0 -> Greyscale\n"
    "               1 -> Inverse Greyscale\n"
    "               2 -> Heat (default)\n"
    "               3 -> Cold\n"
    "               4 -> Plasma\n"
    "               5 -> Forest\n"
    "               6 -> Alien Glow\n"
    "\n"
    "Utility options:\n"
    " -h         Display this useful help page (most useful options)\n"
		" --help     Display a complete list of help options\n"
		" -v         Verbose output\n"
		" -S         Silent mode. Reduce text written to standard output.\n"
		<< endl;
		if (verbose_usage) {
			cout <<
			" -lc <index> Select the colour index for the line of best fit\n"
			"             The available indices are:\n"
			"               0 -> Black\n"
			"               1 -> White\n"
			"               2 -> Red (default)\n"
			"               3 -> Lime green\n"
			"               4 -> Navy blue\n"
			"               5 -> Cyan\n"
			"               6 -> Pink\n"
			"               7 -> Yellow\n"
			"               8 -> Orange\n"
			"               9 -> Green\n"
			"               10 -> Aqua green\n"
			"               11 -> Blue\n"
			"               12 -> Purple\n"
			"               13 -> Pinkish red\n"
			"               14 -> Dark grey\n"
			"               15 -> Light grey\n"
    	"\n"
    	<< endl;
		}
}

void init() {

	// The Barycentric period in seconds
	dopplerFactor = -1;

	// Best values
	bestSNR = -1;

	// best period value in microseconds
	bestPeriod_bc_us = -1;
	bestDM = -1;
	bestFreq = -1;
	bestPulseWidth = 0;

	// Errors
	periodError_ms = -1;
	dmError = -1;
	freqError = -1;

	// Coords of header text
	linex = HEADER_X;
	liney = HEADER_Y;
}


void parseParameters(int argc, char **argv, double &periodOffset_us, double &periodStep_us, double &periodHalfRange_us, 
	                double &dmOffset, double &dmStep, double &dmHalfRange, 
									string &plot_device, pgplot::ColourMap::Name &colour_map) {

	string optionArg;
	
	// If no command line argument was provided, then print out usage
	if (argc <= 1) {
		usage(false);
		exit(0);
	}
	
  // Get command line options and arguments as necessary
  for (int i = 1; i < argc; i++) {
	
		// period step
		if (strcmp(argv[i], "-ps") == 0) { 
			i++;			
			parseAndValidatePositiveDouble("-ps", argv[i], periodStep_us);			
		}
		
		// period range
		else if (strcmp(argv[i], "-pr") == 0) { 
			i++;
			parseAndValidatePositiveDouble("-pr", argv[i], periodHalfRange_us);			
		}
		
		// period offset
		else if (strcmp(argv[i], "-po") == 0) { 
			i++;
			parseAndValidateDouble("-po", argv[i], periodOffset_us);			
		}
		
		// dm step 
		else if (strcmp(argv[i], "-ds") == 0) { 
			i++;
			parseAndValidatePositiveDouble("-ds", argv[i], dmStep);	
		}
		
		// dm range
		else if (strcmp(argv[i], "-dr") == 0) { 
			i++;
			parseAndValidatePositiveDouble("-dr", argv[i], dmHalfRange);			
		}
		
		// dm offset 
		else if (strcmp(argv[i], "-do") == 0) { 
			i++;
			parseAndValidateDouble("-do", argv[i], dmOffset);			
		}

		// graph device
		else if (strcmp(argv[i], "-g") == 0) { 
			i++;
			
			plot_device = argv[i];
			
			if (verbose) {
				cout << "Using plot device " << plot_device << endl;
			}
		}
		
		// colour map 
		else if (strcmp(argv[i], "-c") == 0) { 
			i++;

			optionArg = argv[i];
			
			colour_map = (pgplot::ColourMap::Name) atoi(argv[i]);			
			
			if (verbose) {
				cout << "Setting colour map to " << colour_map << endl;
			}
		}
		
		// Best fit line colour
		else if  (strcmp(argv[i], "-lc") == 0) { 
			i++;

			optionArg = argv[i];
			
			bestFitLineColour = atoi(argv[i]);			
			
			if (verbose) {
				cout << "Setting best fit line colour index to " << bestFitLineColour << endl;
			}
		}
		
		// maximum channel limit 
		else if (strcmp(argv[i], "-mc") == 0 || strcasecmp(argv[i], "--maxchannels") == 0) { 
			i++;
			parseAndValidatePositiveInt("-mc", argv[i], maxChannels);			
		}
		// maximum subint limit 	
		else if (strcmp(argv[i], "-ms") == 0 || strcasecmp(argv[i], "--maxsubints") == 0) { 
			i++;
			parseAndValidatePositiveInt("-ms", argv[i], maxSubints);			
		}
		
		// use the standard profile
		else if (strcmp(argv[i], "-s") == 0) { 
			i++;
			
			// TODO: Check validity of file
			standardProfileFilename = argv[i];
			
			ifstream inFile;
			inFile.open(standardProfileFilename.c_str());
			if (inFile.fail()) {
				cout << "pdmp error: option -s: File " << standardProfileFilename << " doesn't exist." << endl;
				exit(1);
			}
			
			if (verbose) {
				cout << "Using the standard profile in " << standardProfileFilename << endl;
			}
			
			useStandardProfile = true;	
		}
				
		// force - Don't prompt the user about anything
		else if (strcmp(argv[i], "-f") == 0 || strcasecmp(argv[i], "--force") == 0) {
			if (verbose) {
				cout << "Setting force on" << endl;
			}
			force = true;	
		}
		
		// help! 
		else if (strcmp(argv[i], "-h") == 0) {
			usage(false);
			exit(0);
	 	}
		else if (strcasecmp(argv[i], "--help") == 0) {
			usage(true);
			exit(0);
	 	}
		
		// verbose
		else if (strcmp(argv[i], "-v") == 0) {
			if (silent) {
				cerr << "Sorry this program  verbosely silent. Please choose either verbose or silent" << endl;
				exit(1);
			}
			silent = false;
			verbose = true;
		}
		
		// silent
		else if (strcmp(argv[i], "-S") == 0) {
			if (verbose) {
				cerr << "Sorry, it is a paradox to be verbosely silent. Please choose either verbose or silent" << endl;
				exit(1);
			}
			verbose = false;
			silent = true;
		}
		
		// Handle error if there is no such option
		else if (argv[i][0] == '-') {
			fprintf(stderr, "pdmp error: No such option \"%s\"\n", argv[i]);
			exit(1);
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
	
	double periodOffset_us = 0;
	double periodStep_us = -1;
	double periodHalfRange_us = -1;
	double dmOffset = 0;
	double dmStep = -1;
	double dmHalfRange = -1;
	float lineHeight = 0.16;
	
	char c, d;
	string plot_device = "/xs";
	
	int lgraph;
	string option;
	
  vector<string> filenames;
  
  // bools of whether archive should be broken up into its subints
  vector<int> breakup_archives; 
	  
  Pulsar::Plotter plotter;
	
	plotter.set_publn(false);
  pgplot::ColourMap::Name colour_map = pgplot::ColourMap::Heat;
  
	// Get the command line parameters
	parseParameters(argc, argv,  periodOffset_us, periodStep_us, periodHalfRange_us, 
	                dmOffset, dmStep, dmHalfRange, plot_device, colour_map); 

	Pulsar::Archive::verbose = 0;


	if (verbose) {
		Pulsar::Archive::verbose = 3;
		//psrephem::verbose = 1;
		//MJD::verbose = 1;
	}
	
	// Initialised the site code to index hash table
	initSiteCode2Index();
	
	if (cpgopen(plot_device.c_str()) < 0) {
    cout << "Error: Could not open plot device" << endl;
    return -1;
  }

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
    
  plotter.set_colour_map (colour_map);

  for (unsigned ifile = 0; ifile < filenames.size(); ifile++) try {
    
    vector<Reference::To<Pulsar::Archive> > archives = 
      get_archives(filenames[ifile],breakup_archives[ifile]);

    for( unsigned iarch = 0; iarch < archives.size(); iarch++){
    
			// reset global variables		
			init();
		
      Reference::To<Pulsar::Archive> archive = archives[iarch];
						
			dopplerFactor = getDopplerFactor(archive);
			
			if (!silent) {
				cout << "\nWorking on archive " << archive->get_source() << 
				": " << archive->get_filename() << 
				endl;
			}

			if (useStandardProfile) {
				if (!silent) cout << "Using standard profile: " << standardProfileFilename << endl;
				Reference::To<Pulsar::Archive> std_arch( Pulsar::Archive::load(standardProfileFilename) );
				std_arch->fscrunch();
				std_arch->pscrunch();
				std_arch->tscrunch();
	
				std_prof = std_arch->get_Profile(FIRST_SUBINT, FIRST_POL, FIRST_CHAN);
				
				stdSNR = new StandardSNR();	
				stdSNR->set_standard(std_prof);
			}
    
			cpg_next();

			// Don't override my viewport settings!
			plotter.set_svp(false);

			///////////////////////////////////////////////////////////
			// Get the reference values
			
			archive->dedisperse();
			archive->pscrunch();
						
			///////////////////////////////////////////////////////////
			// Start searching for the best period and DM

			string emptyString = "";
			goToDMPViewPort();
			plotter.set_char_height(PLOT_CHAR_HEIGHT);

			Reference::To<Archive> partiallyScrunchedCopy = archive->clone();

			// Before entering the search loop, scrunch the archive partially
			// if necessary.
			scrunchPartially(partiallyScrunchedCopy);

			// Make sure to convert period to microseconds
			// Then solve for the best period and dm and plot the results
			solve_and_plot(partiallyScrunchedCopy, dmOffset, dmStep, dmHalfRange, periodOffset_us, periodStep_us, 
			periodHalfRange_us, plotter);
						
			// Create two copies. One for the phase time plot and one
			// for the phase vs. frequency plot.
			Reference::To<Archive> phaseTimeCopy = partiallyScrunchedCopy->clone();
			Reference::To<Archive> phaseFreqCopy = partiallyScrunchedCopy->clone();
			
			phaseTimeCopy->set_dispersion_measure(bestDM);
			phaseTimeCopy->dedisperse();
			// the archive will be fscrunched by the following method
			plotPhaseTime(phaseTimeCopy, plotter);			

			phaseFreqCopy->new_folding_period((bestPeriod_bc_us / (double)MICROSEC)*dopplerFactor);
			// the archive will be tscrunched by the following method
			plotPhaseFreq(phaseFreqCopy, plotter);			

			printResults(partiallyScrunchedCopy);			
			writeResultFiles(partiallyScrunchedCopy);
			
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

	if (!silent) printf ("\npdmp took %.2lf seconds\n", elapsed  );	
	
  return 0;
}


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


// Try to find out the best DM and Period given:
// 	DM offset, DM step, DM half-range
//  Period offset (us), Period step (us), Period half-range (us)
// NOTE: refP is Barycentric
void solve_and_plot(const Archive* archive, double dmOffset, double dmStep, double dmHalfRange, 
	double periodOffset_us, double periodStep_us, double periodHalfRange_us, Plotter plotter) {

	vector<float> SNRs;

	Reference::To<Profile> profile;
	
	float snr = 0;
	
	float maxsnr = 0;
	
	Reference::To<Archive> copy = archive->clone();	
	
	unsigned nbin = copy->get_nbin();
	unsigned nsub = copy->get_nsubint();
	unsigned nchan = copy->get_nchan();
	double refP_us = getPeriod(copy) * MICROSEC;
	double refDM = getDM(copy);
	
	///////////////
	// Get the RMS
	float rms = getRMS(copy);
	
	// PSRCHIVE normalises the amplitudes after scrunching
	// so need to compensate for this
	rms = rms / sqrt(float(nchan*nsub));
	
	//////////////
	
	//////////////////////////////////////////////////////
	// Find default step and half range if none provided

	if (dmStep < 0) 
		dmStep = getNaturalDMStep(copy, dmHalfRange);
	
	if (dmHalfRange < 0)
		dmHalfRange = getNaturalDMHalfRange(copy, dmStep);
	
	if (periodStep_us < 0) 
		periodStep_us = getNaturalPeriodStep(copy, periodHalfRange_us);
				
	if (periodHalfRange_us < 0)
		periodHalfRange_us = getNaturalperiodHalfRange(copy, periodStep_us);
	
	// Make sure that the steps are sensible. Otherwise, just do one step
	if (nchan == 1 || dmStep <= 0) {
		dmHalfRange = dmStep/2;
	}
	
	if (nsub == 1 || periodStep_us <= 0) {
		periodHalfRange_us = periodStep_us/2;
	}
	
	
	//////////////////////////////////////////////////////
	
	// Now that everything's initialised, do all the searching
	int dmBins, periodBins;
	
	// Number of bins in the DM axis
	dmBins = (int)ceil( ( fabs(dmHalfRange)*2 ) / dmStep);
	
	// Number of bins in the Period axis
	periodBins = (int)ceil( ( fabs(periodHalfRange_us)*2 ) / periodStep_us);
		
	// Print the header
	printHeader(archive, periodOffset_us, periodStep_us, periodHalfRange_us,
	           dmOffset, dmStep, dmHalfRange);
			
	// Revert back to the DM vs P vs Intensity viewport
	goToDMPViewPort();
	
	double minDM = refDM + dmOffset -	fabs(dmStep*(floor((double)dmBins/2))); 	
	double currDM = minDM;
	
	double minP = refP_us +	periodOffset_us - fabs(periodStep_us*(floor((double)periodBins/2)));
	double currP = minP;
	
	double maxDM = minDM + dmStep * dmBins;
	
	double maxP = minP + periodStep_us * periodBins;

	if (!silent) cout << "Searching for optimum DM and Period...";
	
	// Begin the search for optimum DM and Period
	// Foreach DM
	for (int dmBin = 0; dmBin < dmBins ; dmBin++) {
		
		Reference::To<Archive> dmLoopCopy = copy->clone();

		dmLoopCopy->set_dispersion_measure(currDM);
		dmLoopCopy->dedisperse();
				
		dmLoopCopy->fscrunch();
		
		// Foreach Period (include one extra period bin at the end to scale with
		// the plot
		for (int periodBin = 0; periodBin <= periodBins; periodBin++) {
			
			// print out the search progress
			int percentComplete = (int)floor(100 * ((double)(periodBins*(dmBin) + periodBin) / (double)(periodBins * dmBins)));
						
			if (!(periodBin == 0 && dmBin == 0)) {
				printf("\b\b\b\b");
			}
			int displayPercentage = (int)floor((double)percentComplete/SHOW_EVERY_PERCENT_COMPLETE);
			printf("%3d%%", displayPercentage*SHOW_EVERY_PERCENT_COMPLETE);

			// Create a new (unscrunched) copy so the values can be testedget
			Reference::To<Archive> periodLoopCopy = dmLoopCopy->clone();
			
			// set the trial period and dm value and update
			double newFoldingPeriod = currP/(double)MICROSEC;
			
			// Make sure the new folding period is topocentric
			periodLoopCopy->new_folding_period(newFoldingPeriod);
			
			periodLoopCopy->tscrunch();
			
			snr = getSNR(periodLoopCopy, rms);
			
			if (verbose)	{
				printf( "\nrefP topo = %3.10g, Set P = %3.15g dP = %3.15g\n", 
					refP_us, currP, currP - refP_us, snr);
				printf( "refDM = %3.10g, Set DM = %3.15g dDM = %3.15g, SNR = %3.10g\n\n", 
					refDM, currDM, currDM - refDM, snr);
			}
			
			
			if (snr > bestSNR) {

				if (verbose) {
					fprintf(stderr, "Better SNR found: Old snr = %3.15g, New snr = %3.15g, Best DM = %3.15g and period = %3.15g \n", bestSNR, snr, currDM, newFoldingPeriod * MILLISEC);
				}

				bestSNR = snr;

				bestPeriod_bc_us = currP / dopplerFactor;

				bestDM = currDM;

				bestFreq = 1/(bestPeriod_bc_us/(double)MICROSEC);

				freqError = fabs((periodStep_us/(double)MICROSEC)/pow((bestPeriod_bc_us/(double)MICROSEC), 2));

				bestProfile = periodLoopCopy->get_Profile(FIRST_SUBINT, FIRST_POL, FIRST_CHAN);
				
				// get the width of the pulse
				int rise, fall;
				bestProfile->find_spike_edges (rise, fall);
				if (rise > fall) {
					bestPulseWidth = fall + (nbin - rise);
				}
				else {
					bestPulseWidth = fall-rise;
				}

			}	
			
			
			// put this intensity into array
			SNRs.push_back((float)snr);

			currP += periodStep_us;
		}
		
			
		// Reset the initial period value
		currP = minP;

		currDM += dmStep;
	}	
	
	// get the error
	periodError_ms = computePeriodError(archive);
	dmError = computeDMError(archive);
	
	// Now that the period step is initialised, init the freqError
	if (nsub == 1 || periodStep_us <= 0) {
		freqError = 0;
	}
	else {
		freqError = fabs((periodStep_us/(double)MICROSEC)/pow((bestPeriod_bc_us/(double)MICROSEC), 2));
	}
	
	float trf [6] = {(float)(-periodStep_us - periodHalfRange_us) + periodOffset_us, (float)periodStep_us, 0.0, (float)(minDM - 0.5*dmStep), 0.0, (float)dmStep};
	
	///////////////////////////////////////////
	// Plot the deltaPeriod vs. DM vs. SNR plot 
	// and the pulse profile plot
	
	plotter.draw_colour_map(&SNRs[0], dmBins, periodBins+1, -periodHalfRange_us+ periodOffset_us , periodHalfRange_us + periodOffset_us, "delta Period (us)", minDM, maxDM, "DM", "", trf, 5);
	
	plotProfile (bestProfile, plotter);
	
	if (verbose) {
		printf("trf = {%3.10g, %3.10g, %3.10g, %3.10g, %3.10g, %3.10g}\n", 
		-0.5*periodStep_us - periodHalfRange_us, periodStep_us, 0.0, minDM - 0.5*dmStep , 0.0,  dmStep);

		printf("minx = %3.10g, maxx = %3.10g, miny = %3.10g, maxy = %3.10g, rows = %d, cols = %d\n", minP, maxP, minDM, maxDM, dmBins, periodBins);
		cout << "number of SNRs = " << SNRs.size() << endl;
	}
	
	drawBestValuesCrossHair( archive,
     periodOffset_us, periodStep_us, periodHalfRange_us,
     dmOffset, dmStep, dmHalfRange);
}

// Use the the default natural values for offset, step and half-range
void solve_and_plot(const Archive* archive, Plotter plotter) {

	solve_and_plot(archive, 0,-1,-1, 0,-1,-1, plotter );	
	
}


void scrunchPartially(Archive * scrunchedCopy) {

	unsigned nsub = scrunchedCopy->get_nsubint();
	unsigned nchan = scrunchedCopy->get_nchan();
	unsigned nscrunch;
				
	if (nsub > maxSubints && maxSubints > 1) {
	
		// Find the largest legitimate channel limit below the user
		// specified limit
		// TODO: Make this more efficient
		for (int i = maxSubints; i > 0; i--) {
			if ( (nchan % i) == 0) {
				maxSubints = i;
				partialTimeScrunch(scrunchedCopy);
				break;
			}
		}
	}
	
	if (nchan > maxChannels && maxChannels > 1) {
		// Find the largest legitimate channel limit below the user
		// specified limit
		for (int i = maxChannels; i > 0; i--) {
			if ( (nchan % i) == 0) {
				maxChannels = i;
				partialFrequencyScrunch(scrunchedCopy);
				break;
			}
		}
	}		
}


double getNaturalDMStep(const Archive * archive, double halfRange)  {
	
	unsigned nchan = archive->get_nchan();
	
	if (halfRange < 0) {
		// give step arg value of 0 so that it gets the natural half range
		halfRange = getNaturalDMHalfRange(archive, 0);
	}
	
	return halfRange / (double)(2*nchan + 1);
	
}


double getNaturalDMHalfRange(const Archive * archive, double step)  {

	double halfRange;
	
	unsigned nchan = archive->get_nchan();
	
	if (step > 0) {
		halfRange = step * (2 * nchan + 1);	
	}
	else {

  	double centre_period = archive->get_Integration(FIRST_SUBINT)->get_folding_period() * MILLISEC;

		unsigned nbin = archive->get_nbin();

		double bw = archive->get_bandwidth();

		double bwband = fabs(bw/(double)nchan);

		double tbin = centre_period / (double)nbin;

		double fcentre = archive->get_centre_frequency();

		halfRange = tbin / (DMCONST * ( pow(fcentre - 0.5*bwband, -2) - pow(fcentre + 0.5*bwband, -2)) );
	}
	return halfRange;
}


double getNaturalPeriodStep(const Archive * archive, double halfRange)  {

	unsigned nsub = archive->get_nsubint();

	if (halfRange < 0) {
		// give step arg value of 0 so that it gets the natural half range
		halfRange = getNaturalperiodHalfRange(archive, 0);
	}
	
	return halfRange / (double)(2*nsub + 1);

}


double getNaturalperiodHalfRange(const Archive * archive, double step)  {
	
	double halfRange;
	
	if (step > 0) {
		double nsub = archive->get_nsubint();
		
		halfRange = step * (2 * nsub + 1);
	}
	else {
  	double centre_period = getPeriod(archive) * MICROSEC;

		int nbin = archive->get_nbin();

		double tsub = (archive->get_Integration(0))->get_duration() * MICROSEC;

		double tbin = centre_period / (double)nbin; // already in microseconds

		halfRange = tbin * centre_period/tsub; // in microsecs

	}
	

	if (verbose) {
		printf("Natural Half-Range = %3.10g\n", halfRange);
	}

	return halfRange;
}


// Some hardcoded viewport settings. Makes it easier to 
// add things to the window

void goToHeaderViewPort() {
	cpgsvp(0, 1.0, 0.87, 1.0);
	cpgswin(0.0, 1.0, 0.0, 1.0);
	cpgsch(0.7);
	cpgslw(1);

}

void goToDMPViewPort() {
	cpgsvp(0.05, 0.95, 0.68, 0.86);
}

void goToPhaseTimeViewPort() {			
	cpgsvp(0.05, 0.45, 0.33, 0.56);
}

void goToPhaseTimeTitleViewPort() {			
	cpgsvp(0.05, 0.45,0.565, 0.58);
	cpgswin(0.0, 1.0, 0.0, 1.0);
}

void goToPhaseFreqViewPort() {
	cpgsvp(0.55, 0.95, 0.33, 0.56);
}

void goToPhaseFreqTitleViewPort() {
	cpgsvp(0.55, 0.95, 0.565, 0.58);
	cpgswin(0.0, 1.0, 0.0, 1.0);

}

void goToBestValuesViewPort() {
	cpgsvp(0.01, 1.0, 0.185, 0.275);
	cpgswin(0.0, 1.0, 0.0, 1.0);
}


void goToProfileViewPort() {
	cpgsvp(0.05, 0.95, 0.05, 0.205);
}

float getRMS (const Archive * archive) {
	
	Reference::To<Archive> copy = archive->clone();

	double s;
	double smin = 1e30;
	int itmin;
	double minMean;
	
	unsigned nbin = copy->get_nbin();
	unsigned nchan = copy->get_nchan();
	unsigned nsub = copy->get_nsubint();
	
	double ss = 0;
	
	for (int is = 0; is < nsub ; is++) {
	
		for (int ic = 0; ic < nchan; ic++) {
			
			// find the min mean
			smin = -1;
			
			Reference::To<Profile> profile = copy->get_Profile(is, FIRST_POL, ic);
			
			vector<double> amps;
			profile->get_amps(amps);

			for (int ib = 0; ib < nbin; ib++) {
				s = 0;

				for (int j = ib; j < ib + nbin/2; j++) {
					s += amps[j%nbin];
				}

				if ((s < smin) || (smin < 0)) {
					smin = s;
					itmin = ib;
				}
			}

			minMean = smin / (nbin/2);

			for (int i = 0; i < nbin; i++) {
				amps[i] -= minMean;
			}

			for (int i = itmin; i < itmin + nbin/2 - 1; i++) {
				ss += pow(amps[i%nbin], 2);
			}
		}
	}
	
	float rms = sqrt(ss/(float)(nchan*nsub*nbin/2));
			
	return rms;
}


float getSNR (const Archive * archive, float rms) {

	Reference::To<Archive> copy = archive->clone();

	Reference::To<Profile> profile = copy->get_Profile(FIRST_SUBINT, FIRST_POL, FIRST_CHAN);
	
	return profile->snr_fortran(rms);
	
}

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

void partialTimeScrunch(Archive * archive) {
	unsigned nsub = archive->get_nsubint();
	unsigned nchan = archive->get_nchan();
	unsigned nscrunch;
	// do a partial time scrunch

	// Determine how many bins to scrunch at a time
	nscrunch = (unsigned)ceil((double)nsub/(double)maxSubints);

	if (verbose) {
		cout << "Partially time scrunching every " << nscrunch << " bins\n";
	}

	archive->tscrunch(nscrunch);

	int afterSub = archive->get_nsubint();

	if (!silent) cout << "Time scrunched from " << nsub << " subints to " << afterSub << " subints." << endl;
}

void partialFrequencyScrunch(Archive * archive) {
	unsigned nsub = archive->get_nsubint();
	unsigned nchan = archive->get_nchan();
	unsigned nscrunch;

	// do a partial frequency scrunch

	nscrunch = (unsigned)ceil((double)nchan/(double)maxChannels);

	if (verbose) {
		cout << "Partially frequency scrunching every " << nscrunch << " bins\n";
	}
	
	archive->fscrunch(nscrunch);

	int afterChan = archive->get_nchan();

	if (!silent) cout << "Frequency scrunched from " << nchan << " channels to " << afterChan << " channels." << endl;
}

double computePeriodError(const Archive * archive) {

	double pulseWidth_ms = 0;
	int rise, fall;
	double error = 0;
	
	double bcPeriod_s = getPeriod(archive) / dopplerFactor;	
	double refP_ms = bcPeriod_s * MILLISEC;	
	double nsub = archive->get_nsubint();	
	double tsub = archive->get_Integration(0)->get_duration();	
	double integrationLength = tsub * (nsub - 1);	
	unsigned nbin = archive->get_nbin();	
	double tbin_ms = (refP_ms) / nbin;
	
	Reference::To<Archive> copy = archive->clone();
	
	pulseWidth_ms = bestPulseWidth * tbin_ms;
	
	error = pulseWidth_ms / bestSNR;
	
	if (error < (bcPeriod_s / nbin ) * 0.5 * MILLISEC) {
		if (verbose) {
			printf("computeDMError: error of %3.10g < %3.10g\n",
			error, (bcPeriod_s / nbin ) * 0.5 * MILLISEC);
		}
		error = (bcPeriod_s / nbin ) * 0.5 * MILLISEC;
	}
	
	if (integrationLength > 0) {
		return (error*refP_ms) / (integrationLength * MILLISEC);
	}
	else if (integrationLength < 0) {
		throw "Error: nsub <= 0?";
	}
	else {
		return 0;
	}
}


double computeDMError(const Archive * archive) {

	double pulseWidth_ms = 0;
	int rise, fall;
	double error = 0;
	double frequency1 = 0;
	double frequency2 = 0;
	
	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	unsigned nbin = archive->get_nbin();
	unsigned nchan = archive->get_nchan();
	
	Reference::To<Archive> copy = archive->clone();
	
	double channelBW = copy->get_bandwidth() / copy->get_nchan();
	
	double refP_ms = bcPeriod_s * MILLISEC;
	
	double refDM = getDM(archive);

	double tbin_ms = (refP_ms) / nbin;
	
	pulseWidth_ms = bestPulseWidth * tbin_ms;
	
	error = pulseWidth_ms / bestSNR;
	
	if (error < (bcPeriod_s / nbin ) * 0.5 * MILLISEC) {
		if (verbose) {
			printf("computeDMError: error of %3.10g < %3.10g\n",
			error, (bcPeriod_s / nbin ) * 0.5 * MILLISEC);
		}
		error = (bcPeriod_s / nbin ) * 0.5 * MILLISEC;
	}
	
	frequency1 = copy->get_centre_frequency() + 0.5 * fabs(copy->get_bandwidth()) - fabs(channelBW/2);
	frequency2 = copy->get_centre_frequency() - 0.5 * fabs(copy->get_bandwidth()) + fabs(channelBW/2);
		
	double deltaf = frequency1 - frequency2;
	
	if (nchan > 1) {
		return error / ( (8.297616/MILLISEC) * deltaf * pow(copy->get_centre_frequency()/(double)1000, -3));
	}
	else if (nchan <= 0){
		throw "nchan <= 0?";
	}
	else {
		return 0;
	}
	
	// This is an error for a curved fit to the phase vs freq plot
	// return fabs(error / (DMCONST * (pow(frequency1, -2) - pow(frequency2, -2))));

}

void slaPvobs ( double p, double h, double stl, double pv[6] )
{
	double SR = 7.292115855306589e-5;  /* Mean sidereal rate (at J2000)
                                    in radians per (UT1) second */

   double r, z, s, c, v;

/* Geodetic to geocentric conversion */
   slaGeoc ( p, h, &r, &z );

/* Functions of ST */
   s = sin ( stl );
   c = cos ( stl );

/* Speed */
   v = SR * r;

/* Position */
   pv[0] = r * c;
   pv[1] = r * s;
   pv[2] = z;

/* Velocity */
   pv[3] = - v * s;
   pv[4] = v * c;
   pv[5] = 0.0;
}

void slaGeoc ( double p, double h, double *r, double *z )
{
   double sp, cp, c, s;

/* Earth equatorial radius (metres) */
   static double a0 = 6378140.0;

/* Reference spheroid flattening factor and useful function thereof */
   static double f = 1.0 / 298.257;
   double b = ( 1.0 - f ) * ( 1.0 - f );

/* Astronomical unit in metres */
   static double au = 1.49597870e11;

/* Geodetic to geocentric conversion */
   sp = sin ( p );
   cp = cos ( p );
   c = 1.0 / sqrt ( cp * cp + b * sp * sp );
   s = b * c;
   *r = ( a0 * c + h ) * cp / au;
   *z = ( a0 * s + h ) * sp / au;
}


double getPeriod(const Archive * archive) {
	Reference::To<Archive> copy = archive->clone();
	
	Reference::To<Pulsar::Integration> subint = copy->get_Integration((int)(copy->get_nsubint()/(double)2));
	
	if (verbose) {
		MJD start = subint->get_start_time();

		MJD end = subint->get_end_time();
	
		printf("pdmp: getPeriod: start_time = %3.10g, end_time = %3.10g\n",
		start.intday() + start.fracday(), end.intday() + end.fracday());
	}
	
	return subint->get_folding_period();
}


double getDM(const Archive * archive) {
	return archive->get_dispersion_measure();	
}

void initSiteCode2Index() {
	siteCode2Index.insert(entry('1', 0));
	siteCode2Index.insert(entry('2', 1));
	siteCode2Index.insert(entry('3', 2));
	siteCode2Index.insert(entry('4', 3));
	siteCode2Index.insert(entry('5', 4));
	siteCode2Index.insert(entry('6', 5));
	siteCode2Index.insert(entry('7', 6));
	siteCode2Index.insert(entry('8', 7));
	siteCode2Index.insert(entry('9', 8));
	siteCode2Index.insert(entry('a', 9));
	siteCode2Index.insert(entry('b', 10));
	siteCode2Index.insert(entry('c', 11));
	siteCode2Index.insert(entry('d', 12));
	siteCode2Index.insert(entry('e', 13));
	siteCode2Index.insert(entry('f', 14));
	siteCode2Index.insert(entry('g', 15));
	siteCode2Index.insert(entry('h', 16));
	siteCode2Index.insert(entry('i', 17));
}

int getSiteCodeIndex(char site_code) {
	
	map<char,int>::iterator iter;

	iter = siteCode2Index.find(site_code);
	
	if (iter != siteCode2Index.end()) {
  	return iter->second;
  }
  cerr << "pdmp: Error: No known site code '" << site_code << 
		"'. Defaulting to Parkes site code" <<endl;
  
	return DEFAULT_SITE_CODE_INDEX;
}

double getDopplerFactor(const Archive * archive) {
	
	// This hardcoded bit might not be such a good idea.
	// Counting on the fact that these values don't change
	
	string CTEL [] = { "GBT XYZ"          , "NARRABRI CS08", 
	                   "ARECIBO XYZ (JPL)", "Hobart, Tasmania",
	                   "Nanshan,Urumqi"   , "DSS 43 XYZ", 
										 "PARKES XYZ (JER)" , "JODRELL BANK XYZ", 
										 "GB 300FT"         , "GB 140FT", 
										 "GB 85-3"          , "VLA XYZ", 
										 "NORTHERN CROSS"   , "MOST", 
										 "Nancay"           , "Effelsberg", 
										 "JODRELL BANK"     , "Fallbrook"};

	double X []    = { 882589.65          , -4752329.7000,
	                   2390490.0          , -424818.0,
										 -228310.702        , -4460892.6,
										 -4554231.5         , 3822252.643,
										 382546.30          , 382615.409,
										 382545.90          , -1601192,
										 443118.48          , -352219.00,
										 4324165.81         , 4033949.5,
										 531412             , 332235};
										 
	double Y []    = { -4924872.32        , 2790505.9340,
	                   -5564764.0         , -1472621.0,
										 4631922.905        , 2682358.9,
										 2816759.1          , -153995.683,
										 795056.36          , 795009.613,
										 795036.87          , -5041981.4,
										 -113848.48         , -1492525.00,
										 165927.11          , 486989.4,
										 21824              , 1171501};
										 
	double Z []    = { 3943729.348        , -3200483.7470,
	                   1994727.0          , 50.0,
										 4367064.059        , -3674756.0,
										 -3454036.3         , 5086051.443,
										 893.7              , 880.87,
										 835.8              , 3554871.4,
										 25                 , 500.0,
										 4670132.83         , 4900430.8,
										 78                 , 0};			

	// Astronomical units
	double AUS     = 499.004786;
	double TWOPI   = 2*M_PI;
	double RTOD    = (double)360/TWOPI;

	double dps[3];
	double dvb[3];
	double dpb[3];
	double dvh[3];
	double dph[3];
	double dpve[6];

	int hours, degrees, minutes;
	double seconds, ra_secs, ra_degs, dec_degs;
	double R2000, D2000;
	
	Reference::To<Archive> copy = archive->clone();

	double start = copy->start_time().intday() + copy->start_time().fracday();
	double end = copy->end_time().intday() + copy->end_time().fracday();

	double tmjdctr = 0.5 * (end + start) * (double)86400;

	Angle ra_angle = (copy->get_coordinates()).ra();
	Angle dec_angle = (copy->get_coordinates()).dec();
	
	ra_angle.getHMS(hours, minutes, seconds);
	if (verbose) {
		printf("pdmp: getDopplerFactor: mjd_start = %3.10g, mjd_end = %3.10g, ra_hours = %d, ra_min = %d, ra_sec = %3.10g\n",
		start, 
		end, 
		hours, minutes, seconds);
	}
	ra_secs = hours * 3600 + minutes * 60 + seconds;

	dec_angle.getDMS(degrees, minutes, seconds);

	dec_degs = dec_angle.getDegrees();
	
	if (verbose) {
		printf("pdmp: getDopplerFactor: ra_secs = %3.15g, dec_degs = %d, dec_min = %d, dec_sec = %3.10g, dec.getDegrees() = %3.15g\n",
		ra_secs, degrees, minutes, seconds, dec_degs);
	}
	
	// Convert it to degrees
	ra_degs = ra_secs / (double)240; // == (ra_secs/(24*3600)) * 360
		
  // Convert to radians
  R2000 = ra_degs/(double)RTOD;
  D2000 = dec_degs/(double)RTOD;

	if (verbose) {
		printf("pdmp: getDopplerFactor: RTOD = %3.15g, R2000 = %3.15g, D2000 = %3.15g\n",
		RTOD, R2000, D2000 );
	}

	// Get the site index	
	char site_code = copy->get_telescope_code();
	int index = getSiteCodeIndex(site_code);
	
	if (verbose)
		cout << "pdmp: getDopplerFactor: Archive observed from the telescope: " << CTEL[index] << endl;

	// Get the observatory position wrt the geocentre
	double x = X[index]; // in metres
	double y = Y[index];
	double z = Z[index];
	
	// Convert to DCS
  slaDcs2c (R2000, D2000, dps);

	// Convert the time to TDT.
	double tdt = tmjdctr + slaDtt (tmjdctr/(double)86400);

	// Convert UTC to TDB.
  double g = (357.53 + 0.9856003 * (tmjdctr/(double)86400 - 51544.5)) / RTOD;
  double tdb = tdt + 0.001658 * sin (g) + 0.000014 * sin(2 * g);
	
	slaEvp (tdb/(double)86400, (double)2000, dvb, dpb, dvh, dph);
	
	double sitera = 0;
	
	// The velocity of the observatory
	double site_vel[3];
	
	// Cylindrical coordinates of observatory
	double site[3];
	
	/////////////////////////////////
	///// Based on tempo's setup.f
	////////////
	double erad = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));	
	
	double siteLatitude = asin(z/erad);
	
	double siteLongitude = atan2(-y, x);
	
	double sphericalRadius = erad/(2.99792458e8*AUS);
	
	if (verbose) {
		
		printf("pdmp: getDopplerFactor: tmjdctr = %3.10g, radius = %3.10g, siteLatitude = %3.10g, siteLongitude = %3.10g, sphericalRadius = %3.10g\n" , 
		0.5 * (end + start), erad, siteLatitude, siteLongitude, sphericalRadius);
		
		// unsigned nsub = copy->get_nsubint();
// 		int centre_nsub = (int)ceil((double)nsub / 2) - 1;
// 		Reference::To<Pulsar::Pointing> pointing = copy->get_Integration(centre_nsub)->get<Pulsar::Pointing>();
// 		double tsid = pointing->get_local_sidereal_time();
// 		printf("pdmp: using Pointing to get sidereal time at [(%d/2 - 1) = %d] = %3.15g\n", 
// 		nsub, centre_nsub, tsid);
	}
	
	/////////////////////////////////
	///// Based on tempo's arrtim.f
	////////////
	
	site[0] = sphericalRadius * cos(siteLatitude) * AUS;	
	site[1] = site[0] * tan(siteLatitude);	
	site[2] = siteLongitude;
	
	if (verbose) {
		printf("pdmp: getDopplerFactor: site[0] = %3.10g, site[1] = %3.10g, site[2] = %3.10g\n" , 
		site[0], site[1], site[2]);
	}
	
	/////////////////////////////////
	///// Based on tempo's obsite.f
	////////////
	
	double LST_der;
	double LST;
	// Set longitude to 0 because we want the 
	lmst(0.5 * (end + start), 0, &LST, &LST_der);
	
	LST *= TWOPI;
	
	double ph = LST - site[2];
	
	double EEQ[3];
	
	EEQ[0] = site[0] * cos(ph);
	EEQ[1] = site[0] * sin(ph);
	EEQ[2] = site[1];

	sitera = atan2(EEQ[1], EEQ[0]);

	if (verbose) {
		printf("pdmp: getDopplerFactor: LST = %3.10g, ph = %3.10g, EEQ[0] = %3.10g, EEQ[1] = %3.10g, EEQ[2] = %3.10g, sitera = %3.10g\n" , 
		LST, ph, EEQ[0], EEQ[1], EEQ[2], sitera);
	}
	
	double speed = TWOPI*site[0]/((double)86400/ 1.00273);
	
	site_vel[0] = -speed * sin(sitera);	
	site_vel[1] =  speed * cos(sitera);	
	site_vel[2] = 0;
		
	if (verbose) {
		printf("pdmp: getDopplerFactor: site_vel[0] = %3.10g, site_vel[1] = %3.10g, site_vel[2] = %3.10g, speed = %3.10g\n" , 
		site_vel[0], site_vel[1], site_vel[2], speed);
	}

	/////////////////////////////////
	///// Based on psrephd.f
	////////////
	
	// Add the EC-observatory vectors to the barycentre-EC vectors.
  for (int i = 0; i < 3; i++) {
		
		// Convert from AUS per second to metres per second
		dvb[i] *= AUS;	
	  dvb[i] = dvb[i] + site_vel[i];
	}
	
	double evel = slaDvdv(dvb, dps);
	
	if (verbose) {
		printf("pdmp: getDopplerFactor: dopps = %3.20g\n", 1-evel);
	}

	return 1-evel;
}

double TRUNC(double value) {
	double result = floor(abs(value));
	if (value < 0.0)
		result = (-result);
	return (result);
}

double FRAC(double value) {
	double result = value - TRUNC(value);
	if (result < 0.0)
		result += 1.0;
	return (result);
}


/* ******************************************** */
/* lmst                                         */
/* Author:  G. Hobbs (06 May 2003)              */
/* Purpose: Calculates the local mean sidereal  */
/*          time (tsid) and its derivative      */
/*                                              */
/* Inputs:                                      */
/* Outputs: tsid, tsid_der                      */
/*                                              */
/* Notes: based on lmst.f                       */
/*        NONE of this has been checked         */
/* Changes:                                     */
/* ******************************************** */

double lmst(double mjd,double olong,double *tsid,double *tsid_der)
{
  double xlst,sdd;
  double gmst0;
  double a = 24110.54841;
  double b = 8640184.812866;
  double c = 0.093104;
  double d = -6.2e-6;
  double bprime,cprime,dprime;
  double tu0,fmjdu1,dtu,tu,seconds_per_jc,gst;
  int nmjdu1;

  nmjdu1 = (int)mjd;
  fmjdu1 = mjd - nmjdu1;

  tu0 = ((double)(nmjdu1-51545)+0.5)/3.6525e4;
  dtu  =fmjdu1/3.6525e4;
  tu = tu0+dtu;
  gmst0 = (a + tu0*(b+tu0*(c+tu0*d)))/86400.0;
  seconds_per_jc = 86400.0*36525.0;

  bprime = 1.0 + b/seconds_per_jc;
  cprime = 2.0 * c/seconds_per_jc;
  dprime = 3.0 * d/seconds_per_jc;

  sdd = bprime+tu*(cprime+tu*dprime);

  gst = gmst0 + dtu*(seconds_per_jc + b + c*(tu+tu0) + d*(tu*tu+tu*tu0+tu0*tu0))/86400;
  xlst = gst - olong/360.0;
	
  xlst = fortran_mod(xlst,1.0);

  if (xlst<0.0)xlst=xlst+1.0;

  *tsid = xlst;
  *tsid_der = sdd;
  return 0.0;
}

/* Emulate the Fortran "mod" operator */
double fortran_mod(double a,double p)
{
  double ret;
  ret = a - (int)(a/p)*p;
  return ret;
}

//////////////////////////////////////////////////////////////////////////
////////////////
//////////////// Drawing and printing methods
////////////////
//////////////////////////////////////////////////////////////////////////

void printHeader(const Archive * archive,
                 double periodOffset_us, 
								 double periodStep_us, 
								 double periodHalfRange_us,
								 double dmOffset,
								 double dmStep,
								 double dmHalfRange) {

	/////////////////////////////////////////////////
	// Print the header
	//

	float lineHeight = 0.16;
	char temp [1024];
	string temp_str;
	string space = "  ";

	// Set the view port to the top row to display the header information
	goToHeaderViewPort();

	Reference::To<Archive> copy = archive->clone();

	// Display the pulsar name and archive file
	temp_str = copy->get_source() + ": " + copy->get_filename();
	cpgtext(linex, 0.85, temp_str.c_str());

	// reset the y ordinate for the text when looping over archives
	liney = HEADER_Y;

	
	double refP_us = getPeriod(copy) * MICROSEC;
	double refDM = getDM(copy);
	int nBin = copy->get_nbin();
	int nChan = copy->get_nchan();
	int nSub = copy->get_nsubint();
	double tspan = copy->integration_length();
	double tsub = (copy->get_Integration(0))->get_duration();
	double tbin = (refP_us/1000) / nBin;
	
	Reference::To<Pulsar::Integration> subint = copy->get_Integration((int)floor(copy->get_nsubint()/(double)2));
	MJD start = subint->get_start_time();

	int hours, degrees, ra_minutes, dec_minutes;
	double ra_seconds, dec_seconds;

	Angle ra_angle = (copy->get_coordinates()).ra();
	Angle dec_angle = (copy->get_coordinates()).dec();
	
	double dec_degs = dec_angle.getDegrees();
	
	ra_angle.getHMS(hours, ra_minutes, ra_seconds);
	dec_angle.getDMS(degrees, dec_minutes, dec_seconds);
	
	if (dec_degs < 0) {
		degrees = -degrees;
	}

	////////////////////////////////////////
	/// Print the reference period and dm
	
	sprintf(temp, "Ref BC period (ms) = %3.9f%sRef TC period (ms) = %3.9f%sRef DM = %3.3f%sRAJ = %02d:%02d:%05.2f%sDecJ = %02d:%02d:%04.1f",
		(refP_us/1000) / dopplerFactor, space.c_str(), 
		getPeriod(copy) * MILLISEC,
		space.c_str(),
		refDM,
		space.c_str(),
		hours, ra_minutes, ra_seconds,
		space.c_str(),
		degrees, dec_minutes, dec_seconds);
	
	cpgtext(linex, liney, temp);
	liney -= lineHeight;

	///////////////////////////////
	/// Print the MJF and frequency
	
	// Newline
	sprintf(temp, "Ref BC MJD = %.6f" , 
	start.intday() + start.fracday());
	temp_str = temp;

	sprintf(temp, "Centre freq. (MHz) = %3.3f",copy->get_centre_frequency());
	temp_str += space + temp;

	sprintf(temp, "Bandwidth (MHz) = %3.9g",
	copy->get_bandwidth());
	temp_str += space + temp;

	cpgtext(linex, liney, temp_str.c_str());
	
	//////////////////////////////////////////
	/// New line for bin counts and bin times
	
	liney -= lineHeight;

	sprintf(temp , "NBin = %d%sNChan = %d%sNSub = %d%s", nBin, space.c_str(), nChan,
	space.c_str(),	nSub, space.c_str());			
	temp_str = temp;

	sprintf(temp , "TBin(ms) = %.3f%sTSub(s) = %.3f%sTSpan(s) = %.3f%s",  tbin ,
	space.c_str(),	tsub, space.c_str(), tspan, space.c_str());			
	temp_str += temp;

	// Print the line
	cpgtext(linex, liney, temp_str.c_str());

	liney -= lineHeight;

	////////////////////////////////////////////////////////
	/// Print the Period and DM offset, step and half-range
	
	// Newline
	sprintf(temp, "P (us): offset = %3.6f, step = %3.6f, half-range = %3.6f", 
	 	periodOffset_us, periodStep_us, periodHalfRange_us);
	
	temp_str = temp;
		
	sprintf(temp, "%s%sDM: offset = %3.3f, step = %3.3f, half-range = %3.3f", 
		space.c_str(), space.c_str(), dmOffset, dmStep, dmHalfRange);
	
	temp_str += temp;
	cpgtext(linex, liney, temp_str.c_str());
	

}

void printResults(const Archive * archive) {

	/////////////////////////////////////////////////
	// Print the best values and corrections
	
	char temp [512];
	Reference::To<Archive> copy = archive->clone();

	double bcPeriod_s = getPeriod(archive) / dopplerFactor;

	double refDM = getDM(archive);
	double refP_us = bcPeriod_s * MICROSEC;

	// Now that the best values are discovered			
	goToBestValuesViewPort();

	cpgsch(0.7);
	cpgslw(1);
	float lineHeight = 0.2;
	float colWidth = 0.16;

	float bestValues_x = 0;
	float bestValues_y = 1;
	
	unsigned nbin = archive->get_nbin();
	double tbin = (bestPeriod_bc_us/1000) / nbin;
	
	const float RIGHT_JUSTIFY = 1;
	const float HORIZONTAL = 0;

	// Firstly print out the first column of labels
	cpgtext(bestValues_x, bestValues_y, "Best BC period (ms):");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Correction (ms):");

	bestValues_y -= lineHeight;
  cpgtext(bestValues_x, bestValues_y, "Error (ms):");

	// Next, print out all the values
	bestValues_x += colWidth*1.7;
	bestValues_y = 1;

	sprintf(temp, "%3.9f", bestPeriod_bc_us / MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", (bestPeriod_bc_us-refP_us) / MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", periodError_ms);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);
	
	// New column
	bestValues_x += colWidth * 0.1;
	bestValues_y = 1;
	
	cpgtext(bestValues_x, bestValues_y, "Best TC period (ms):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Correction (ms):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Error (ms):");
	
	bestValues_x += colWidth * 1.7;
	bestValues_y = 1;	
	
	sprintf(temp, "%3.9f", dopplerFactor * bestPeriod_bc_us / MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", (dopplerFactor * bestPeriod_bc_us / MILLISEC) - getPeriod(copy) * MILLISEC);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.9f", periodError_ms);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	
	// New column
	bestValues_x += colWidth * 0.1;
	bestValues_y = 1;
	cpgtext(bestValues_x, bestValues_y, "Best DM:");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Correction:");

	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Error:");
	
  // New column: Print out all the values
	bestValues_x += colWidth * 0.8;
	bestValues_y = 1;
	sprintf(temp, "%3.3f", bestDM);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.3f", bestDM-refDM);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.3f", dmError);
	cpgptxt(bestValues_x, bestValues_y, HORIZONTAL, RIGHT_JUSTIFY, temp);
	

	// New column
	bestValues_x += colWidth * 0.1;
	bestValues_y = 1;
	
	cpgtext(bestValues_x, bestValues_y, "Best BC freq (Hz):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Freq error (Hz):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Pulse width (ms):");
	bestValues_y -= lineHeight;
	cpgtext(bestValues_x, bestValues_y, "Best SNR:");
	
	bestValues_x += colWidth * 0.8;
	bestValues_y = 1;	
	
	sprintf(temp, "%-3.9f", bestFreq);
	cpgtext(bestValues_x, bestValues_y, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%-3.9f", freqError);
	cpgtext(bestValues_x, bestValues_y, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%3.3f", bestPulseWidth * tbin);
	cpgtext(bestValues_x, bestValues_y, temp);

	bestValues_y -= lineHeight;
	sprintf(temp, "%-3.3f", bestSNR);
	cpgtext(bestValues_x, bestValues_y, temp);


	// Print out the results on the console
	if (!silent) {
		cout << "\n\nBest SNR = " << bestSNR << endl;
		
		printf("Ref BC Period (ms) = %3.15g  Ref TC Period (ms) =  %3.10g  Ref DM = %3.10g\n", 
						bcPeriod_s * MILLISEC,	getPeriod(copy) * MILLISEC, getDM(copy)); 

		printf("Best BC Period (ms) = %3.15g  Correction (ms) = %3.10g  Error (ms) = %3.10g\n", 
						bestPeriod_bc_us / MILLISEC,	(bestPeriod_bc_us-refP_us) / MILLISEC, periodError_ms / MILLISEC); 
		
		printf("Best TC Period (ms) = %3.15g  Correction (ms) = %3.10g  Error (ms) = %3.10g\n", 
						dopplerFactor * bestPeriod_bc_us / MILLISEC,	
						(dopplerFactor * bestPeriod_bc_us / MILLISEC) - (getPeriod(copy) * MILLISEC), 
						periodError_ms / MILLISEC); 
		
		printf("Best DM = %3.15g  Correction = %3.10g  Error = %3.10g\n", 
	        	bestDM, bestDM-refDM, dmError);
		
		printf("Best BC Frequency (Hz) = %3.15g  Error (Hz) = %3.10g\n", 
	        	bestFreq, freqError);
		printf("Pulse width (bins) = %d\n", 
	        	bestPulseWidth);
	}
	
}

void writeResultFiles(Archive * archive) {

	double bcPeriod_s = getPeriod(archive) / dopplerFactor;	
	double refP_ms = bcPeriod_s * MILLISEC;	
	unsigned nbin = archive->get_nbin();	
	double tbin_ms = (refP_ms) / nbin;
	double glong, glat;

	FILE *file;

	AnglePair glgb = (archive->get_coordinates()).getGalactic();
	
	glgb.getDegrees(&glong, &glat);
	
	ofstream posnFile("pdmp.posn", ios::out | ios::app);

	if (posnFile.is_open()) {
		posnFile << archive->get_source() << "\t" << glong << "\t" << glat << "\t" << bestSNR << "\t" << 
		archive->start_time() << "\t" << bestPeriod_bc_us << "\t" << periodError_ms << "\t" << 
		bestDM << "\t" << dmError << "\t" << archive->get_filename() << endl;		

		posnFile.close();
	}
	else {
		cerr << "pdmp: Failed to open file pdmp.posn for writing results\n";
	}
	
	file = fopen("pdmp.per", "at");

	if (file != NULL) {
		fprintf(file, " %3.6f\t%3.20f\t%3.20f\t%3.6f\t%3.6f\t%3.3f\t%3.3f\t%s\n",
		archive->start_time().intday() + archive->start_time().fracday(),
		bestPeriod_bc_us/MILLISEC, 
		periodError_ms,
		bestDM, 
		dmError,
		bestPulseWidth * tbin_ms,
		bestSNR, 
		archive->get_filename().c_str());
	}
	else {
		cerr << "pdmp: Failed to open file pdmp.per for writing results\n";
	}
	fclose(file);
}

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

void plotPhaseFreq(const Archive * archive, Plotter plotter) {
	// Plot the phase vs frequency colour map
	Reference::To<Archive> phase_freq_copy = archive->clone();	
	
	phase_freq_copy->pscrunch();
	phase_freq_copy->tscrunch();
	
	goToPhaseFreqViewPort();
	plotter.phase_frequency (phase_freq_copy, "", "", "Channel Index", false);			
	
	cpgsci(6);
	cpgslw(8);
	drawBestFitPhaseFreq(phase_freq_copy, plotter);

	cpgslw(1);
	cpgsci(1);
	goToPhaseFreqTitleViewPort();
	cpgtext(0, 1, "Phase vs Frequency");

}

void plotPhaseTime(const Archive * archive, Plotter plotter) {

	Reference::To<Archive> phase_time_copy = archive->clone();	

	phase_time_copy->fscrunch();
	phase_time_copy->pscrunch();
	
	// Plot the phase vs time colour map
	goToPhaseTimeViewPort();
	plotter.set_char_height(PLOT_CHAR_HEIGHT);
	plotter.phase_time (phase_time_copy, "", "", "Subints");
	
	// Draw the line of best fit of the original
	drawBestFitPhaseTime(phase_time_copy, plotter);

	// reset the colour and size of the line back to its original
	cpgslw(1);
	cpgsci(1);
	goToPhaseTimeTitleViewPort();
	cpgtext(0, 1, "Phase vs Time");
	
}


void drawBestFitPhaseFreq(const Archive * archive, Plotter plotter) {
	
	int rise, fall, centre, refCentreBin, refCentreChan, deltaBin;

	Reference::To<Archive> copy = archive->clone();
	
	Reference::To<Profile> profile;

	unsigned nbin = archive->get_nbin();
	
	unsigned nchan = archive->get_nchan();
	
	double dm = getDM(copy);
	
	double bw = archive->get_bandwidth();
	
	double bwband = fabs(bw/(double)nchan);
	
	double fspan = fabs((nchan) * bwband);

	double centre_freq = archive->get_centre_frequency();
	
	double deltaDM = bestDM - dm;	
	
	double maxmax = 0;

	double max;
	
	unsigned peakWidth;
	
	int minPeakWidth = -1;
	
	vector<float> xpoints;
	
	vector<float> ypoints;

	double refPhase = 0.5;
	
	// Note that DMCONST is in milliseconds
	// Note that this deltaPhase refers to the midpoint of the 
	// first channel to the midpoint of the second channel
	double deltaPhase = (deltaDM * DMCONST * 
	                     (pow(centre_freq - 0.5*fspan + bwband/2, -2) - 
											  pow(centre_freq + 0.5*fspan - bwband/2, -2) ) ) / 
												(bestPeriod_bc_us/MILLISEC);
	double gradient;
	
	if (nchan == 1) {
		deltaPhase = 0;
		gradient = 0;
	}
	else {
		gradient = deltaPhase / (nchan - 1);
	}
	
	// get the deltaPhase for the entire length of the freq channels
	// This ensures a nice line going from the top of the phase-freq
	// plot to the bottom instead of midway between the top and bottom
	// channels.
	deltaPhase = gradient * nchan;
	
	// The first point is the centre of the first subint
	xpoints.push_back(refPhase);
	ypoints.push_back(FIRST_CHAN);

	// Second point is the point on the last subint
	// which is offseted by delta phase
	xpoints.push_back(refPhase + deltaPhase);
	ypoints.push_back(nchan);

	cpgswin(0,1,FIRST_CHAN,nchan);
	if (verbose) {
		printf("Drawing line of best fit for phase frequency with coords: (%3.10g, %3.10g) to (%3.10g, %3.10g)\n",
		xpoints[0], ypoints[0], xpoints[1], ypoints[1]);
	} 
	
	cpgsci(bestFitLineColour);
	cpgslw(BEST_FIT_LINE_WIDTH);
	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);
	
	// reset the colour and width back to original
	cpgsci(1);
	cpgslw(1);

}


void drawBestFitPhaseTime(const Archive * archive, Plotter plotter) {

	int rise, fall, centre, refCentreBin, refCentreSubint, deltaBin;

	Reference::To<Archive> copy = archive->clone();
	Reference::To<Profile> profile;

	double bcPeriod_s = getPeriod(archive) / dopplerFactor;
	
	double p = bcPeriod_s*MICROSEC;
	
	double deltaP = bestPeriod_bc_us - p;
	
	unsigned nsub = archive->get_nsubint();
	
	unsigned nbin = archive->get_nbin();
	
	double tsub = archive->get_Integration(0)->get_duration();

	double tspan = tsub / (nsub-1);
	
	double maxmax = 0;

	double max;
	
	unsigned peakWidth;
	
	int minPeakWidth = -1;

	vector<float> xpoints;
	
	vector<float> ypoints;

	double refPhase = 0.5;

	// This equation is derived from the property
	// deltaP / P = deltaT / T
	// Note that this deltaPhase refers to the midpoint of the 
	// first subint to the midpoint of the second subint
	double deltaPhase = ((nsub-1) * tsub * (deltaP / MICROSEC)) / pow((bestPeriod_bc_us/MICROSEC), 2);
	
	double gradient;
	
	if (nsub == 1) {
		deltaPhase = 0;
		gradient = 0;
		
	}
	else {
		gradient = deltaPhase / (nsub - 1);
	}
		
	// get the deltaPhase for the entire length of the subints
	// This draws a nice line from top to bottom, instead of midway
	// through the first and last subints
	deltaPhase = gradient * nsub;

	if (verbose) {
		printf("deltaPhase = %3.10g, deltaP = %3.10g, bestPeriod_bc_us = %3.10g\n", deltaPhase, deltaP, bestPeriod_bc_us);
	}
	
	// The first point is the centre of the first subint
	xpoints.push_back(refPhase);
	ypoints.push_back(FIRST_SUBINT);

	// Second point is the point on the last subint
	// which is offseted by delta phase
	xpoints.push_back(refPhase + deltaPhase);
	ypoints.push_back(nsub);
		
	if (verbose) {
		printf("Drawing line of best fit for phase time with coords: (%3.10g, %3.10g) to (%3.10g, %3.10g)\n",
		xpoints[0], ypoints[0], xpoints[1], ypoints[1]);
	}
	
	cpgsci(bestFitLineColour);
	cpgslw(BEST_FIT_LINE_WIDTH);
	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);
	
	// reset the colour and width back to original
	cpgsci(1);
	cpgslw(1);
}

void drawBestValuesCrossHair( const Archive * archive,
     double periodOffset_us, double periodStep_us, double periodHalfRange_us,
     double dmOffset, double dmStep, double dmHalfRange) {

	goToDMPViewPort();
	cpgsci(bestFitLineColour);
	cpgslw(1);
	
	// In microsec
	double bcPeriod_us = (getPeriod(archive) / dopplerFactor) * MICROSEC;
	
	vector<float> xpoints;
	
	vector<float> ypoints;
	
	double minDM = getDM(archive) + dmOffset -	dmHalfRange; 	
	double maxDM = getDM(archive) + dmOffset +	dmHalfRange;
	
	// Get the initial minimum search period as a starting point. The following
	// way of getting minP ensures that the reference P is used to calculate
	// SNR as, in some cases, the optimal SNR is found using the ref P and DM
	double minP = -periodHalfRange_us+ periodOffset_us;
	double maxP = periodHalfRange_us + periodOffset_us;

	cpgswin(minP,maxP,minDM,maxDM);

	// Draw vertical line
	xpoints.push_back((bestPeriod_bc_us - bcPeriod_us));
	ypoints.push_back(minDM);

	xpoints.push_back((bestPeriod_bc_us - bcPeriod_us));
	ypoints.push_back(maxDM);
	
	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);
	
	xpoints.clear();
	ypoints.clear();
	
	// Draw vertical line
	xpoints.push_back(minP);
	ypoints.push_back(bestDM);

	xpoints.push_back(maxP);
	ypoints.push_back(bestDM);
	
	cpgline(xpoints.size(), &xpoints[0], &ypoints[0]);
	
	// reset the colour back to original
	cpgsci(1);
}
