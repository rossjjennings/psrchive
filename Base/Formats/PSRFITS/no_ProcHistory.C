/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"

#include "psrfitsio.h"
#include "FITSError.h"

using namespace std;

/*!  

  This function defines the bare minimum and most likely sensible
  defaults to be assumed when there is no HISTORY HDU in the file.

  \pre the current HDU is the SUBINT binary table

*/
void Pulsar::FITSArchive::no_ProcHistory (fitsfile* fptr)
{
  // Get NBIN 
  int nbin = 0;
  psrfits_read_key (fptr, "NBIN", &nbin);
  set_nbin( nbin );

  // Get NPOL 
  int npol = 0;
  psrfits_read_key (fptr, "NPOL", &npol);
  set_npol( npol );

  if (npol == 4)
    set_state ( Signal::Stokes );
  else if (npol == 2)
    set_state ( Signal::PPQQ );
  else if (npol == 1)
    set_state ( Signal::Intensity );
  else
    throw Error (InvalidState, "Pulsar::FITSArchive::no_ProcHistory",
		 "unhandled npol=%d", npol);

  // Get NCHAN 
  int nchan = 0;
  psrfits_read_key (fptr, "NCH_FILE", &nchan);
  set_nchan( nchan );

}

