#include "Pulsar/CalibratorExtension.h"
#include "FITSError.h"
#include "fitsutil.h"

#include <memory>
#include <fitsio.h>

namespace Pulsar {

  void load (fitsfile* fptr, CalibratorExtension* ext);

  void unload (fitsfile* fptr, const CalibratorExtension* ext);

  void load_Estimates (fitsfile* fptr, vector< Estimate<double> >& data,
		       char* column_name);

  void unload_Estimates (fitsfile* fptr, vector< Estimate<double> >& data,
			 char* column_name);
}

