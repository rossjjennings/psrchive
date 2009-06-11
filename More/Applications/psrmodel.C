/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
//#include "Pulsar/PlotOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/ComplexRVMFit.h"
#include "MEAL/ComplexRVM.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarParameter.h"

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
  version = "$Id: psrmodel.C,v 1.2 2009/06/11 07:38:11 straten Exp $";

  add( new Pulsar::StandardOptions );

  rvm = new ComplexRVMFit;
  rvm_fit = false;
}

std::string psrmodel::get_options ()
{
  return "AZBPa:z:b:p:";
}

std::string psrmodel::get_usage ()
{
  return
    " -a degrees       alpha: colatitude of magnetic axis \n"
    " -z degrees       zeta: colatitude of line of sight \n"
    " -b degrees       longitude of magnetic meridian \n"
    " -p degrees       position angle at magnetic meridian \n";
}

double radians (const std::string& arg)
{
  return fromstring<double> (arg) * M_PI/180.0;
}

//! Parse a command line option
bool psrmodel::parse (char code, const std::string& arg)
{
  MEAL::RotatingVectorModel* RVM = rvm->get_model()->get_rvm();
  rvm_fit = true;

  switch (code)
  {

  case 'a':
    RVM->magnetic_axis->set_value (radians (arg));
    break;
  case 'A':
    RVM->magnetic_axis->set_infit (0, false);
    break;

  case 'z':
    RVM->line_of_sight->set_value (radians (arg));
    break;
  case 'Z':
    RVM->line_of_sight->set_infit (0, false);
    break;
   
  case 'b':
    RVM->magnetic_meridian->set_value (radians (arg));
    break;
  case 'B':
    RVM->magnetic_meridian->set_infit (0, false);
    break;
            
  case 'p':
    RVM->reference_position_angle->set_value (radians (arg));
    break;
  case 'P':
    RVM->reference_position_angle->set_infit (0, false);
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
  data->remove_baseline();

  Reference::To<PolnProfile> p = data->get_Integration(0)->new_PolnProfile(0);
  rvm->set_observation (p);
  rvm->solve();

  MEAL::RotatingVectorModel* RVM = rvm->get_model()->get_rvm();

  double r = 180/M_PI;

  cerr << "PA_0="  << r*RVM->reference_position_angle->get_value() << endl;
  cerr << "zeta="  << r*RVM->line_of_sight->get_value() << endl;
  cerr << "alpha=" << r*RVM->magnetic_axis->get_value() << endl;
  cerr << "phi_0=" << r*RVM->magnetic_meridian->get_value() << endl;

}
