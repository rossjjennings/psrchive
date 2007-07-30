//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeightShift.h,v $
   $Revision: 1.1 $
   $Date: 2007/07/30 06:04:29 $
   $Author: straten $ */

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

    //! Returns the shift
    Functor< double() > get_shift;

  protected:

    //! Derived classes implement the PhaseWeight calculation
    void calculate (PhaseWeight*);

  };

}


#endif // !defined __Pulsar_BaselineWeight_h
