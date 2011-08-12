//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FourthMomentStats.h,v $
   $Revision: 1.11 $
   $Date: 2009/06/24 05:11:32 $
   $Author: straten $ */

#ifndef __Pulsar_FourthMomentStats_h
#define __Pulsar_FourthMomentStats_h

#include "Pulsar/PolnProfileStats.h"

namespace Pulsar {

  class StokesCovariance;

  //! Computes fourth-order moment pulse profile statistics
  class FourthMomentStats : public PolnProfileStats {
    
  public:

    //! Default constructor
    FourthMomentStats (const PolnProfile* profile = 0);

    //! Destructor
    ~FourthMomentStats();

    void set_profile (const PolnProfile* _profile);

    //! Get the eigen polarization profiles
    void eigen (PolnProfile&, PolnProfile&, PolnProfile&);

    //! Get the mode-separated profiles
    void separate (PolnProfile& modeA, PolnProfile& modeB);

  protected:

    Reference::To<const StokesCovariance> covariance;

  };

}


#endif



