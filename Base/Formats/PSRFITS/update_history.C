#include "Pulsar/FITSArchive.h"
#include "Pulsar/ProcHistory.h"
#include "string_utils.h"

void Pulsar::FITSArchive::update_history()
{
  ProcHistory* history = get<ProcHistory>();

  if (!history)
    add_extension (history = new ProcHistory);

  // Construct the new final row
  history->add_blank_row();
  
  time_t timeval = time(0);
  string timestr = ctime (&timeval);

  history->get_last().date_pro = chop(timestr);
  
  history->get_last().proc_cmd = history->get_command_str();
  
  if (get_state() == Signal::PPQQ)
    history->get_last().pol_type = "XXYY";
  else if (get_state() == Signal::Stokes)
    history->get_last().pol_type = "STOKE";
  else if (get_state() == Signal::Coherence)
    history->get_last().pol_type = "XXYYCRCI";
  else if (get_state() == Signal::Intensity)
    history->get_last().pol_type = "INTEN";
  else if (get_state() == Signal::Invariant)
    history->get_last().pol_type = "INVAR";
  else
    history->get_last().pol_type = "UNKNOWN";

  history->get_last().npol = get_npol();
  history->get_last().nbin = get_nbin();
  history->get_last().nbin_prd = get_nbin();

  if ( get_nsubint() )
    history->get_last().tbin = get_Integration(0)->get_folding_period() / get_nbin();
  else
    history->get_last().tbin = 0.0;

  history->get_last().ctr_freq = get_centre_frequency();
  history->get_last().nchan = get_nchan();
  history->get_last().chanbw = get_bandwidth() / float(get_nchan());
  history->get_last().par_corr = get_parallactic_corrected();
  history->get_last().rm_corr = get_ism_rm_corrected();
  history->get_last().dedisp = get_dedispersed();
  
  if (get_poln_calibrated())
    history->get_last().cal_mthd = history->get_cal_mthd();
  else
    history->get_last().cal_mthd = "NONE";
  
  if (get_flux_calibrated())
    history->get_last().sc_mthd = history->get_sc_mthd();
  else
    history->get_last().sc_mthd = "NONE";
  
  history->get_last().cal_file = history->get_cal_file();
  history->get_last().rfi_mthd = history->get_rfi_mthd();;
}
