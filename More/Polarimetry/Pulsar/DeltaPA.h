//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/DeltaPA.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_DeltaPA_h
#define __Pulsar_DeltaPA_h

#include "Estimate.h"

namespace Pulsar {

  class PolnProfile;

  //! Calculates the position angle change using complex cross-correlation
  class DeltaPA  {

  public:

    //! Default constructor
    DeltaPA () { threshold = 3.0; }

    //! Destructor
    ~DeltaPA () { }

    //! Get the rotation measure 
    Estimate<double> get (const PolnProfile* p0, const PolnProfile* p1) const;

    //! Set the cutoff threshold in units of the baseline noise
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

  protected:

    float threshold;

  };

}

#endif
