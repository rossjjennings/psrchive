//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Smooth.h,v $
   $Revision: 1.2 $
   $Date: 2004/04/16 00:27:56 $
   $Author: ahotan $ */

#ifndef __Pulsar_Smooth_h
#define __Pulsar_Smooth_h

#include <memory>

#include "ReferenceAble.h"

namespace Pulsar {

  class Profile;

  //! Abstract base class of cyclical smoothing algorithms
  class Smooth : public Reference::Able {

  public:

    //! Default constructor
    Smooth ();

    //! Destructor
    virtual ~Smooth ();

    //! Set the width of the window used to smooth
    void set_duty_cycle (float duty_cycle);

    //! Smooth the given Profile
    void smooth (Profile* profile);

  protected:

    //! The smoothing function
    /*! Derived classes are passed the array into which the output
     must be written and the unwrapped input data array.
     \param nbin   size of output array
     \param output output array
     \param wbin   size of the smoothing window (always odd)
     \param input  input data with wbin/2 wrap-around points added to ends */
    virtual void smooth_data (unsigned nbin, float* output,
			      unsigned wbin, float* input) = 0;

    //! The fractional number of high frequencies used to calculate noise
    float duty_cycle;

  };

}

#endif
