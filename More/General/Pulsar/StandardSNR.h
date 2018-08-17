//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/StandardSNR.h

#ifndef __Pulsar_StandardSNR_h
#define __Pulsar_StandardSNR_h

#include "Pulsar/ProfileShiftFit.h"
#include "Pulsar/SNRatioEstimator.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio by fitting against a standard
  class StandardSNR : public SNRatioEstimator {

  public:

    //! Set the standard against which the S/N will be calculated
    void set_standard (const Profile* profile);

    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    StandardSNR* clone () const;

    //! Set the name of the file from which the standard profile will be loaded
    void set_standard_filename (const std::string& filename);

    //! Return the name of the file from which the standard was loaded
    std::string get_standard_filename () const;
    
    
  protected:

    //! Used to perform the fit and compute the S/N
    ProfileShiftFit fit;

    std::string filename;
  };

}

#endif
