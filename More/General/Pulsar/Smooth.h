//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Smooth.h,v $
   $Revision: 1.5 $
   $Date: 2005/12/09 16:41:07 $
   $Author: straten $ */

#ifndef __Pulsar_Smooth_h
#define __Pulsar_Smooth_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing algorithms
  class Smooth : public Transformation<Profile> {

  public:

    //! Default constructor
    Smooth ();

    //! Destructor
    virtual ~Smooth ();

    //! Set the width of the window used to smooth
    void set_duty_cycle (float duty_cycle);

    //! Get the width of the window used to smooth
    float get_duty_cycle () const;

    //! Smooth the given Profile
    void transform (Profile* profile);

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

    //! The width of the window used to smooth
    float duty_cycle;

  };

}

#endif
