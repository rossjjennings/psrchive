//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/AdaptiveSNR.h,v $
   $Revision: 1.1 $
   $Date: 2004/04/27 06:56:41 $
   $Author: straten $ */

#ifndef __Pulsar_AdaptiveSNR_h
#define __Pulsar_AdaptiveSNR_h

#include "ReferenceAble.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio in the Adaptive domain
  class AdaptiveSNR : public Reference::Able {

  public:

    //! Default constructor
    AdaptiveSNR ();

    //! Set the width of the window used to find the initial baseline
    void set_initial_baseline_window (float width);

    //! Set the threshold below which samples are included in the baseline
    void set_baseline_threshold (float sigma);

    //! Set the (maximum?) number of iterations
    void set_max_iterations (unsigned iterations);

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

  protected:

    //! The width of the window used to find the initial baseline
    float initial_baseline_window;

    //! The threshold below which samples are included in the baseline
    float baseline_threshold;

    //! The (maximum?) number of iterations
    unsigned max_iterations;


  };

}

#endif
