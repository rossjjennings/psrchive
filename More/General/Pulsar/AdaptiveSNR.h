//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/AdaptiveSNR.h,v $
   $Revision: 1.2 $
   $Date: 2004/04/27 15:26:29 $
   $Author: straten $ */

#ifndef __Pulsar_AdaptiveSNR_h
#define __Pulsar_AdaptiveSNR_h

#include "Reference.h"

namespace Pulsar {

  class Profile;
  class BaselineFunction;

  //! Calculates the signal-to-noise ratio using a baseline computing function
  class AdaptiveSNR : public Reference::Able {

  public:

    //! Default constructor
    AdaptiveSNR ();

    //! Destructor
    ~AdaptiveSNR ();

    //! Set the function used to compute the baseline
    void set_baseline (BaselineFunction* function);

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

  protected:

    //! The function used to compute the baseline
    Reference::To<BaselineFunction> baseline;

  };

}

#endif
