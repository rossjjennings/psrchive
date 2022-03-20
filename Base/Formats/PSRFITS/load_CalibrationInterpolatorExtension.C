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

void load (fitsfile* fptr, int row,
	   CalibrationInterpolatorExtension::Parameter* param)
{
  psrfits_read_col (fptr, "MODEL", &(param->code), row);
  psrfits_read_col (fptr, "IPARAM", &(param->iparam), row);
  psrfits_read_col (fptr, "LOGSMTH", &(param->log10_smoothing_factor), row);
  psrfits_read_col (fptr, "CHISQ", &(param->total_chi_squared), row);
  psrfits_read_col (fptr, "NDAT", &(param->ndat_input), row);
  psrfits_read_col (fptr, "NFLAG_IN", &(param->ndat_flagged_before), row);
  psrfits_read_col (fptr, "NFLAG_OUT", &(param->ndat_flagged_after), row);
  psrfits_read_col (fptr, "INTERTXT", &(param->interpolator), row); 
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

  Reference::To<CalibrationInterpolatorExtension> ext;
  ext = new CalibrationInterpolatorExtension;

  // Get CAL_MTHD
  string cal_mthd;
  psrfits_read_key (fptr, "CAL_MTHD", &cal_mthd);

  if (verbose > 2)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension "
            "CAL_MTHD='" << cal_mthd << "'" << endl;

  ext->set_type( Calibrator::Type::factory (cal_mthd) );

  if (verbose == 3)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension type=" 
	 << ext->get_type()->get_name() << endl;

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
  
  for (unsigned iparam = 0; iparam < nparam; iparam++)
  {
    cerr << "load interpolation row=" << iparam+1 << endl;
    
    auto param = new CalibrationInterpolatorExtension::Parameter;
    ::load (fptr, iparam+1, param);
    ext->add_parameter (param);
  }
  
  add_extension (ext);
  
  if (verbose == 3)
    cerr << "FITSArchive::load_CalibrationInterpolatorExtension exiting" << endl;

}
catch (Error& error)
{
  throw error += "FITSArchive::load CalibrationInterpolatorExtension";
}

