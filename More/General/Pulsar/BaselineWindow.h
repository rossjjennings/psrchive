//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineWindow.h,v $
   $Revision: 1.7 $
   $Date: 2006/04/06 22:07:00 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineWindow_h
#define __Pulsar_BaselineWindow_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  class Profile;

  //! Finds the phase window in which the Profile mean is an extremum
  class BaselineWindow : public BaselineEstimator {

  public:

    //! Default constructor
    BaselineWindow ();

    //! Set the duty cycle
    void set_duty_cycle (float duty_cycle);

    //! Get the duty cycle
    float get_duty_cycle () const;

    //! Set to find the minimum mean
    void set_find_minimum ();
  
    //! Set to find the maximum mean
    void set_find_maximum ();

    //! Set to find the mean closest to the specified value
    void set_find_mean (float mean);

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

    //! Get the phase at the centre of the window
    void get_phase () const;

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (const std::vector<float>& amps);

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (unsigned nbin, const float* amps);

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight& weight);

    //! The width of the window over which the mean is computed
    float duty_cycle;

    //! Set true when algorithm finds max
    bool find_max;

    //! Set true when algorithm finds mean
    bool find_mean;

    //! Mean value to be found
    float mean;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

  };

}


#endif // !defined __Pulsar_BaselineWindow_h
