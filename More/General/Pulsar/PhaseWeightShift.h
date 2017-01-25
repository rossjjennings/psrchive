//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseWeightShift.h

#ifndef __Pulsar_PhaseWeightShift_h
#define __Pulsar_PhaseWeightShift_h

#include "Pulsar/PhaseWeightModifier.h"
#include "Functor.h"

namespace Pulsar {

  //! Shifts PhaseWeight elements
  class PhaseWeightShift : public PhaseWeightModifier {

  public:

    //! Default constructor
    PhaseWeightShift ();

    //! Return a new copy of self
    PhaseWeightShift* clone () const;

    //! Returns the shift
    Functor< double() > get_shift;

  protected:

    //! Derived classes implement the PhaseWeight calculation
    void calculate (PhaseWeight*);

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
