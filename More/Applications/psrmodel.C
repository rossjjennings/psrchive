/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/ComplexRVMFit.h"
#include "MEAL/ComplexRVM.h"
#include "MEAL/RotatingVectorModel.h"

using namespace Pulsar;
using namespace std;

//! Pulsar modeling application
class psrmodel: public Pulsar::Application
{
public:

  //! Default constructor
  psrmodel ();

  //! Return usage information 
  std::string get_usage ();

  //! Return getopt options
  std::string get_options ();

  //! Parse a command line option
  bool parse (char code, const std::string& arg);

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  Reference::To<ComplexRVMFit> rvm;
  bool rvm_fit;
};

int main (int argc, char** argv)
{
  psrmodel program;
  return program.main (argc, argv);
}

psrmodel::psrmodel () : Pulsar::Application ("psrmodel",
					     "pulsar modeling program")
{
  has_manual = false;
  version = "$Id: psrmodel.C,v 1.1 2009/06/11 05:03:27 straten Exp $";

  add( new Pulsar::StandardOptions );
  add( new Pulsar::PlotOptions );

  rvm = new ComplexRVMFit;
  rvm_fit = false;
}

std::string psrmodel::get_options ()
{
  return "Ra:z:m:p:";
}

std::string psrmodel::get_usage ()
{
  return
    " -R               fit the rotating vector model \n"
    " -a degrees       alpha: colatitude of magnetic axis \n"
    " -z degrees       zeta: colatitude of line of sight \n"
    " -m degrees       longitude of magnetic meridian \n"
    " -p degrees       position angle at magnetic meridian \n";
}

double radians (const std::string& arg)
{
  return fromstring<double> (arg) * M_PI/180.0;
}

//! Parse a command line option
bool psrmodel::parse (char code, const std::string& arg)
{
  switch (code)
  {
  case 'R':
    rvm_fit = true;
    break;

  case 'a':
    rvm->get_model()->get_rvm()->set_magnetic_axis (radians (arg));
    break;
    
  case 'z':
    rvm->get_model()->get_rvm()->set_line_of_sight (radians (arg));
    break;
    
  case 'm':
    rvm->get_model()->get_rvm()->set_magnetic_meridian (radians (arg));
    break;
    
  case 'p':
    rvm->get_model()->get_rvm()->set_reference_position_angle (radians (arg));
    break;

  default:
    return false;
  }

  return true;
}


void psrmodel::setup ()
{
  if (!rvm_fit)
    throw Error (InvalidState, "psrmodel",
		 "please use -r (can only do RVM fit for now)");
}

void psrmodel::process (Pulsar::Archive* data)
{
  if (verbose)
    cerr << "psrmodel: fitting " << data->get_filename() << endl;

  data->tscrunch();
  data->fscrunch();
  data->convert_state(Signal::Stokes);

  Reference::To<PolnProfile> p = data->get_Integration(0)->new_PolnProfile(0);
  rvm->set_observation (p);
  rvm->solve();
}
