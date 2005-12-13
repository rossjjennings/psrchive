//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/OnPulseThreshold.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/13 07:02:21 $
   $Author: straten $ */

#ifndef __Pulsar_OnPulseThreshold_h
#define __Pulsar_OnPulseThreshold_h

#include "Pulsar/OnPulseEstimator.h"

namespace Pulsar {

  class BaselineEstimator;

  //! Uses the baseline statistics to find on-pulse phase bins
  /*! The on-pulse phase bins are those with a value greater than the
    threshold times the rms of the off-pulse phase bins.  Therefore,
    this class makes use of a BaselineEstimator to find the off-pulse
    baseline.  By default, this is the BaselineWindow class. */

  class OnPulseThreshold : public OnPulseEstimator {

  public:

    //! Default constructor
    OnPulseThreshold ();

    //! Set the Profile from which the on-pulse PhaseWeight will be derived
    void set_Profile (const Profile* profile);

    //! Set the threshold
    void set_threshold (float threshold);

    //! Get the threshold
    float get_threshold () const;

    //! Set the BaselineEstimator used to find the off-pulse phase bins
    void set_baseline_estimator (BaselineEstimator*);

    //! Get the BaselineEstimator used to find the off-pulse phase bins
    const BaselineEstimator* get_baseline_estimator () const;
    BaselineEstimator* get_baseline_estimator ();

    //! Set the start and end bins of the search
    void set_range (int bin_start, int bin_end);

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight& weight);

    //! The threshold
    float threshold;

    //! The first bin in the selected range
    int bin_start;

    //! The last bin in the selected range
    int bin_end;

    //! Set true when range is specified
    bool range_specified;

    //! The BaselineEstimator used to find the off-pulse phase bins
    Reference::To<BaselineEstimator> baseline_estimator;

  };

}


#endif // !defined __Pulsar_OnPulseThreshold_h
