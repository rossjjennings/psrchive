#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

bool Pulsar::PolnCalibrator::verbose = false;


Pulsar::PolnCalibrator::PolnCalibrator (const Pulsar::Archive* arch)
{
  if (verbose)
    cerr << "Pulsar::PolnCalibrator" << endl;

  if (arch->get_type() != Signal::PolCal)
    throw Error (InvalidParam, "Pulsar::PolnCalibrator", "Pulsar::Archive='"
		   + arch->get_filename() + "' not a PolnCal");
  
  // Here the decision is made about full stokes or dual band observations.
  Signal::State state = arch->get_state();

  bool fullStokes = state == Signal::Stokes || state == Signal::Coherence;

  bool calibratable = fullStokes || state == state == Signal::PPQQ;

  if (!calibratable)
    throw Error (InvalidParam, "Pulsar::PolnCalibrator", "Pulsar::Archive='"
		 + arch->get_filename() + "'\n"
		 "invalid state=" + State2string(state));

  gain.resize (nchan);

  if (fullStokes)
    phase.resize (nchan);

  vector<vector<Estimate<double> > > cal_hi;
  vector<vector<Estimate<double> > > cal_lo;
  
  arch->get_Integration(0)->cal_levels (cal_hi, cal_lo);

  unsigned nchan = arch->get_nchan();
  unsigned npol = arch->get_npol();

  unsigned nprobes = 2;

  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    bool invalid = false;
    for (unsigned ipol=0; ipol<nprobes; ++ipol)
      if (cal_hi[ipol][ichan]==0 || cal_hi[i][j] <= cal_lo[i][j])
	invalid = true;

    if (invalid) {
      if (verbose)
	cerr << "Pulsar::PolnCalibrator channel " << ichan
	     << ": invalid levels flagged" << endl;
      continue;
    }

    if (state == Signal::Stokes) {


    }
    else {
 || state == Signal::Coherence;

  bool calibratable = fullStokes || state == state == Signal::PPQQ;
    }

    if (!fullStokes)
      continue;

    double delta2 = cal_hi[2][j] - cal_lo[2][j];
    double delta3 = cal_hi[3][j] - cal_lo[3][j];
    
    if (delta3 == 0 && delta2 == 0)  {
      inst_phase[j] = sigsq_inst_phase[j] = 0;
      if (verbose) cerr << "psrcal_data:: channel " << j << ":"
		     " flagging invalid instrumental phase" << endl;
      continue;
    }
    
    inst_phase[j] = atan2 (delta3, delta2);
    double tmp_num = delta3 * delta3;
    double tmp_denom = delta2 * delta2;
    double denom = tmp_num + tmp_denom;

    double err = (varcal_hi[3][j]+varcal_lo[3][j])/denom +
      (varcal_hi[2][j]+varcal_lo[2][j])*tmp_num/(tmp_denom*denom);
    
    sigsq_inst_phase[j] = (float)err;
    
    if (verbose)
      cerr << "psrcal_data:: channel " << j << ":"
	   << " instrumental phase " << inst_phase[j].getradians() 
	   << " err " << sqrt(sigsq_inst_phase[j]) << endl;
  }
}

