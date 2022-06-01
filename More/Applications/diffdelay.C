 /**************************************************************************2
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/SingleAxisCalibrator.h"

#include "ChiSquared.h"

using namespace std;
using namespace Pulsar;

//
//! An example of an application
//
class diffdelay : public Pulsar::Application
{
public:

  //! Default constructor
  diffdelay ();

  //! Load differential phase from PolnCalibrator
  void process (Pulsar::Archive*);

  //! Add command line options
  void add_options (CommandLine::Menu&);

protected:

  //! Differential phase parameter index
  unsigned iparam_phi;

};



diffdelay::diffdelay ()
  : Application ("diffdelay", "estimates differential delay")
{
  // default set up for most
  iparam_phi = 2;
}


void diffdelay::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (iparam_phi, 'i', "index");
  arg->set_help ("model index of rotation about line of sight");
}


void diffdelay::process (Pulsar::Archive* archive)
{
  Reference::To<PolnCalibrator> calibrator;

  if (archive->get<Pulsar::PolnCalibratorExtension>())
    calibrator = new Pulsar::PolnCalibrator (archive);
  else
    calibrator = new Pulsar::SingleAxisCalibrator (archive);

  unsigned nchan = calibrator->get_nchan();
  double bandwidth = archive->get_bandwidth();

  //! Differntial phase in radians
  std::vector< Estimate<double> > phi;
  //! Frequency offset in MHz
  std::vector< double > freq_offset;

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    if (!calibrator->get_transformation_valid(ichan))
      continue;

    Estimate<double> rot;
    rot = calibrator->get_transformation(ichan)->get_Estimate(iparam_phi);

    if (rot.var == 0.0)
      continue;
    
    double freq_MHz = ichan * bandwidth / nchan;
    
    phi.push_back (rot);
    freq_offset.push_back (freq_MHz);
  }
  
  unsigned ndat = phi.size();
  
  vector<double> yval (ndat);
  vector<double> wt (ndat);

  for (unsigned idat=0; idat < ndat; idat++)
  {
    wt[idat] = 1.0 / phi[idat].var;
    yval[idat] = phi[idat].val;
  }

  double fit_phi0 = 0;
  double fit_tau = 0;

  weighted_linear_fit (fit_tau, fit_phi0, yval, freq_offset, wt);

  cout << archive->get_filename() << " tau= " << fit_tau*1e3 << " ns" << endl;
}

int main (int argc, char** argv)
{
  diffdelay program;
  return program.main (argc, argv);
}

