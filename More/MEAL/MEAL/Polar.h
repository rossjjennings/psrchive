//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Polar.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_Polar_H
#define __MEAL_Polar_H

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2.h"
#include "Estimate.h"
#include "Stokes.h"

namespace MEAL {

  class Gain;
  class Boost;
  class Rotation;

  //! Represents the polar decomposition of a transformation
  /*! The transformation is parameterized by the gain, G, the boost vector,
    \f${\bf b}=\sinh\beta\hat{m}\f$, and three rotations, \f$\phi_i\f$. */
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

    //! Get the specified component of the Boost Gibbs vector
    Estimate<double> get_boostGibbs (unsigned i) const;
    
    //! Get the specified basis rotation in radians
    Estimate<double> get_rotationEuler (unsigned i) const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator voltage units
    void set_gain (const Estimate<double>& gain);

    //! Set the specified component of the Boost Gibbs vector
    void set_boostGibbs (unsigned i, const Estimate<double>& b_i);
    
    //! Set the specified basis rotation angle in radians
    void set_rotationEuler (unsigned i, const Estimate<double>& phi_i);
 
    //! Given the source and sky states, solve for most of the parameters
    void solve (Quaternion<Estimate<double>, Hermitian>& source,
		Quaternion<Estimate<double>, Hermitian>& sky);

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
    Reference::To<Rotation> rotation[3];

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

