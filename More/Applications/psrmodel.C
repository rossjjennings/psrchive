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
  bool global_search;
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
  version = "$Id: psrmodel.C,v 1.4 2009/06/11 09:55:58 straten Exp $";

  add( new Pulsar::StandardOptions );

  rvm = new ComplexRVMFit;
  rvm_fit = false;
  global_search = false;
}

std::string psrmodel::get_options ()
{
  return "AZBPga:z:b:p:t:";
}

std::string psrmodel::get_usage ()
{
  return
    " -g               do a global minimum search \n"
    " -t sigma         cutoff threshold when selecting bins [default 3] \n"
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

  case 'g':
    global_search = true;
    break;

  case 't':
    rvm->set_threshold ( fromstring<float>(arg) );
    break;

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

  if (global_search)
    rvm->global_search ();
  else
    rvm->solve();

  cerr << "chisq=" << rvm->get_chisq() << "/nfree=" << rvm->get_nfree()
       << " = " << rvm->get_chisq()/ rvm->get_nfree() << endl;

  MEAL::RotatingVectorModel* RVM = rvm->get_model()->get_rvm();

  double d = 180/M_PI;
  double t = 0.5/M_PI;

  // negative because sign of PA in RVM is opposite to IAU convention
  Estimate<double> PA = - RVM->reference_position_angle->get_value();
  PA.val = fmod (PA.val, M_PI);
  if (PA.val > M_PI/2)
    PA.val -= M_PI/2;
  if (PA.val < -M_PI/2)
    PA.val += M_PI/2;

  cerr <<
    "PA_0="  << d*PA << " deg\n"
    "zeta="  << d*RVM->line_of_sight->get_value() << " deg\n"
    "alpha=" << d*RVM->magnetic_axis->get_value() << " deg\n"
    "phi_0=" << t*RVM->magnetic_meridian->get_value() << " turns"
	     << endl;

}
