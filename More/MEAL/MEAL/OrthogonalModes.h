//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/OrthogonalModes.h

#ifndef __MEAL_OrthogonalModes_H
#define __MEAL_OrthogonalModes_H

#include "MEAL/Coherency.h"
#include "MEAL/Wrap.h"

namespace MEAL {

  //! A sum of orthogonally polarized modes
  class OrthogonalModes : public Wrap<Coherency>
  {

  public:

    //! Default constructor
    OrthogonalModes ();

    //! Copy constructor
    OrthogonalModes (const OrthogonalModes& copy);

    //! Assignment operator
    OrthogonalModes& operator = (const OrthogonalModes& copy);

    //! Destructor
    ~OrthogonalModes ();

    //! Mode A
    Coherency* get_modeA () const { return modeA; }

    //! Mode B
    Coherency* get_modeB () const { return modeB; }

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Works for the constructors
    void init ();

    Reference::To<Coherency> modeA;
    Reference::To<Coherency> modeB;
  };

}

#endif
