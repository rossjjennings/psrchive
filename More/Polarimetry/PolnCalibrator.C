#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pauli.h"
#include "Error.h"

bool Pulsar::PolnCalibrator::store_parameters = false;

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
    calibrator = clone;
  }
  else
    calibrator = arch;
}

/*! Upon completion, the flux of the archive will be normalized with respect
  to the flux of the calibrator, such that a FluxCalibrator simply needs
  to scale the archive by the calibrator flux. */
void Pulsar::PolnCalibrator::calibrate (Archive* arch)
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		 "no PolnCal Archive");

  string reason;
  if (!calibrator->mixable (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + "\n" + reason);

  if (jones.size() != arch->get_nchan())
    create (arch->get_nchan());

  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    calibrate (arch->get_Integration(isub));

}

void Pulsar::PolnCalibrator::create (unsigned nchan)
{
  // get the calibrator hi and lo levels from the PolnCal archive
  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  calibrator->get_Integration(0)->cal_levels (cal_hi, cal_lo);

  if (calibrator->get_nchan() == nchan) {
    calculate (cal_hi, cal_lo);
    return;
  }

  throw Error (InvalidState, "Pulsar::FluxCalibrator::create",
	       "Cannot currently calibrate archives with different nchan");

  // make hi and lo the right size of cal_hi and cal_lo
  vector<vector<Estimate<double> > > hi;
  vector<vector<Estimate<double> > > lo;
    
  calculate (hi, lo);

}

void
Pulsar::PolnCalibrator::calculate (vector<vector<Estimate<double> > >& cal_hi,
				   vector<vector<Estimate<double> > >& cal_lo)
{
  unsigned npol = cal_hi.size();
  unsigned nchan = cal_hi[0].size();
  unsigned nprobes = 2;

  bool full_polarization = npol == 4;

  if (store_parameters) {
    boost_q.resize (nchan);
    gain.resize (nchan);

    if (full_polarization)
      rotation_q.resize (nchan);
  }

  else {
    boost_q.clear ();
    gain.clear ();
    rotation_q.clear ();
  }

  baseline.resize (nchan);
  jones.resize (nchan);

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    bool invalid = false;
    for (unsigned ipol=0; ipol<nprobes; ++ipol)
      if (cal_hi[ipol][ichan]==0 || cal_hi[ipol][ichan] <= cal_lo[ipol][ichan])
	invalid = true;
    
    if (invalid) {
      if (verbose)
	cerr << "Pulsar::PolnCalibrator channel " << ichan
	     << ": invalid levels flagged" << endl;

      boost_q[ichan] = 0;
      rotation_q[ichan] = 0;

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

    Estimate<double> beta = 0.25 * log (cal_a / cal_b);
    if (store_parameters)
      boost_q[ichan] = beta;

    // gain, G = g_a g_b C
    Estimate<double> Gain = sqrt (cal_a * cal_b);
    if (store_parameters)
      gain[ichan] = Gain;

    // baseline intensity, normalized by cal flux, C
    baseline[ichan] = cal_lo[0][ichan]/cal_a + cal_lo[1][ichan]/cal_b;

    if (verbose)
      cerr << "Pulsar::PolnCalibrator channel " << ichan <<
	"\n  boost=" << beta << 
	"\n  gain=" << Gain << 
	"\n  baseline=" << baseline[ichan] << endl;

    Quaternion<double, Hermitian> b_q (cosh(beta.val), sinh(beta.val), 0, 0);

    Quaternion<double, Unitary> r_q = Quaternion<double, Unitary>::identity();

    if (full_polarization) {
      Estimate<double> cal_ab = cal_hi[2][ichan] - cal_lo[2][ichan];
      Estimate<double> cal_ba = cal_hi[3][ichan] - cal_lo[3][ichan];

      if (cal_ba == 0 && cal_ab == 0)  {
	if (verbose) cerr << "Pulsar::PolnCalibrator channel " << ichan
			  << ": invalid instrumental rotation" << endl;
	if (store_parameters)
	  rotation_q[ichan] = 0;
      }

      Estimate<double> phi = 0.5 * atan2 (cal_ba, cal_ab);

      if (store_parameters)
	rotation_q[ichan] = phi;

      if (verbose)
	cerr << "Pulsar::PolnCalibrator channel " << ichan << ":"
	     << " differential phase=" << phi << endl;

      r_q = Quaternion<double> (cos(phi.val), sin(phi.val), 0, 0);
    }

    Jones<double> response = sqrt(Gain.val) * b_q * r_q;

    jones[ichan] = inv (response);

  }
}

void Pulsar::PolnCalibrator::calibrate (Integration* subint)
{
  if (subint->get_npol() != 4)
    throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		 "TODO: npol != 4 not implemented");

  subint->transform (jones);
}
