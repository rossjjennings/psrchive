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

void unload (fitsfile* fptr, int& row,
	     const CalibrationInterpolatorExtension::Parameter* param,
	     unsigned bytes_per_row)
{
  psrfits_write_col (fptr, "LOGSMTH", row, param->log10_smoothing_factor);
  psrfits_write_col (fptr, "CHISQ", row, param->total_chi_squared);
  psrfits_write_col (fptr, "NDAT", row, (int) param->ndat_input);
  psrfits_write_col (fptr, "NFLAG_IN", row, (int) param->ndat_flagged_before);
  psrfits_write_col (fptr, "NFLAG_OUT", row, (int) param->ndat_flagged_after);

  string txt = param->interpolator;

  unsigned chars_per_row = bytes_per_row - 1; // leave one for null char
  unsigned nrow = ceil (double(txt.size()) / chars_per_row);

  int code = param->code;
  int iparam = param->iparam;
  
  for (unsigned irow=0; irow < nrow; irow++)
  {
    psrfits_write_col (fptr, "MODEL", row, code);
    psrfits_write_col (fptr, "IPARAM", row, iparam);

    string block = txt.substr (irow*chars_per_row, chars_per_row);
    psrfits_write_col (fptr, "INTERTXT", row, block);
    row ++;
  }
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

  // COUPLING
  string coupling = tostring( ext->get_coupling_point () );
  psrfits_update_key (fptr, "COUPLING", coupling);

  // NRCVR
  unsigned nreceptor = ext->get_nreceptor();
  psrfits_update_key (fptr, "NRCVR", nreceptor);

  // SCALE
  string scale = "NA";

  if (nreceptor)
  {
    if (ext->get_native_scale())
      scale = "SCALE";
    else
      scale = "S_CAL";
  }
  
  psrfits_update_key (fptr, "SCALE", scale);

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

  // INTERTXT
  int colnum = 0;
  int status = 0;
  fits_get_colnum (fptr, CASEINSEN, "INTERTXT", &colnum, &status);
  int typecode = 0;
  long repeat = 0;
  long width = 0;
  
  fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

  if (status)
    throw FITSError (status,
		     "FITSArchive::unload CalibrationInterpolatorExtension",
                     "fits_get_coltype (name=INTERTXT, col=%d)", colnum);

  if (typecode != TSTRING)
    throw FITSError (status,
		     "FITSArchive::unload CalibrationInterpolatorExtension",
                     "INTERTXT (col=%d) is not TSTRING", colnum);
  
  int row = 1;
  for (unsigned iparam=0; iparam < nparam; iparam++)
    ::unload (fptr, row, ext->get_parameter(iparam), width);
  
  if (verbose == 3)
    cerr << "FITSArchive::unload CalibrationInterpolatorExtension " << row
	 << " rows written" << endl; 

}
catch (Error& error)
{
  throw error += "FITSArchive::unload CalibrationInterpolatorExtension";
}

