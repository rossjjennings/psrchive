//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolnCalExtFreqIntegrate.h

#ifndef __Pulsar_PolnCalExtFreqIntegrate_h
#define __Pulsar_PolnCalExtFreqIntegrate_h

#include "Pulsar/Integrate.h"
#include "Pulsar/EvenlySpaced.h"
#include "Pulsar/EvenlyWeighted.h"
#include "Pulsar/PolnCalibratorExtension.h"

namespace Pulsar {

  //! Integrates frequency channels in a polarization calibration extension
  class PolnCalExtFreqIntegrate : public Integrate<PolnCalibratorExtension>
  {

  public:

    //! Default constructor
    PolnCalExtFreqIntegrate ();

    //! The frequency integration operation
    void transform (PolnCalibratorExtension*);

    //! Policy for producing evenly spaced frequency channel ranges
    class EvenlySpaced;

    //! Policy for producing evenly distributed frequency channel ranges
    class EvenlyWeighted;

  };

  class PolnCalExtFreqIntegrate::EvenlySpaced :
    public Integrate<PolnCalibratorExtension>::EvenlySpaced
  {
    unsigned get_size (const PolnCalibratorExtension* sub)
    { return sub->get_nchan(); }
  };

  class PolnCalExtFreqIntegrate::EvenlyWeighted : 
    public Integrate<PolnCalibratorExtension>::EvenlyWeighted
  {
    unsigned get_size (const PolnCalibratorExtension* sub)
    { return sub->get_nchan(); }

    double get_weight (const PolnCalibratorExtension* sub, unsigned ichan)
    { return sub->get_weight (ichan); }
  };

}

#endif
