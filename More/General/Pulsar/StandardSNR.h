//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/StandardSNR.h,v $
   $Revision: 1.3 $
   $Date: 2004/08/09 04:22:17 $
   $Author: ahotan $ */

#ifndef __Pulsar_StandardSNR_h
#define __Pulsar_StandardSNR_h

#include "Reference.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio by fitting against a standard
  class StandardSNR : public Reference::Able {

  public:

    //! Set the standard against which the S/N will be calculated
    void set_standard (const Profile* profile);

    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);

    //! Return the S/N based on a difference power computation
    float get_morph_snr (const Profile* profile);

  protected:

    //! The standard against which the S/N will be calculated
    Reference::To<const Profile> standard;

  };

}

#endif
