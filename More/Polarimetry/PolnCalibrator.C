#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pauli.h"
#include "Error.h"

bool Pulsar::PolnCalibrator::store_parameters = false;

Pulsar::PolnCalibrator::~PolnCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

Pulsar::PolnCalibrator::PolnCalibrator (const vector<Archive*>& archives)
{
  throw Error (InvalidState, "PolnCalibrator::",
	       "vector constructor not implemented");
}

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
  if (!calibrator->match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + "\n" + reason);

  if (jones.size() != arch->get_nchan())
    create (arch->get_nchan());

  arch->transform (jones);
}

void Pulsar::PolnCalibrator::build ()
{
  store_parameters = true;
  create (calibrator->get_nchan());
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

void Pulsar::PolnCalibrator::calculate (vector<vector<Estimate<double> > >& hi,
					vector<vector<Estimate<double> > >& lo)
{
  unsigned npol = hi.size();
  unsigned nchan = hi[0].size();

  baseline.resize (nchan);
  jones.resize (nchan);

  if (store_parameters)
    resize_parameters (nchan);

  // coherency products in a single channel
  vector<Estimate<double> > cal_hi (npol);
  vector<Estimate<double> > cal_lo (npol);

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    for (unsigned ipol=0; ipol<npol; ++ipol) {
      cal_hi[ipol] = hi[ipol][ichan];
      cal_lo[ipol] = lo[ipol][ichan];
    }

    Estimate<double> cal_AA = cal_hi[0] - cal_lo[0];
    Estimate<double> cal_BB = cal_hi[1] - cal_lo[1];

    if (cal_AA.val <= 0 || cal_BB.val <= 0) {
      if (verbose)
	cerr << "Pulsar::PolnCalibrator::calculate ichan=" << ichan <<
	  " bad calibrator levels" << endl;
      baseline[ichan] = 0;
      jones[ichan] = Jones<float>();
      continue;
    }

    // baseline intensity, normalized by cal flux, C
    baseline[ichan] = cal_lo[0]/cal_AA + cal_lo[1]/cal_BB;

    // store the Jones matrix appropriate for inverting the system response
    jones[ichan] = inv( solve (cal_hi, cal_lo, ichan) );

  }
}

