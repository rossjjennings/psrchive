//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/More/General/Pulsar/Correlate.h

#ifndef __Pulsar_Correlate_h
#define __Pulsar_Correlate_h

#include "Pulsar/Combination.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Computes the cross-correlation between two profiles
  class Correlate : public Combination<Profile>
  {
  public:

    //! Set argument amplitudes equal to the cross-correlation between argument and operand
    /*! operand is a member of the Combination<Profile> base class*/
    void transform (Profile* argument) override;

    //! Modify the behaviour of subsequent calls to transform method
    /*! If true, call Profile::correlate_normalized to compute the power-normalized cross-correlation
        If false, call Profile::correlate */
    void set_normalize(bool flag) { normalize = flag; }

    protected:

    //! By default, call Profile::correlate_normalized to compute the power-normalized cross-correlation
    bool normalize = true;
  }; 

}

#endif
