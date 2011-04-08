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

#include "MEAL/Coherency.h"
#include "MEAL/Composite.h"

namespace MEAL {

  class OrthogonalModes;
  class UnitTangent;
  class Scalar;

  //! Partial coherence of orthogonally polarized modes
  class CrossCoherency : public Coherency
  {

  public:

    //! Default constructor
    CrossCoherency (OrthogonalModes*);

    //! Copy constructor
    CrossCoherency (const CrossCoherency& copy);

    //! Assignment operator
    CrossCoherency& operator = (const CrossCoherency& copy);

    //! Destructor
    ~CrossCoherency ();

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! The orthogonal modes
    Project<OrthogonalModes> modes;

    //! The axis that defines the modes
    Project<UnitTangent> axis;

    //! The quasi-Stokes parameters in the natural basis
    Project<Scalar> stokes[4];

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! Composite parameter policy
    Composite composite;

    //! Works for the constructors
    void init ();
  };

}

#endif
