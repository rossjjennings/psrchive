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
    
  int nchan = 0;
  psrfits_read_key (fptr, "NCHAN", &nchan, 0, verbose > 2);

  if (!nchan)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_DynamicResponse DYN_RESP HDU contains no data." << endl;
    return;
  }
  
  // Set nchan 
  response->set_nchan( nchan ); 

  int ntime = 0;
  psrfits_read_key (fptr, "NTIME", &ntime, 0, verbose > 2);

  if (!ntime)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_DynamicResponse DYN_RESP HDU contains no data." << endl;
    return;
  }

  response->set_ntime( ntime ); 

  // Get npol from DYN_RESP 
  int npol = 1;
  psrfits_read_key (fptr, "NPOL", &npol, 1, verbose > 2);

  if (!npol)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_DynamicResponse DYN_RESP HDU contains no data" << endl;
    return;
  }

  // Set npol 
  response->set_npol( npol );

  double minfreq = 0;
  psrfits_read_key (fptr, "MINFREQ", &minfreq);
  response->set_minimum_frequency (minfreq);
  
  double maxfreq = 0;
  psrfits_read_key (fptr, "MAXFREQ", &maxfreq);
  response->set_maximum_frequency (maxfreq);

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
  throw error += "FITSArchive::load_DynamicResponse";
}
