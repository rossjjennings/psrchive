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

#include <stdlib.h>
#include <assert.h>

using namespace std;
using namespace Pulsar;

void load (fitsfile* fptr, int& row,
	   CalibrationInterpolatorExtension::Parameter* param,
	   unsigned nrows)
{
  int code = 0;
  psrfits_read_col (fptr, "MODEL", &code, row);
  param->code = (CalibrationInterpolatorExtension::Parameter::Type) code;

  int iparam = 0;
  psrfits_read_col (fptr, "IPARAM", &iparam, row);
  param->iparam = iparam;
  
  psrfits_read_col (fptr, "LOGSMTH", &(param->log10_smoothing_factor), row);
  psrfits_read_col (fptr, "CHISQ", &(param->total_chi_squared), row);

  int ndat = 0;
  psrfits_read_col (fptr, "NDAT", &ndat, row);
  param->ndat_input = ndat;
  
  int nflag = 0;
  psrfits_read_col (fptr, "NFLAG_IN", &nflag, row);
  param->ndat_flagged_before = nflag;
  
  psrfits_read_col (fptr, "NFLAG_OUT", &nflag, row);
  param->ndat_flagged_after = nflag;
  
  int first_code = code;
  int first_iparam = iparam;
  
  while (iparam == first_iparam && code == first_code)
  {
    string txt;
    psrfits_read_col (fptr, "INTERTXT", &txt, row);
    param->interpolator += txt;
    
    row ++;

    if (row <= nrows)
    {
      psrfits_read_col (fptr, "MODEL", &code, row);
      psrfits_read_col (fptr, "IPARAM", &iparam, row);
    }
    else
      iparam = -1;  // break the loop
  }
}

void FITSArchive::load_CalibrationInterpolatorExtension (fitsfile* fptr) try
{
  if (verbose == 3)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension entered"
	 << endl;
  
  // Move to the PCMINTER HDU
  bool optional = true;
  if (!psrfits_move_hdu (fptr, "PCMINTER", optional))
    return;

  // Get CAL_MTHD
  string cal_mthd;
  psrfits_read_key (fptr, "CAL_MTHD", &cal_mthd);

  if (cal_mthd == "")
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_CalibrationInterpolatorExtension"
              " ignoring CAL_MTHD=''" << endl;
    return;
  }

  Reference::To<CalibrationInterpolatorExtension> ext;
  ext = new CalibrationInterpolatorExtension;

  if (verbose > 2)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension "
            "CAL_MTHD='" << cal_mthd << "'" << endl;

  ext->set_type( Calibrator::Type::factory (cal_mthd) );

  if (verbose == 3)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension type=" 
	 << ext->get_type()->get_name() << endl;

  // Get COUPLING (new in PSRFITS version 6.5)
  string coupling = "BeforeBasis";
  psrfits_read_key (fptr, "COUPLING", &coupling, coupling, verbose > 2);

  auto point = fromstring<CalibratorStokes::CouplingPoint> (coupling);
  ext->set_coupling_point (point);

  // Get NRCVR (new in PSRFITS version 6.6)
  int nrcvr = 2;
  psrfits_read_key (fptr, "NRCVR", &nrcvr, nrcvr, verbose > 2);
  ext->set_nreceptor (nrcvr);

  // Get SCALE (new in PSRFITS version 6.6)
  string scale = "S_CAL";
  psrfits_read_key (fptr, "SCALE", &scale, scale, verbose > 2);

  if (scale == "SCALE")
    ext->set_native_scale (true);
  else
    ext->set_native_scale (false);
    
  // NPARAM
  unsigned nparam = 0;
  psrfits_read_key (fptr, "NPARAM", &nparam);

  // NCHAN_IN
  unsigned nchan_in = 0;
  psrfits_read_key (fptr, "NCHAN_IN", &nchan_in);
  ext->set_nchan_input (nchan_in);

  // NSUB_IN
  unsigned nsub_in = 0;
  psrfits_read_key (fptr, "NSUB_IN", &nsub_in);
  ext->set_nsub_input (nsub_in);

  // REFFREQ
  double reffreq = 0;
  psrfits_read_key (fptr, "REFFREQ", &reffreq);
  ext->set_reference_frequency (reffreq);
  
  // MINFREQ
  double minfreq = 0;
  psrfits_read_key (fptr, "MINFREQ", &minfreq);
  ext->set_minimum_frequency (minfreq);
  
  // MAXFREQ
  double maxfreq = 0;
  psrfits_read_key (fptr, "MAXFREQ", &maxfreq);
  ext->set_maximum_frequency (maxfreq);
  
  // REFEPOCH
  double refepoch = 0;
  psrfits_read_key (fptr, "REFEPOCH", &refepoch);
  ext->set_reference_epoch (refepoch);
  
  // MINEPOCH
  double minepoch = 0;
  psrfits_read_key (fptr, "MINEPOCH", &minepoch);
  ext->set_minimum_epoch (minepoch);
  
  // MAXEPOCH
  double maxepoch = 0;
  psrfits_read_key (fptr, "MAXEPOCH", &maxepoch);
  ext->set_maximum_epoch (maxepoch);

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
		     "FITSArchive::load_CalibrationInterpolatorExtension",
                     "fits_get_coltype (name=INTERTXT, col=%d)", colnum);

  if (typecode != TSTRING)
    throw FITSError (status,
		     "FITSArchive::load_CalibrationInterpolatorExtension",
                     "INTERTXT (col=%d) is not TSTRING", colnum);

  long nrows = 0;
  fits_get_num_rows(fptr, &nrows, &status);

  int row = 1;
  for (unsigned iparam = 0; iparam < nparam; iparam++)
  {
    auto param = new CalibrationInterpolatorExtension::Parameter;
    ::load (fptr, row, param, nrows);
    ext->add_parameter (param);
  }

  if (verbose == 3)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension " << row
	 << " rows read" << endl;
    
  add_extension (ext);
}
catch (Error& error)
{
  throw error += "FITSArchive::load CalibrationInterpolatorExtension";
}

