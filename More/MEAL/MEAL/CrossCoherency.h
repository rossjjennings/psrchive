//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/CrossCoherency.h

#ifndef __MEAL_CrossCoherency_H
#define __MEAL_CrossCoherency_H

#include "MEAL/SingularCoherency.h"
#include "MEAL/Wrap.h"

namespace MEAL {

  class JonesSpinor;
  class Complex;

  //! Partial coherence of 100% polarized modes

  /*! The modes are converted into spinor representation, such that
    the cross coherency is completely described by a single complex
    correlation coefficient. */

  class CrossCoherency : public Wrap<Coherency>
  {

  public:

    //! Default constructor
    CrossCoherency ();

    //! Copy constructor
    CrossCoherency (const CrossCoherency& copy);

    //! Assignment operator
    CrossCoherency& operator = (const CrossCoherency& copy);

    //! Destructor
    ~CrossCoherency ();

    void set_modeA (SingularCoherency*);
    void set_modeB (SingularCoherency*);
    void set_correlation (Complex*);

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! The complex correlation coefficient
    Reference::To<Complex> correlation;

    //! The spinor representation of mode A
    Reference::To<JonesSpinor> modeA;

    //! The spinor representation of mode B
    Reference::To<JonesSpinor> modeB;

    //! Works for the constructors
    void init ();
  };

}

#endif
