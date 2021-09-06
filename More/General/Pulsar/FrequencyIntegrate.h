//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/FrequencyIntegrate.h

#ifndef __Pulsar_FrequencyIntegrate_h
#define __Pulsar_FrequencyIntegrate_h

#include "Pulsar/Integrate.h"
#include "Pulsar/EvenlySpaced.h"
#include "Pulsar/EvenlyWeighted.h"
#include "Pulsar/NonlinearlySpaced.h"

#include "Pulsar/Integration.h"

namespace Pulsar {

  class Integration;

  //! Integrates frequency channels in an Integration
  /*!
    Before integrating the frequency channels:
    <UL>
    <LI> If the data have not already been dedispersed, then for each
    resulting frequency channel, a weighted centre frequency will be
    calculated and dispersion delays between this reference frequency
    and the individual channel centre frequencies will be removed.

    <LI> If the data have not already been corrected for Faraday
    rotation, then for each resulting frequency channel, a weighted
    centre frequency will be calculated and Faraday rotation between
    this reference frequency and the individual channel centre
    frequencies will be corrected.
    </UL>
  */
  class FrequencyIntegrate : public Integrate<Integration> {

  public:

    //! Default constructor
    FrequencyIntegrate ();

    //! The frequency integration operation
    void transform (Integration*);

    //! Correct dispersion before integrating, if necessary
    void set_dedisperse (bool);
    bool get_dedisperse () const;

    //! Correct Faraday rotation before integrating, if necessary
    void set_defaraday (bool flag);
    bool get_defaraday () const;

    //! Policy for producing evenly spaced frequency channel ranges
    class EvenlySpaced;

    //! Policy for producing evenly weighted frequency channel ranges
    class EvenlyWeighted;

    //! Policy for dividing the band into intervals evenly spaced in wavelength squared
    class WavelengthSquaredSpaced;
    
  protected:

    bool dedisperse;
    bool defaraday;

  };

  class FrequencyIntegrate::EvenlySpaced :
    public Integrate<Integration>::EvenlySpaced
  {
    unsigned get_size (const Integration* sub)
    { return sub->get_nchan(); }
  };

  class FrequencyIntegrate::EvenlyWeighted : 
    public Integrate<Integration>::EvenlyWeighted
  {
    unsigned get_size (const Integration* sub)
    { return sub->get_nchan(); }

    double get_weight (const Integration* sub, unsigned ichan)
    { return sub->get_weight (ichan); }
  };

  class FrequencyIntegrate::WavelengthSquaredSpaced : 
    public Integrate<Integration>::NonlinearlySpaced
  {
    unsigned get_size (const Integration* sub)
    { return sub->get_nchan(); }

    double get_value (const Integration* sub, unsigned ichan)
    { double freq = sub->get_centre_frequency (ichan); return 1.0 / (freq*freq); }
  };
  
}

#endif
