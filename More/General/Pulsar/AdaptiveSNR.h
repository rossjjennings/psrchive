//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/AdaptiveSNR.h,v $
   $Revision: 1.4 $
   $Date: 2005/12/09 16:41:07 $
   $Author: straten $ */

#ifndef __Pulsar_AdaptiveSNR_h
#define __Pulsar_AdaptiveSNR_h

#include "Algorithm.h"

namespace Pulsar {

  class Profile;
  class BaselineEstimator;

  //! Calculates the signal-to-noise ratio using a BaselineEstimator
  class AdaptiveSNR : public Algorithm {

  public:

    //! Default constructor
    AdaptiveSNR ();

    //! Destructor
    ~AdaptiveSNR ();

    //! Set the function used to compute the baseline
    void set_baseline (BaselineEstimator* function);

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

  protected:

    //! The function used to compute the baseline
    Reference::To<BaselineEstimator> baseline;

  };

}

#endif
