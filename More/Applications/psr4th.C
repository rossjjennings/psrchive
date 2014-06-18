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
  std::vector< Matrix<4,4,double> > stokes_squared;
  std::vector< Stokes<double> > stokes;
  uint64_t count;

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
  // add( new Pulsar::UnloadOptions );

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
  unsigned npol = archive->get_npol();
  unsigned nbin = archive->get_nbin();

  if (npol != 4)
    throw Error (InvalidParam, "psr4th::process",
		 "The input data must have npol == 4 (i.e. full poln)");

  if (stokes.size() == 0)
    {
      // first file - resize results arrays and init to zero
      stokes.resize (nbin);
      stokes_squared.resize (nbin);

      for (unsigned ibin=0; ibin < nbin; ibin++)
	{
	  stokes[ibin] = 0.0;
	  stokes_squared[ibin] = 0.0;
	}

      count = 0;
    }
  else if (stokes.size() != nbin)
    throw Error (InvalidParam, "psr4th::process",
		 "The input data have nbin = %u; should have nbin = %u",
		 nbin, stokes.size());

  archive->fscrunch();
  archive->convert_state( Signal::Stokes );

  unsigned ichan = 0;

  for (unsigned isub=0; isub < nsub; isub++)
  {
    Pulsar::Integration* subint = archive->get_Integration (isub);
    Pulsar::PolnProfile* profile = subint->new_PolnProfile (ichan);

    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      Stokes<double> S = profile->get_Stokes (ibin);

      stokes[ibin] += S;
      stokes_squared[ibin] += outer(S,S);
    }

    count ++;
  }
}

void psr4th::finalize()
{
  unsigned nbin = stokes.size();

  for (unsigned ibin = 0; ibin < nbin ; ibin ++)
  {
    stokes_squared [ibin] /= count;
    stokes [ibin] /= count;

    Matrix<4,4,double> covar = stokes_squared [ibin] 
      - outer(stokes[ibin], stokes[ibin]);

    cout << ibin << " ";
    for (unsigned i=0; i<4; i++)
      for (unsigned j=i; j<4; j++)
	cout << covar[i][j] << " ";
    cout << endl;
  }
}

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  psr4th program;
  return program.main (argc, argv);
}

