//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Formats/PSRFITS/fitsio_tempo.h

#ifndef __FITSIO_TEMPO_H
#define __FITSIO_TEMPO_H

#include <fitsio.h>

#include "psrephem.h"
#include "polyco.h"

  void load   (fitsfile*, Legacy::psrephem*, long row=0);
  void unload (fitsfile*, const Legacy::psrephem*, long row=0);

  void fits_map (fitsfile*, std::vector<int>& ephind, int& maxstrlen);

  void load (fitsfile*, polynomial*, long row);
  void unload (fitsfile*, const polynomial*, long row);

  void load (fitsfile*, polyco*, int back=0);
  void unload (fitsfile*, const polyco*, int back=0);

#endif

