//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Andrew Jameson
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_FITSSKLoader_h
#define __Pulsar_FITSSKLoader_h

#include "Pulsar/FITSArchive.h"
#include "Pulsar/SpectralKurtosis.h"

namespace Pulsar {

  class FITSArchive::SKLoader : public Pulsar::SpectralKurtosis::Loader {

  public:

    //! Default constructor
    SKLoader ();

    ~SKLoader ();

    void init (fitsfile* fptr, int row, int hdu_num);

    void load (SpectralKurtosis * instance);

  protected:

    //! PSRFITS file pointer for loading
    fitsfile* fptr;

    //! row for this integration
    int row;

    //! HDU num for the Spectral Kurtosis HDU
    int hdu_num;

  };
}

#endif

