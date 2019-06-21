//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/BaselineWindow.h

#ifndef __Pulsar_BaselineWindow_h
#define __Pulsar_BaselineWindow_h

#include "Pulsar/BaselineEstimator.h"
#include "PhaseRange.h"

namespace Pulsar {

  class Profile;
  class Smooth;

  //! Finds the phase window in which the smoothed Profile is an extremum
  class BaselineWindow : public BaselineEstimator
  {

  public:

    //! Default constructor
    BaselineWindow ();

    //! Copy constructor
    BaselineWindow (const BaselineWindow&);

    //! Set the smoothing function
    void set_smooth (Smooth*);

    //! Get the smoothing function
    Smooth* get_smooth () const;

    //! Set to find the minimum mean
    void set_find_minimum (bool f = true);
    bool get_find_minimum () const;

    //! Set to find the maximum mean
    void set_find_maximum (bool f = true);
    bool get_find_maximum () const;

    //! Set to find the mean closest to the specified value
    void set_find_mean (float mean);
    float get_find_mean () const;

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

    //! Set the range to be searched
    void set_search_range (const Phase::Range&);

    //! Get the range to be search
    Phase::Range get_search_range () const;

    //! Get the range found during execution
    Phase::Range get_found_range () const;

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (const std::vector<float>& amps);

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (unsigned nbin, const float* amps);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    BaselineWindow* clone () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

    //! The smoothing function
    Reference::To<Smooth> smooth;

    //! Set true when algorithm finds max
    bool find_max;

    //! Set true when algorithm finds mean
    bool find_mean;

    //! Mean value to be found
    float mean;

    //! The first bin in the selected range
    Phase::Range search_range;
    Phase::Range found_range;

    //! Set true when range is specified
    bool range_specified;
  };

}


#endif // !defined __Pulsar_BaselineWindow_h
