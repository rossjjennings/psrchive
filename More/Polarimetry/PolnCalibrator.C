#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

bool Pulsar::PolnCalibrator::verbose = false;


Pulsar::PolnCalibrator::PolnCalibrator (const Archive* arch)
{
  if (verbose)
    cerr << "Pulsar::PolnCalibrator" << endl;

  if (arch->get_type() != Signal::PolnCal)
    throw Error (InvalidParam, "Pulsar::PolnCalibrator", "Pulsar::Archive='"
		   + arch->get_filename() + "' not a PolnCal");
  
  // Here the decision is made about full stokes or dual band observations.
  Signal::State state = arch->get_state();

  bool fullStokes = state == Signal::Stokes || state == Signal::Coherence;

  bool calibratable = fullStokes || state == Signal::PPQQ;

  if (!calibratable)
    throw Error (InvalidParam, "Pulsar::PolnCalibrator", "Pulsar::Archive='"
		 + arch->get_filename() + "'\n"
		 "invalid state=" + State2string(state));

  if (fullStokes && state != Signal::Coherence) {
    Archive* clone = arch->clone();
    clone->convert_state (Signal::Coherence);
    archive = clone;
  }
  else
    archive = arch;
}

//! Calibrate the flux in the given archive
void Pulsar::PolnCalibrator::calibrate (Archive* arch)
{
  if (!archive)
    throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		 "no PolnCal Archive");

  string reason;
  if (!archive->mixable (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + archive->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + "\n" + reason);

  if (gain.size() != arch->get_nchan()) {

    // get the calibrator hi and lo levels from the PolnCal archive
    vector<vector<Estimate<double> > > cal_hi;
    vector<vector<Estimate<double> > > cal_lo;
    archive->get_Integration(0)->cal_levels (cal_hi, cal_lo);

    vector<vector<Estimate<double> > > hi;
    vector<vector<Estimate<double> > > lo;

    // make hi and lo the right size of cal_hi and cal_lo

    calculate (hi, lo);

  }

}


void
Pulsar::PolnCalibrator::calculate (vector<vector<Estimate<double> > >& cal_hi,
				   vector<vector<Estimate<double> > >& cal_lo)
{
  unsigned npol = cal_hi.size();
  unsigned nchan = cal_hi[0].size();
  unsigned nprobes = 2;

  bool full_polarization = npol == 4;

  gain.resize (nchan);

  if (full_polarization)
    phase.resize (nchan);
  else
    phase.clear ();

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    bool invalid = false;
    for (unsigned ipol=0; ipol<nprobes; ++ipol)
      if (cal_hi[ipol][ichan]==0 || cal_hi[ipol][ichan] <= cal_lo[ipol][ichan])
	invalid = true;
    
    if (invalid) {
      if (verbose)
	cerr << "Pulsar::PolnCalibrator channel " << ichan
	     << ": invalid levels flagged" << endl;

      gain[ichan] = 0;
      phase[ichan] = 0;

      continue;
    }

    Estimate<double> cal_a = cal_hi[0][ichan] - cal_lo[0][ichan];
    Estimate<double> cal_b = cal_hi[1][ichan] - cal_lo[1][ichan];

    // note: there is an error in Britton 2000, following Equation (14):
    // correction: \beta = 1/2 ln (g_a/g_b)

    // note also: that this equation refers to the Jones matrix and, since
    // we are dealing with detected coherency products:
    //   cal_a = g_a^2 C
    //   cal_b = g_b^2 C
    // where C is the cal flux in each probe (assumed equal in this model)

    gain[ichan] = 0.25 * log (cal_a / cal_b);

    if (verbose)
      cerr << "Pulsar::PolnCalibrator channel " << ichan << ":"
	   << " instrumental gain " << gain[ichan] << endl;

    if (!full_polarization)
      continue;

    Estimate<double> cal_ab = cal_hi[2][ichan] - cal_lo[2][ichan];
    Estimate<double> cal_ba = cal_hi[3][ichan] - cal_lo[3][ichan];

    if (cal_ba == 0 && cal_ab == 0)  {
      if (verbose) cerr << "Pulsar::PolnCalibrator channel " << ichan
			<< ": invalid instrumental phase flagged" << endl;

      phase[ichan] = 0;

      continue;
    }
    
    phase[ichan] = 0.5 * atan2 (cal_ba, cal_ab);

    if (verbose)
      cerr << "Pulsar::PolnCalibrator channel " << ichan << ":"
	   << " instrumental phase " << phase[ichan] << endl;
  }
}

