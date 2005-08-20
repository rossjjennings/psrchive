//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineWindow.h,v $
   $Revision: 1.3 $
   $Date: 2005/08/20 14:29:09 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineWindow_h
#define __Pulsar_BaselineWindow_h

#include "Pulsar/BaselineEstimator.h"

namespace Pulsar {

  class Profile;

  //! A simple tool for independently weighting pulse phase bins
  /*! The Pulsar::BaselineWindow class implements a useful set of methods
    required to create, store, and manipulate a weighted set of
    Pulsar::Profile phase bins. */
  class BaselineWindow : public BaselineEstimator {

  public:

    //! Default constructor
    BaselineWindow ();

    //! Set the Profile from which the baseline PhaseWeight will be derived
    void set_Profile (const Profile* profile);

    //! Retrieve the PhaseWeight
    void get_weight (PhaseWeight& weight);

    //! Set the duty cycle
    void set_duty_cycle (float duty_cycle);

    //! Get the duty cycle
    float get_duty_cycle () const;

    //! Set to find the minimum mean
    void set_find_minimum ();
  
    //! Set to find the maximum mean
    void set_find_maximum ();

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

    //! Get the phase at the centre of the window
    void get_phase () const;

    //! Return the phase at which minimum or maximum mean is found
    float find_phase (unsigned nbin, const float* amps);

  protected:

    //! The width of the window over which the mean is computed
    float duty_cycle;

    //! Set true when algorithm finds max
    bool find_max;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

    //! The Profile from which the mask will be calculated
    Reference::To<const Profile> profile;

  };

}


#endif // !defined __Pulsar_BaselineWindow_h
