//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Feed.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 20:45:59 $
   $Author: straten $ */

#ifndef __Calibration_Feed_H
#define __Calibration_Feed_H

#include "MEAL/SumRule.h"
#include "MEAL/Rotation.h"

#include "Estimate.h"

namespace Calibration {

  //! Represents the feed as a non-ideal combination of two ideal receptors
  /*! This model of the receiver feed is described by Equation 16 of van
    Straten (2003) which is equivalent to the transformation in
    Equation 16 of Britton (2000). */

  class Feed : public MEAL::SumRule<MEAL::Complex2> {

  public:

    //! Default Constructor
    Feed ();

    //! Copy Constructor
    Feed (const Feed& feed);

    //! Assignment Operator
    Feed& operator = (const Feed& feed);

    //! Destructor
    ~Feed ();

    //! Get the orientation, \f$\theta\f$, of the specified receptor in radians
    Estimate<double> get_orientation (unsigned ireceptor) const;

    //! Get the ellipticity, \f$\chi\f$, of the specified receptor in radians
    Estimate<double> get_ellipticity (unsigned ireceptor) const;

    //! Set the orientation, \f$\theta\f$, of the specified receptor in radians
    void set_orientation (unsigned ireceptor, const Estimate<double>& theta);

    //! Set the ellipticity, \f$\chi\f$, of the specified receptor in radians
    void set_ellipticity (unsigned ireceptor, const Estimate<double>& chi);

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! The receptor orientation tranformations
    Reference::To<MEAL::Rotation> orientation[2];

    //! The receptor ellipticity tranformations
    Reference::To<MEAL::Rotation> ellipticity[2];

  private:
    
    //! Working method for the constructors
    void init ();

  };

}

#endif

