/***************************************************************************
 *
 *   Copyright (C) 2024 Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/DynamicResponse.h"

#include "psrfitsio.h"
#include "FITSError.h"

#include <stdlib.h> 

using namespace std;

void Pulsar::FITSArchive::load_DynamicResponse (fitsfile* fptr) try
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::load_DynamicResponse entered" << endl;
   
  // Move to DYN_RESP HDU   
  fits_movnam_hdu (fptr, BINARY_TBL, "DYN_RESP", 0, &status);
  
  if (status == BAD_HDU_NUM)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_DynamicResponse : no DYN_RESP HDU" << endl;
    return;
  }

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_DynamicResponse",
		     "fits_movnam_hdu DYN_RESP");
 
  Reference::To<DynamicResponse> response = new DynamicResponse;

  psrfits_read_key (fptr, "NCHAN", response, &DynamicResponse::set_nchan);
  psrfits_read_key (fptr, "NTIME", response, &DynamicResponse::set_ntime);
  psrfits_read_key (fptr, "NPOL",  response, &DynamicResponse::set_npol);
  psrfits_read_key (fptr, "CFREQ", response, &DynamicResponse::set_centre_frequency);
  psrfits_read_key (fptr, "BW",    response, &DynamicResponse::set_bandwidth);
  psrfits_read_key (fptr, "NPOS_FIR", response, &DynamicResponse::set_impulse_pos);
  psrfits_read_key (fptr, "NNEG_FIR", response, &DynamicResponse::set_impulse_neg);

  double minepoch = 0;
  psrfits_read_key (fptr, "MINEPOCH", &minepoch);
  response->set_minimum_epoch (minepoch);
  
  double maxepoch = 0;
  psrfits_read_key (fptr, "MAXEPOCH", &maxepoch);
  response->set_maximum_epoch (maxepoch);

  response->resize_data();

  if (verbose > 2) 
    cerr << "FITSArchive::load DynamicResponse"
      " nchan=" << response->get_nchan() <<
      " ntime=" << response->get_ntime() <<
      " npol=" << response->get_npol() <<
      " ndat=" << response->get_data().size() << endl;

  psrfits_read_col (fptr, "DATA", response->get_data());
    
  if (verbose > 2)
    cerr << "FITSArchive::load_DynamicResponse loaded" << endl;

  add_extension (response);

  if (verbose > 2)
    cerr << "FITSArchive::load_DynamicResponse exiting" << endl;	  
}

catch (Error& error)
{
  if (verbose)
    cerr << "FITSArchive::load_DynamicResponse ignoring exception" << endl;
  if (verbose > 2)
    cerr << error << endl;
}

