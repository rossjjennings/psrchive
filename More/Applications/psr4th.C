/***************************************************************************
 *
 *   Copyright (C) 2008 - 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/FourthMoments.h"

#include "Matrix.h"
#include "Stokes.h"

//
//! Computes the phase-resolved 4x4 covariance matrix of the Stokes parameters
//
class psr4th : public Pulsar::Application
{
public:

  //! Default constructor
  psr4th ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Output the results
  void finalize ();

protected:

  //! Array of 4x4 fourth moments - one for each pulse phase bin
  class result
  {
  public:
    std::vector< Matrix<4,4,double> > stokes_squared;
    std::vector< Stokes<double> > stokes;
    uint64_t count;

    //! Resize arrays and initialize to zero
    void resize (unsigned nbin);

    Matrix<4,4,double> get_covariance (unsigned ibin);
    Stokes<double> get_mean (unsigned ibin);
  };

  //! Array of results - one for each frequency channel
  std::vector<result> results;

  Reference::To<Pulsar::Archive> output;

  double integration_length;

  //! Add command line options
  void add_options (CommandLine::Menu&);
};


/*!

  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "psr4th -h"

  This constructor makes use of

  - StandardOptions (-j -J etc.): an option set that provides standard
  preprocessing with the pulsar command language interpreter.

  - UnloadOptions (-e -m etc.): an option set that provides standard
  options for unloading data.

  This constructor also sets the default values of the attributes that
  are unique to the program.

*/

psr4th::psr4th ()
  : Application ("psr4th", "psr4th psrchive program")
{
  add( new Pulsar::StandardOptions );
  integration_length = 0;
}


/*!

  Add application-specific command-line options.

*/

void psr4th::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  // // add an option that enables the user to set the scale with -s
  // arg = menu.add (scale, 's', "scale");
  // arg->set_help ("multiply all amplitudes by 'scale'");

  // // add an option that enables the user to set the source name with -name
  // arg = menu.add (scale, "name", "string");
  // arg->set_help ("set the source name to 'string'");
}


/*!

  Scale every profile and optionally set the source name

*/

void psr4th::process (Pulsar::Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  unsigned nbin = archive->get_nbin();
  unsigned nchan = archive->get_nchan();

  archive->convert_state( Signal::Stokes );

  if (!output)
  {
    string output_format = "PSRFITS";
    output = Pulsar::Archive::new_Archive (output_format);  
    output->copy (*archive);
    output->resize(1);

    results.resize (nchan);
    for (unsigned ichan = 0; ichan < nchan; ichan++)
      results[ichan].resize (nbin);
  }

  if (output->get_nchan() != nchan)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nchan = %u != required nchan = %u",
		 nchan, output->get_nchan());

  if (output->get_nbin() != nbin)
    throw Error (InvalidParam, "psr4th::process",
		 "archive nbin = %u != required nbin = %u",
		 nbin, output->get_nbin());

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Reference::To<Pulsar::Integration> subint = archive->get_Integration (isub);
    integration_length += subint->get_duration ();

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (subint->get_weight(ichan) == 0)
        continue;

      Reference::To<Pulsar::PolnProfile> profile = subint->new_PolnProfile (ichan);

      for (unsigned ibin=0; ibin < nbin; ibin++)
      {
	Stokes<double> S = profile->get_Stokes (ibin);

	results[ichan].stokes[ibin] += S;
	results[ichan].stokes_squared[ibin] += outer(S,S);
      }

      results[ichan].count ++;
    }
  }
}

void psr4th::finalize()
{
  unsigned nbin = output->get_nbin();
  unsigned nchan = output->get_nchan();
  unsigned nmoment = 10;

  Pulsar::Integration* subint = output->get_Integration (0);
  subint->set_duration( integration_length );

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    Pulsar::PolnProfile* profile = subint->new_PolnProfile (ichan);

    Reference::To<Pulsar::MoreProfiles> more = new Pulsar::FourthMoments;
    more->resize( nmoment, nbin );

    subint->get_Profile(0,ichan)->add_extension(more);

    if (results[ichan].count == 0)
      subint->set_weight (ichan, 0.0);

    for (unsigned ibin = 0; ibin < nbin ; ibin ++)
    {
      Matrix<4,4,double> covar = results[ichan].get_covariance (ibin);
      Stokes<double> mean = results[ichan].get_mean (ibin);

      unsigned index=0;
      for (unsigned i=0; i<4; i++)
      {
	profile->get_Profile(i)->get_amps()[ibin] = mean[i];
	for (unsigned j=i; j<4; j++)
	{
	  more->get_Profile(index)->get_amps()[ibin] = covar[i][j];
	  index ++;
	}
      }
    }
  }

  output->unload ("psr4th.ar");
}

void psr4th::result::resize (unsigned nbin)
{
  stokes.resize (nbin);
  stokes_squared.resize (nbin);

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    stokes[ibin] = 0.0;
    stokes_squared[ibin] = 0.0;
  }
  
  count = 0;
}

Matrix<4,4,double> psr4th::result::get_covariance (unsigned ibin)
{
  Matrix<4,4,double> meansq = stokes_squared [ibin];
  meansq /= count;

  Stokes<double> mean = get_mean(ibin);

  return meansq - outer(mean,mean);
}

Stokes<double> psr4th::result::get_mean (unsigned ibin)
{
  Stokes<double> mean = stokes [ibin];
  mean /= count;
  return mean;
}

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  psr4th program;
  return program.main (argc, argv);
}

