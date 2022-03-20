/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/CalibrationInterpolatorExtension.h"

#include "psrfitsio.h"
#include "strutil.h"

// #define _DEBUG 1
#include "debug.h"

#include <assert.h>

using namespace std;
using namespace Pulsar;

void unload (fitsfile* fptr, int row,
	     const CalibrationInterpolatorExtension::Parameter* param,
	     unsigned nchar)
{
  psrfits_write_col (fptr, "MODEL", row, param->code);
  psrfits_write_col (fptr, "IPARAM", row, param->iparam);
  psrfits_write_col (fptr, "LOGSMTH", row, param->log10_smoothing_factor);
  psrfits_write_col (fptr, "CHISQ", row, param->total_chi_squared);
  psrfits_write_col (fptr, "NDAT", row, param->ndat_input);
  psrfits_write_col (fptr, "NFLAG_IN", row, param->ndat_flagged_before);
  psrfits_write_col (fptr, "NFLAG_OUT", row, param->ndat_flagged_after);

  string txt = param->interpolator;
  txt.resize (nchar, 0);
  psrfits_write_col (fptr, "INTERTXT", row, txt);  
}

void FITSArchive::unload (fitsfile* fptr, 
			  const CalibrationInterpolatorExtension* ext) try
{
  if (verbose == 3)
    cerr << "FITSArchive::unload CalibrationInterpolatorExtension" << endl;
  
  // Initialize the PCMINTER Binary Table
  
  psrfits_init_hdu (fptr, "PCMINTER");  

  // CAL_MTHD
  string cal_mthd = ext->get_type()->get_name();
  psrfits_update_key (fptr, "CAL_MTHD", cal_mthd);

  // NPARAM
  unsigned nparam = ext->get_nparam ();
  psrfits_update_key (fptr, "NPARAM", nparam);

  // NCHAN_IN
  unsigned nchan = ext->get_nchan_input();
  psrfits_update_key (fptr, "NCHAN_IN", nchan);

  // NSUB_IN
  unsigned nsub = ext->get_nsub_input();
  psrfits_update_key (fptr, "NSUB_IN", nsub);

  // REFFREQ
  double ref_freq = ext->get_reference_frequency();
  psrfits_update_key (fptr, "REFFREQ", ref_freq);

  // MINFREQ
  double min_freq = ext->get_minimum_frequency();
  psrfits_update_key (fptr, "MINFREQ", min_freq);

  // MAXFREQ
  double max_freq = ext->get_maximum_frequency();
  psrfits_update_key (fptr, "MAXFREQ", max_freq);

  // REFEPOCH
  double ref_epoch = ext->get_reference_epoch().in_days();
  psrfits_update_key (fptr, "REFEPOCH", ref_epoch);

  // MINEPOCH
  double min_epoch = ext->get_minimum_epoch().in_days();
  psrfits_update_key (fptr, "MINEPOCH", min_epoch);

  // MAXEPOCH
  double max_epoch = ext->get_maximum_epoch().in_days();
  psrfits_update_key (fptr, "MAXEPOCH", max_epoch);

  // NCHAR
  unsigned max_strlen = 0;
  for (int iparam=0; iparam < nparam; iparam++)
  {
    unsigned strlen = ext->get_parameter(iparam)->interpolator.size();
    max_strlen = std::max (max_strlen, strlen);
  }

  assert (max_strlen > 0);

  psrfits_update_key (fptr, "NCHAR", max_strlen);

  for (unsigned iparam=0; iparam < nparam; iparam++)
    ::unload (fptr, iparam+1, ext->get_parameter(iparam), max_strlen);
  
  if (verbose == 3)
    cerr << "FITSArchive::unload CalibrationInterpolatorExtension exiting" << endl; 

}
catch (Error& error)
{
  throw error += "FITSArchive::unload CalibrationInterpolatorExtension";
}

