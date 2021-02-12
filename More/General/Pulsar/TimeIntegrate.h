//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/TimeIntegrate.h

#ifndef __Pulsar_TimeIntegrate_h
#define __Pulsar_TimeIntegrate_h

#include "Pulsar/Integrate.h"
#include "Pulsar/Archive.h"
#include "Pulsar/EvenlySpaced.h"
#include "Pulsar/EvenlyWeighted.h"

namespace Pulsar {

  //! Integrates sub-integrations in an Archive
  /*!
    Before integrating the sub-integrations:
    <UL>

    <LI> If the data have not already been dedispersed, then for each
    frequency channel of each resulting sub-integration, a weighted
    centre frequency will be calculated and dispersion delays between
    this reference frequency and the individual channel centre
    frequencies will be removed.

    <LI> If the data have not already been corrected for Faraday
    rotation, then for each frequency channel of each resulting
    sub-integration, a weighted centre frequency will be calculated
    and Faraday rotation between this reference frequency and the
    individual channel centre frequencies will be corrected. 

    </UL>
  */
  class TimeIntegrate : public Integrate<Archive> {

  public:

    //! The frequency integration operation
    void transform (Archive*);

    //! Policy for producing evenly spaced sub-integration ranges
    class EvenlySpaced;

    //! Policy for producing evenly distributed sub-integration ranges
    class EvenlyWeighted;

    //! Policy for producing sub-integrations of a specified duration
    class TargetDuration;
  };

  class TimeIntegrate::EvenlySpaced :
    public Integrate<Archive>::EvenlySpaced
  {
    unsigned get_size (const Archive* arch) { return arch->get_nsubint(); }
  };

  class TimeIntegrate::EvenlyWeighted : 
    public Integrate<Archive>::EvenlyWeighted
  {
    unsigned get_size (const Archive* arch) { return arch->get_nsubint(); }
  };

}

#endif
