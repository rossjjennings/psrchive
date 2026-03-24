/***************************************************************************
 *
 *   Copyright (C) 2022 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ConfigurableProjectionExtension.h"
#include "CalibratorExtensionIO.h"

#include "psrfitsio.h"
#include "strutil.h"

// #define _DEBUG 1
#include "debug.h"

#include <assert.h>

using namespace std;

void unload_variances (fitsfile*, const Pulsar::ConfigurableProjectionExtension*,
		       int nparam, vector<float>& data);

void unload_covariances (fitsfile*, const Pulsar::ConfigurableProjectionExtension*,
			 int ncovar, vector<float>& data);

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const ConfigurableProjectionExtension* cpe) try
{
  if (verbose > 2)
    cerr << "FITSArchive::unload ConfigurableProjectionExtension entered" << endl;
  
  // Initialize the CFGPROJ Binary Table
  
  psrfits_init_hdu (fptr, "CFGPROJ");  

  int nchan = cpe->get_nchan();
  int nparam = cpe->get_nparam();
  int ncovar = 0;

  if (cpe->get_has_covariance())
    ncovar = nparam * (nparam+1) / 2;

  if (nparam == 0)
    throw Error (InvalidState, "FITSArchive::unload ConfigurableProjectionExtension",
		 "number of model parameters == 0");

  if (verbose > 2)
    cerr << "FITSArchive::unload ConfigurableProjectionExtension nchan=" 
	 << nchan <<  " nparam=" << nparam << " ncovar=" << ncovar << endl;

  string config = cpe->get_configuration();

  // Write configuration as long string
  int status = 0;
  char* comment = "[Long String] Projection configuration";

  fits_write_key_longwarn (fptr, &status);
  fits_write_key_longstr (fptr, "CONFIG", const_cast<char*>(config.c_str()), 
                          comment, &status);

  // Write NPARAM
  psrfits_update_key (fptr, "NPARAM", nparam);

  // Write NCOVAR
  psrfits_update_key (fptr, "NCOVAR", ncovar);

  // Write NCHAN
  psrfits_update_key (fptr, "NCHAN", nchan);

  const ConfigurableProjectionExtension::Transformation* valid = 0;
  for (int ichan=0; ichan < nchan; ichan++)
    if (cpe->get_valid(ichan))
      valid = cpe->get_transformation(ichan);

  if (valid)
  {
    assert (valid->get_nparam() == unsigned(nparam));
    for (unsigned iparam=0; iparam < valid->get_nparam(); iparam++)
    {
      string key = stringprintf ("PAR_%04d", iparam);
      psrfits_update_key (fptr, key.c_str(), valid->get_param_name(iparam),
			  valid->get_param_description(iparam).c_str());
    }
  }

  vector<float> wts ( nchan );
  for (int i = 0; i < nchan; i++)
    wts[i] = cpe->get_valid(i);

  vector<unsigned> dimensions;

  // Write the weights
  psrfits_write_col (fptr, "DAT_WTS", 1, wts, dimensions);

  if (verbose > 2) cerr << "FITSArchive::unload CalibratorStokes"
                 " weights written" << endl;

  long dimension = nchan * nparam;  
  vector<float> data( dimension, 0.0 );

  int count = 0;
  for (count = 0; count < dimension; count++)
    data[count] = fits_nullfloat;

  count = 0;
  for (int ichan = 0; ichan < nchan; ichan++)
  {
    if (cpe->get_valid(ichan))
    {
      DEBUG ("FITSArchive::unload ConfigurableProjectionExtension ichan=" << ichan << " valid");
      for (int j = 0; j < nparam; j++)
      {
	data[count] = cpe->get_transformation(ichan)->get_param(j);
        DEBUG ("\t" << j << " " << data[count]);
	count++;
      }
    }
    else
    {
      DEBUG ("FITSArchive::unload ConfigurableProjectionExtension ichan=" << ichan << " invalid");
      count += nparam;
    }
  }

  assert (count == dimension);

  dimensions.resize (2);
  dimensions[0] = nparam;
  dimensions[1] = nchan;

  psrfits_write_col (fptr, "DATA", 1, data, dimensions);

  if (ncovar)
    unload_covariances (fptr, cpe, ncovar, data);
  else
    unload_variances (fptr, cpe, nparam, data);

  if (verbose > 2)
    cerr << "FITSArchive::unload ConfigurableProjectionExtension exiting" << endl; 
}
catch (Error& error)
{
  throw error += "FITSArchive::unload ConfigurableProjectionExtension";
}

void unload_variances (fitsfile* fptr,
		       const Pulsar::ConfigurableProjectionExtension* cpe,
		       int nparam, vector<float>& data)
{
  unsigned nchan = cpe->get_nchan();

  data.resize( nparam * nchan );

  unsigned count = 0;
  for (unsigned i = 0; i < nchan; i++) {
    if (cpe->get_valid(i)) {
      for (int j = 0; j < nparam; j++) {
	    data[count] = sqrt(cpe->get_transformation(i)->get_variance(j));
	    count++;
      }
    }
    else {
      count += nparam;
    }
  }

  assert (count == data.size());

  vector<unsigned> dimensions (2);
  dimensions[0] = nparam;
  dimensions[1] = nchan;

  psrfits_write_col (fptr, "DATAERR", 1, data, dimensions);
  psrfits_delete_col (fptr, "COVAR");
}

void unload_covariances (fitsfile* fptr,
			 const Pulsar::ConfigurableProjectionExtension* cpe,
			 int ncovar, vector<float>& data)
{
  unsigned nchan = cpe->get_nchan();

  if (Pulsar::Archive::verbose > 2)
    cerr << "FITSArchive::unload ConfigurableProjectionExtension"
      " ncovar = " << ncovar << endl;

  data.resize( ncovar * nchan );

  vector<double> covar;
  unsigned count = 0;

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    bool zero = false;

    if (!cpe->get_valid(ichan))
      zero = true;

    if (!zero)
      cpe->get_transformation(ichan)->get_covariance (covar);

    if (covar.size() == 0)
    {
      DEBUG ("unload_covariances ichan=" << ichan << " zero");
      zero = true;
    }
    else
    {
      DEBUG ("unload_covariances ichan=" << ichan << " ncovar=" << covar.size());
      assert (covar.size() == unsigned(ncovar));
    }

    for (int icovar = 0; icovar < ncovar; icovar++)
    {
      if (zero)
      {
	    data[count] = 0;
      }
      else
      {
            DEBUG ("\t" << icovar << " " << covar[icovar]);
	    data[count] = covar[icovar];
      }
      count++;
    }
  }

  assert (count == data.size());

  vector<unsigned> dimensions (2);
  dimensions[0] = ncovar;
  dimensions[1] = nchan;

  psrfits_delete_col (fptr, "DATAERR");
  psrfits_write_col (fptr, "COVAR", 1, data, dimensions);
}

