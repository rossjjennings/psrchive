//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* psrchive/More/MEAL/MEAL/Union.h,v */

#ifndef __MEAL_Union_H
#define __MEAL_Union_H

#include "MEAL/ScalarVector.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Represents the union of disjoint vector subspaces
  class Union : public ScalarVector
  {

  public:

    //! Default constructor
    Union (Composite* policy = 0);

    //! Copy constructor
    Union (const Union& v);

    //! Assignment operator
    Union& operator = (const Union& copy);

    //! Destructor
    ~Union () { }

    //! Add a vector space
    void push_back (ScalarVector*);

    //! Get the dimension of the union
    unsigned size () const { return dimension; }

    //! Return the name of the class
    std::string get_name () const { return "Union"; }

  protected:

    //! Return the result and its gradient
    void calculate (double& result, std::vector<double>* grad);

  private:

    //! The models and their mappings
    std::vector< Project<ScalarVector> > model;

    //! The dimension of the superspace
    unsigned dimension;

    //! Composite parameter policy
    Reference::To<Composite> composite;

  };
  
}

#endif
