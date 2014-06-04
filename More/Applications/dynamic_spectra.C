/***************************************************************************
 *
 *   Copyright (C) 2013 by Stefan Oslowski
 *   Licensed under the Academic Free License version 3.0
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/TimeAppend.h"
#include "Pulsar/StandardSNR.h"
#include "FTransform.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace Pulsar;

//
//! An application for calculatin dynamic and secondary spectra
//
class dynamic_spectra : public Pulsar::Application
{
public:

  //! Default constructor
  dynamic_spectra ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  void finalize ();

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! Compute the dynamic spectrum
  void compute_dynamic_spectrum();

  // save results:
  ofstream outfile;
  void save_dynamic_spectrum(string);
  void save_axes(string);
  string ds_file;
  string axis_file;

  //format of the output:
  string format_name;
  bool use_gnuplot_pm3d_format;
  bool use_gsl_format;
  bool use_matlab_format;

  //! Creating total archive:
  TimeAppend timeApp;
  Reference::To<Archive> total;
  int total_count;
  int nchan;
  int nsubint;

  //! Dynamic spectrum array
  float **ds;

  //! Computation machinery:
  StandardSNR std_snr;
};


dynamic_spectra::dynamic_spectra ()
  : Application ("dynamic_spectra", "dynamic_spectra psrchive program")
{
  add( new Pulsar::StandardOptions );
//  add( new Pulsar::PlotOptions );

  ds_file = "dynamic_spectrum.dat";
  axis_file = "dynamic_spectrum.axes";

  use_gnuplot_pm3d_format = true;
  use_gsl_format = false;
  use_matlab_format = false;

  timeApp.must_match = true;
  total_count = 0;
}

void dynamic_spectra::process (Pulsar::Archive* archive)
{
  archive->pscrunch();

  if (total_count == 0) {
    total = archive;
    timeApp.init(total);
  } else {
    timeApp.append(total, archive);
  }
  total_count++;
}

void dynamic_spectra::setup ()
{
  // choose the output format
  if (!format_name.empty())
  {
    if (format_name == string("gnuplot"))
      use_gnuplot_pm3d_format = true;
    else if (  format_name == string("gsl")) {
      use_gsl_format = true;
      use_gnuplot_pm3d_format = false;
    } else if (format_name == string("matlab")) {
      use_matlab_format = true;
      use_gnuplot_pm3d_format = false;
    } else
      throw Error (InvalidParam, "dynamic_spectra::setup",
		      "unknown format requested '" + format_name + "'");
  }
}

void dynamic_spectra::finalize ()
{
  // prepare for calculating the S/N

  Reference::To<Profile> data_avg = total->total()->get_Profile(0,0,0);
  std_snr.set_standard( data_avg );

  nchan   = total->get_nchan();
  nsubint = total->get_nsubint();

  //allocate memory for the dynamic spectrum
  ds = new float*[nsubint];
  compute_dynamic_spectrum();
  save_dynamic_spectrum(ds_file);
  if ( !axis_file.empty() )
    save_axes(axis_file);
}

static dynamic_spectra program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

void dynamic_spectra::compute_dynamic_spectrum()
{
  for ( int isub = 0; isub < nsubint; isub++ ) {
    ds[isub] = new float[nchan];
    for (int ichan = 0; ichan < nchan; ichan++ ) {
      Reference::To<const Profile> prof = total->get_Profile( isub, 0, ichan );

      if ( prof->get_weight() == 0.0 )
	ds[isub][ichan] = 0.0;
      else
	ds[isub][ichan] = std_snr.get_snr(prof);
    }
  }
}

void dynamic_spectra::save_dynamic_spectrum( string name )
{
  outfile.open( name.c_str() );

  if ( use_gnuplot_pm3d_format ) {
    streamsize ss = cout.precision();
    for (int isub = 0; isub < nsubint; isub++ ) {
      for (int ichan = 0; ichan < nchan; ichan++ ) {
	outfile << setprecision(12) << total->get_Profile(isub,0,ichan)->get_centre_frequency() << setprecision(ss) 
		<< " " << total->get_Integration(isub)->get_epoch().printdays(20)
		<< " " << ds[isub][ichan] << endl;
      }
      // add a gap for compatibility with pm3d in gnuplot
      outfile << endl;
    }
  }
  else if ( use_gsl_format ) {
    for (int isub = 0; isub < nsubint; isub++ ) {
      for (int ichan = 0; ichan < nchan; ichan++ ) {
	outfile << ds[isub][ichan] << endl;
      }
    }
  } else if ( use_matlab_format ) {
    for (int isub = 0; isub < nsubint; isub++ ) {
      for (int ichan = 0; ichan < nchan; ichan++ ) {
	if ( ichan < nchan-1 )
	  outfile << ds[isub][ichan] << " ";
	else 
	  outfile << ds[isub][ichan] << endl;
      }
    }
  }
  outfile.close();
}

void dynamic_spectra::save_axes( string name )
{
  outfile.open ( name.c_str() );
  
  streamsize ss = cout.precision();
  for (int isub = 0; isub < nsubint; isub++ ) {
    for (int ichan = 0; ichan < nchan; ichan++ ) {
      outfile << setprecision(12) << total->get_Profile(isub, 0, ichan)->get_centre_frequency() << setprecision(ss) << " " << total->get_Integration(isub)->get_epoch().printdays(20) << endl;
    }
  }
}

void dynamic_spectra::add_options ( CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add("");

  arg = menu.add( ds_file, 'o', "filename");
  arg->set_help ("output the dynamic spectrum to filename");

  arg = menu.add( axis_file, 'A', "filename");
  arg->set_help ("output abcissa and ordinate values");
 
  arg = menu.add( format_name, 'f', "format");
  arg->set_help ("Choose the output format: format = gnuplot gsl or matlab");
  arg->set_long_help ("gnuplot is the default format, suitable for pm3d plotting in gnuplot\n"
		 "gsl is one entry per line with frequency varying faster than time\n"
		 "matlab is a matrix with frequency varying with columns and time with rows");
}
