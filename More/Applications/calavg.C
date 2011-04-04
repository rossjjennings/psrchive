/***************************************************************************
 *
 *   Copyright (C) 2011 by Jonathan Khoo
 *   Licensed under the Academic Free License version 3.0
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/StandardOptions.h"

#include "Error.h"

#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/Receiver.h"

#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// The number of polns of a fluxcal observation is always 2.
const unsigned NPOL = 2;

//! Calibration file (Pulsar Archive) averaging application.
class calavg : public Pulsar::Application
{
  public:

    calavg();

    void process(Pulsar::Archive*);

    void finalize();

  protected:

    //! Add command line options
    void add_options(CommandLine::Menu&) {}

  private:

    void set_average_centre_frequency(const double frequency);

    double get_average_centre_frequency() const;

    void set_average_receiver(const string receiver);

    string get_average_receiver() const;

    void set_average_bw(const double _bw);

    double get_average_bw() const;

    void set_average_nchan(const unsigned _nchan);

    unsigned get_average_nchan() const;

    Pulsar::Archive* average;

    bool is_frontend_same(Pulsar::Archive* archive) const;

    double average_centre_frequency;

    string average_receiver;

    double bw;

    unsigned nchan;

    vector<vector<vector<Estimate<double> > > > S_sys_values;

    vector<vector<vector< Estimate<double> > > > S_cal_values;

    string get_receiver(Pulsar::Archive* archive) const;

    double get_frequency(Pulsar::Archive* archive) const;

    double get_bw(Pulsar::Archive* archive) const;

    unsigned get_nchan(Pulsar::Archive* archive) const;
};

string calavg::get_receiver(Pulsar::Archive* archive) const
{
  Reference::To<Pulsar::Receiver> ext = archive->get<Pulsar::Receiver>();

  if (ext) {
    const string receiver = ext->get_name();
    return receiver;
  }

  return "";
}

double calavg::get_frequency(Pulsar::Archive* archive) const
{
  const double centre_frequency = average->get_centre_frequency();
  return centre_frequency;
}

double calavg::get_bw(Pulsar::Archive* archive) const
{
  const double bw = archive->get_bandwidth();
  return bw;
}

unsigned calavg::get_nchan(Pulsar::Archive* archive) const
{
  Reference::To<Pulsar::FluxCalibratorExtension> ext =
    archive->get<Pulsar::FluxCalibratorExtension>();

  if (ext) {
    const unsigned nchan = ext->get_nchan();
    return nchan;
  }

  return 0;
}

void calavg::set_average_nchan(const unsigned _nchan)
{
  nchan = _nchan;
}

unsigned calavg::get_average_nchan() const
{
  return nchan;
}

void calavg::set_average_bw(const double _bw)
{
  bw = _bw;
}

double calavg::get_average_bw() const
{
  return bw;
}

void calavg::set_average_centre_frequency(const double frequency)
{
  average_centre_frequency = frequency;
}

double calavg::get_average_centre_frequency() const
{
  return average_centre_frequency;
}

void calavg::set_average_receiver(const string receiver)
{
  average_receiver = receiver;
}

string calavg::get_average_receiver() const
{
  return average_receiver;
}

bool calavg::is_frontend_same(Pulsar::Archive* archive) const
{
  Reference::To<Pulsar::Receiver> ext = archive->get<Pulsar::Receiver>();

  if (ext) {
    return average_receiver == ext->get_name();
  }

  return false;
}

void calavg::finalize()
{

  Reference::To<Pulsar::FluxCalibratorExtension> ext =
    average->get<Pulsar::FluxCalibratorExtension>();

  const unsigned nchan = ext->get_nchan();

  // Calculate the weighted average (channel by channel) for each file, channel,
  // and receptor.
  for (unsigned ipol = 0; ipol < NPOL; ++ipol) {
    for (unsigned ichan = 0; ichan < S_sys_values[0].size(); ++ichan) {
      // Iterate over the S_sys and S_sys_err values of each file and calculate
      // the average mean (for the current channel and receptor).
      MeanEstimate<double, double> total_S_sys = MeanEstimate<double, double>(0, 0);

      for (unsigned ifile = 0; ifile < S_sys_values.size(); ++ifile) {
        const MeanEstimate<double, double> S_sys = S_sys_values[ifile][ichan][ipol];
        total_S_sys += S_sys;
      }

      ext->set_S_sys(ichan, ipol, total_S_sys);
    }
  }

  // Calculate the channel weight by summing the channel weights (calculated
  // from the error) across both receptors.
  for (unsigned ichan = 0; ichan < nchan; ++ichan) {
    MeanEstimate<double, double> total_S_sys = MeanEstimate<double, double>(0, 0);
    for (unsigned ipol = 0; ipol < NPOL; ++ipol) {
      const MeanEstimate<double, double> S_sys = ext->get_S_sys(ichan, ipol);
      total_S_sys += S_sys;
    }

    // Calculate channel weight from error.
    const double error = total_S_sys.get_Estimate().get_error();
    const double weight = 1.0/pow(error, 2);
    ext->set_weight(ichan, weight); // CHECKED
  }

  //average->unload("hello");
}

void calavg::process(Pulsar::Archive* archive)
{
  if (verbose) {
    cerr << "Processing: " << archive->get_filename() << endl;
  }

  Reference::To<Pulsar::FluxCalibratorExtension> ext =
    archive->get<Pulsar::FluxCalibratorExtension>();

  // Ensure that the loaded archive is a fluxcal.
  if (ext == NULL) {
    cerr << archive->get_filename() << " is not a fluxcal... " << endl;

    return;
  }

  // Compare parameters if it is not the first file.
  if (average) {
    // Compare (before continuing):
    //  frontend
    //  freq
    //  bw
    //  nchan

    if (get_receiver(average) != get_receiver(archive)) {
      throw Error(InvalidState, "calavg::process",
          "Receivers are different (%s and %s)", get_receiver(average).c_str(),
          get_receiver(archive).c_str());
    }

    if (get_frequency(average) != get_frequency(archive)) {
      throw Error(InvalidState, "calavg::process",
          "frequencies are different (%g and %g)", get_frequency(average),
          get_frequency(archive));
    }

    if (get_bw(average) != get_bw(archive)) {
      throw Error(InvalidState, "calavg::process",
          "bandwidths are different (%g and %g)", get_bw(average),
          get_bw(archive));
    }

    if (get_nchan(average) != get_nchan(archive)) {
      throw Error(InvalidState, "calavg::process",
          "nchans are different (%g and %g)", get_nchan(average),
          get_nchan(archive));
    }
  }

  // Copy all S_sys values from the current fluxcal.
  S_sys_values.push_back(ext->get_S_sys());

  if (average == NULL) {
    if (verbose) {
      cerr << "Using " << archive->get_filename() <<
        " as a storage container." << endl;
    }

    // Retain a copy of the first cal to act as a container of the weighted
    // average fluxcal. 
    average = archive->clone();

    // Store the parameters that are required to be consistent with
    // following fluxcal files.
    set_average_receiver(get_receiver(average));
    set_average_centre_frequency(get_frequency(average));
    set_average_bw(get_bw(average));
    set_average_nchan(get_nchan(average));
  }
}

static calavg program;

int main(int argc, char *argv[])
{
  return program.main(argc, argv);
}

calavg::calavg() :
  Pulsar::Application("calavg", "averages cal files"),
  average(NULL)
{
  add( new Pulsar::StandardOptions );
}

