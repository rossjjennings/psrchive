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
#include "Pulsar/StokesCylindrical.h"

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
  unsigned global_search;

  bool rvm_fit;
  bool plot_result;

  Pulsar::PlotOptions plot;
};

int main (int argc, char** argv)
{
  psrmodel program;
  return program.main (argc, argv);
}

psrmodel::psrmodel () :
  Pulsar::Application ("psrmodel", "pulsar modeling program"),
  plot (false)
{
  has_manual = false;
  version = "$Id: psrmodel.C,v 1.5 2009/06/12 09:10:05 straten Exp $";

  add( new Pulsar::StandardOptions );
  add( &plot );

  rvm = new ComplexRVMFit;
  global_search = 0;

  rvm_fit = false;
  plot_result = false;
}

std::string psrmodel::get_options ()
{
  return "AZBPds:a:z:b:p:t:x";
}

std::string psrmodel::get_usage ()
{
  return
    " -d               plot the resulting model over the data \n"
    " -s nstep         do a global minimum search on nstep^2 grid \n"
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

  case 'd':
    plot.set_open_device (true);
    plot_result = true;
    break;

  case 's':
    global_search = fromstring<unsigned>(arg);
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

  case 'x':
    rvm->set_chisq_map (true);
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
  data->centre();

  Reference::To<PolnProfile> p = data->get_Integration(0)->new_PolnProfile(0);
  rvm->set_observation (p);

  if (global_search)
  {
    cerr << "psrmodel: performing global search over " << global_search
	 << " square grid" << endl;

    rvm->global_search (global_search);
  }
  else
  {
    cerr << "psrmodel: solving with initial guess: " << endl;
    rvm->solve();
  }

  cerr << "chisq=" << rvm->get_chisq() << "/nfree=" << rvm->get_nfree()
       << " = " << rvm->get_chisq()/ rvm->get_nfree() << endl;

  MEAL::RotatingVectorModel* RVM = rvm->get_model()->get_rvm();

  double d = 180/M_PI;
  double t = 0.5/M_PI;

  Estimate<double> PA = RVM->reference_position_angle->get_value();

#if 0
  PA.val = fmod (PA.val, M_PI);
  if (PA.val > M_PI/2)
    PA.val -= M_PI;
  if (PA.val < -M_PI/2)
    PA.val += M_PI;

  // RVM->line_of_sight->set_value( RVM->line_of_sight->get_value() - 2*MPI );
#endif

  cerr <<
    "PA_0="  << d*PA << " deg\n"
    "zeta="  << d*RVM->line_of_sight->get_value() << " deg\n"
    "alpha=" << d*RVM->magnetic_axis->get_value() << " deg\n"
    "phi_0=" << t*RVM->magnetic_meridian->get_value() << " turns"
	     << endl;

  if (plot_result)
  {
    StokesCylindrical plotter;

    AnglePlot* pa = plotter.get_orientation();

    pa->set_threshold( rvm->get_threshold() );
    pa->model.set (RVM, &MEAL::RotatingVectorModel::compute);
    pa->get_frame()->get_y_scale()->set_range_norm (0, 1.5);

    plotter.plot( data );
  }
}
