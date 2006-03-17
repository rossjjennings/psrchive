/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "psrfitsio.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr,
				  const WidebandCorrelator* ext)
try {

  psrfits_update_key (fptr, "BACKEND",  ext->get_name());
  psrfits_update_key (fptr, "BECONFIG", ext->configfile);
  psrfits_update_key (fptr, "BE_PHASE", (int)ext->get_argument());
  // psrfits_update_key (fptr, "BE_HAND",  (int)ext->get_hand());
  psrfits_update_key (fptr, "NRCVR",    ext->nrcvr);
  psrfits_update_key (fptr, "TCYCLE",   ext->tcycle);

}
catch (Error& error) {
  throw error += "Pulsar::FITSArchive::unload WidebandCorrelator";
}
