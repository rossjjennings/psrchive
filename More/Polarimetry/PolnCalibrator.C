#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "smooth.h"

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
  if (!arch)
    throw Error (InvalidState, "PolnCalibrator::", "no Archive");

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
  if (!calibrator->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + reason);

  if (jones.size() != arch->get_nchan())
    create (arch->get_nchan());

  arch->transform (jones);
}

void Pulsar::PolnCalibrator::build ()
{
  store_parameters = true;
  create (calibrator->get_nchan());
}

/*!
  \param isubint the calibrator Integration from which to derive levels
  \param nchan the desired frequency resolution
  \retval cal_hi the mean levels of the calibrator hi state
  \retval cal_lo the mean levels of the calibrator lo state
*/
void 
Pulsar::PolnCalibrator::get_levels (unsigned isubint, unsigned nchan, 
				    vector<vector<Estimate<double> > >& cal_hi,
				    vector<vector<Estimate<double> > >& cal_lo)
  const
{
  if (verbose)
    cerr << "Pulsar::PolnCalibrator::get_levels call Integration::cal_levels"
	 << endl;

  calibrator->get_Integration(isubint)->cal_levels (cal_hi, cal_lo);

  unsigned npol = cal_hi.size();
  unsigned ipol = 0;

  unsigned window = unsigned (calibrator->get_nchan() * median_smoothing);

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::get_levels median smoothing window = "
	 << window << " channels" << endl;

  // even a 3-window sort can zap a single channel birdie
  if (window < 3)
    window = 3;

  for (ipol=0; ipol < npol; ipol++) {
    fft::median_smooth (cal_lo[ipol], window);
    fft::median_smooth (cal_hi[ipol], window);
  }

  if (calibrator->get_nchan() == nchan)
    return;

  // make hi and lo the right size of cal_hi and cal_lo
  vector<vector<Estimate<double> > > hi (npol);
  vector<vector<Estimate<double> > > lo (npol);
 
  for (ipol=0; ipol < npol; ipol++) {
    lo[ipol].resize (nchan);
    fft::interpolate (lo[ipol], cal_lo[ipol]);
    
    hi[ipol].resize (nchan);
    fft::interpolate (hi[ipol], cal_hi[ipol]);
  }

  cal_lo = lo;
  cal_hi = hi;
}

void Pulsar::PolnCalibrator::create (unsigned nchan)
{
  if (verbose)
    cerr << "Pulsar::PolnCalibrator::create nchan=" << nchan << endl;

  // get the calibrator hi and lo levels from the PolnCal archive
  vector<vector<MeanEstimate<double> > > total_hi;
  vector<vector<MeanEstimate<double> > > total_lo;

  // get the calibrator hi and lo levels from the PolnCal archive
  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;

  unsigned nsub = calibrator->get_nsubint();
  unsigned npol = calibrator->get_npol();

  for (unsigned isub=0; isub<nsub; isub++) {

    get_levels (isub, nchan, cal_hi, cal_lo);

    if (nsub > 1) {
      if (isub == 1) {
	total_hi.resize (cal_hi.size());
	total_lo.resize (cal_lo.size());
	for (unsigned ipol=0; ipol<npol; ipol++) {
	  total_hi[ipol].resize (nchan);
	  total_lo[ipol].resize (nchan);
	}
      }
      for (unsigned ipol=0; ipol<npol; ipol++) {
	for (unsigned ichan=0; ichan<nchan; ichan++) {
	  total_hi[ipol][ichan] += cal_hi[ipol][ichan];
	  total_lo[ipol][ichan] += cal_lo[ipol][ichan];
	}
      }
    }

  }

  if (nsub > 1) {
    for (unsigned ipol=0; ipol<npol; ipol++) {
      for (unsigned ichan=0; ichan<nchan; ichan++) {
	cal_hi[ipol][ichan] = total_hi[ipol][ichan].get_Estimate();
	cal_lo[ipol][ichan] = total_lo[ipol][ichan].get_Estimate();
      }
    }
  }

  calculate (cal_hi, cal_lo);
}

void Pulsar::PolnCalibrator::calculate (vector<vector<Estimate<double> > >& hi,
					vector<vector<Estimate<double> > >& lo)
{
  if (verbose)
    cerr << "Pulsar::PolnCalibrator::calculate" << endl;

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

