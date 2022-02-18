/***************************************************************************
 *
 *   Copyright (C) 2003-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"
#include "Pulsar/AuxColdPlasma.h"

#include "strutil.h"

using namespace std;

string fits_scale_string (Signal::Scale scale, bool verbose)
{
  switch (scale)
  {
  case Signal::FluxDensity:
    return "FluxDen";
  case Signal::ReferenceFluxDensity:
    return "RefFlux";
  case Signal::Jansky:
    return "Jansky";
  default:
    if (verbose)
      cerr << "PSRFITS WARNING output SCALE unknown" << endl;
    return "UNKNOWN";
  }
}

string fits_state_string (Signal::State state, bool verbose)
{
  switch (state)
  {
  case Signal::Nyquist:
    return "AB";
  case Signal::Analytic:
    return "ARAIBRBI";
  case Signal::PPQQ:
    return "AABB";
  case Signal::Stokes:
    return "IQUV";
  case Signal::PseudoStokes:
    return "PSTOKES";
  case Signal::Coherence:
    return "AABBCRCI";
  case Signal::Intensity:
    return "INTEN";
  case Signal::Invariant:
    return "INVAR";
  default:
    if (verbose)
      cerr << "PSRFITS WARNING output POL_TYPE unknown" << endl;
    return "UNKNOWN";
  }
}

void Pulsar::FITSArchive::update_history()
{
  ProcHistory* history = getadd<ProcHistory>();

  /*
    Add a new row only if the data have been loaded from a PSRFITS file,
    in which case the history should be updated, or there is no current
    row
  */

  if (history->get_nrow() == 0 || loaded_from_fits)
    history->add_blank_row();
  
  time_t timeval = time(0);
  string timestr = ctime (&timeval);

  ProcHistory::row& last = history->get_last();

  last.date_pro = remove_all(timestr,'\n');
  
  last.proc_cmd = history->get_command_str();

  last.scale = state_scale = fits_scale_string(get_scale(), verbose > 1);

  last.pol_type = state_pol_type = fits_state_string(get_state(), verbose > 1);

  last.nsub = get_nsubint();
  last.npol = get_npol();
  last.nbin = get_nbin();
  last.nbin_prd = get_nbin();

  if ( get_nsubint() )
  {
    double gate = get_Integration(0)->get_gate_duty_cycle();
    last.nbin_prd = round (last.nbin_prd / gate);

    double diff = fabs(last.nbin_prd * gate - last.nbin);
    if ( diff > 0.01 )
      warning << "FITSArchive::update_history nbin=" << last.nbin 
              << " divided by gate=" << gate << " does not result in an integer"
              << " nbin_prd=" << last.nbin/gate << endl;

    double interval = gate * get_Integration(0)->get_folding_period();
    last.tbin = interval / get_nbin();
  }
  else
    last.tbin = 0.0;

  last.ctr_freq = get_centre_frequency();
  last.nchan = get_nchan();
  last.chan_bw = get_bandwidth() / float(get_nchan());

  last.rotation_measure = get_rotation_measure ();
  last.rm_corr = get_faraday_corrected();

  last.dispersion_measure = get_dispersion_measure ();
  last.dedisp = get_dedispersed();

  Receiver* receiver = get<Receiver>();
  if (!receiver)
  {
    last.pr_corr = false;
    last.fd_corr = false;
  }
  else
  {
    last.pr_corr = receiver->get_projection_corrected();
    last.fd_corr = receiver->get_basis_corrected();
  }

  Backend* backend = get<Backend>();
  if (!backend)
    last.be_corr = false;
  else
    last.be_corr = backend->get_corrected();

  if (get_poln_calibrated())
  {
    if (history->get_cal_mthd() == "NONE")
      history->set_cal_mthd("Other");
    last.cal_mthd = history->get_cal_mthd();
  }
  else
    last.cal_mthd = "NONE";

  last.cal_file = history->get_cal_file();
  last.rfi_mthd = history->get_rfi_mthd();

  /*
    Auxiliary RM and DM correction information
  */

  AuxColdPlasma* aux = get<AuxColdPlasma> ();
  if (aux)
  {
    last.aux_dm_model = aux->get_dispersion_model_name ();
    last.aux_dm_corr = aux->get_dispersion_corrected ();

    last.aux_rm_model = aux->get_birefringence_model_name ();
    last.aux_rm_corr = aux->get_birefringence_corrected ();
  }
}
