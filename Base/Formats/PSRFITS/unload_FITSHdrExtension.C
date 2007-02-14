/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FITSArchive.h"
#include "Pulsar/FITSHdrExtension.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, const FITSHdrExtension* ext)
{
  psrfits_update_key (fptr, "COORD_MD", ext->coordmode);
  psrfits_update_key (fptr, "EQUINOX", ext->equinox);
  psrfits_update_key (fptr, "DATE", ext->creation_date);
  psrfits_update_key (fptr, "TRK_MODE", ext->trk_mode);
  psrfits_update_key (fptr, "DATE-OBS", ext->stt_date + "T" + ext->stt_time);
  psrfits_update_key (fptr, "STT_LST", ext->stt_lst);
}

