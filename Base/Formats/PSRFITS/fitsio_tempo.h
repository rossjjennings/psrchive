//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/PSRFITS/fitsio_tempo.h,v $
   $Revision: 1.1 $
   $Date: 2007/05/16 23:59:04 $
   $Author: straten $ */

#ifndef __FITSIO_TEMPO_H
#define __FITSIO_TEMPO_H

#include <fitsio.h>

#include "psrephem.h"
#include "polyco.h"

  void load   (fitsfile*, psrephem*, long row=0);
  void unload (fitsfile*, const psrephem*, long row=0);

  void fits_map (fitsfile*, std::vector<int>& ephind, int& maxstrlen);

  void load (fitsfile*, polynomial*, long row);
  void unload (fitsfile*, const polynomial*, long row);

  void load (fitsfile*, polyco*, int back=0);
  void unload (fitsfile*, const polyco*, int back=0);

#endif

