//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Polar.h,v $
   $Revision: 1.9 $
   $Date: 2008/04/30 15:32:33 $
   $Author: straten $ */

#ifndef __MEAL_Polar_H
#define __MEAL_Polar_H

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2.h"
#include "Quaternion.h"
#include "Estimate.h"

namespace MEAL {

  class Gain;
  class Boost;
  class Rotation;

  //! Represents the polar decomposition of a transformation
  /*! The transformation is parameterized by the gain, G, the boost vector,
    \f${\bf b}=\sinh\beta\hat{m}\f$, and the rotation vector,
    \f${\bf r}=\phi_i\hat{n}\f$. */
  class Polar : public ProductRule<Complex2> {

  public:

    //! Default Constructor
    Polar ();

    //! Copy Constructor
    Polar (const Polar& s);

    //! Assignment Operator
    const Polar& operator = (const Polar& s);

    //! Destructor
    ~Polar ();

    //! Get the instrumental gain, \f$ G \f$, in calibrator voltage units
    Estimate<double> get_gain () const;

    //! Get the specified component of the boost vector
    Estimate<double> get_boost (unsigned i) const;
    
    //! Get the specified component of the rotation vector
    Estimate<double> get_rotation (unsigned i) const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator voltage units
    void set_gain (const Estimate<double>& gain);

    //! Set the specified component of the boost vector
    void set_boost (unsigned i, const Estimate<double>& b_i);
    
    //! Set the specified component of the rotation vector
    void set_rotation (unsigned i, const Estimate<double>& phi_i);

    //! Polar decompose the Jones matrix
    void solve (Jones< Estimate<double> >& jones);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Gain Complex2
    Reference::To<Gain> gain;

    //! Boost Complex2
    Reference::To<Boost> boost;

    //! Rotation Complex2s
    Reference::To<Rotation> rotation;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

