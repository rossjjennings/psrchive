//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/FrequencyIntegrate.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/17 13:34:50 $
   $Author: straten $ */

#ifndef __Pulsar_FrequencyIntegrate_h
#define __Pulsar_FrequencyIntegrate_h

#include "Pulsar/Transformation.h"
#include "Reference.h"
#include <vector>

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
  class FrequencyIntegrate : public Transformation<Integration> {

  public:

    //! Default constructor
    FrequencyIntegrate ();

    //! Set the number of output frequency channels
    void set_nchan (unsigned nchan);

    //! Get the number of output frequency channels
    unsigned get_nchan () const;

    //! Set the number of channels to integrate
    void set_nscrunch (unsigned nscrunch);

    //! Get the number of channels to integrate
    unsigned get_nscrunch () const;

    //! The frequency integration operation
    void transform (Integration*);

    //! Policy for dividing frequency channels into ranges
    class RangePolicy;

    //! Policy for producing evenly spaced frequency channel ranges
    class EvenlySpaced;

    //! Policy for producing evenly distributed frequency channel ranges
    class EvenlyDistributed;

    //! Set the frequency channel range division policy
    void set_range_policy (RangePolicy*);

    //! Compute a roughly even division of nchan into ranges
    void divide (unsigned nchan, unsigned& nrange, unsigned& spacing) const;

  protected:

    //! The number of output frequency channels
    unsigned nchan;

    //! The number of frequency channels to integrate
    unsigned nscrunch;

    //! The frequency channel range selection policy
    Reference::To<RangePolicy> range_policy;

  };


  //! Policies for dividing frequency channels into ranges
  /*! This pure virtual base class defines the interface to the various
    frequency channel range selection policies. */
  class FrequencyIntegrate::RangePolicy : public Reference::Able {

  public:

    //! Initialize ranges for the specified parameters
    virtual void initialize (FrequencyIntegrate*, Integration*) = 0;
    
    //! Return the number of output frequency channel ranges
    virtual unsigned get_nrange () = 0;
    
    //! Return the frequency channels indeces for the specified range
    virtual void get_range (unsigned irange, 
			    unsigned& ichan_start,
			    unsigned& ichan_stop) = 0;

  };

  //! Policy for producing evenly spaced frequency channel ranges
  /*! Evenly spaced frequency channel ranges have a constant number
    of input frequency channels per output frequency channel. */
  class FrequencyIntegrate::EvenlySpaced : public RangePolicy {

  public:

    //! Initialize ranges for the specified parameters
    void initialize (FrequencyIntegrate* freqint, Integration* integration);

    //! Return the number of output frequency channel ranges
    unsigned get_nrange () { return nrange; }

     //! Return the frequency channels indeces for the specified range
   void get_range (unsigned irange, unsigned& start, unsigned& stop);

  protected:

    //! The number of frequency channels in the input Integration
    unsigned subint_nchan;
    //! The number of frequency ranges (channels) in the output Integration
    unsigned nrange;
    //! The number of input channels in each output range
    unsigned spacing;

  };


  //! Policy for producing evenly distributed frequency channel ranges
  /*! Evenly distributed frequency channel ranges have the same number of
    valid input frequency channels integrated into each output frequency
    channel. */
  class FrequencyIntegrate::EvenlyDistributed : public RangePolicy {

  public:

    void initialize (FrequencyIntegrate* freqint, Integration* integration);

    unsigned get_nrange () { return stop_indeces.size(); }

    void get_range (unsigned irange, unsigned& start, unsigned& stop);

  protected:

    //! The input frequency channel index at the end of each range
    std::vector<unsigned> stop_indeces;

  };

}

#endif
