//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/ArrivalTime.h,v $
   $Revision: 1.2 $
   $Date: 2009/09/30 08:40:06 $
   $Author: straten $ */

#ifndef __Pulsar_ArrivalTime_h
#define __Pulsar_ArrivalTime_h

#include "Pulsar/Algorithm.h"
#include "toa.h"

namespace Pulsar {

  class ProfileShiftEstimator;
  class ShiftEstimator;
  class Archive;
  class Integration;
  class Profile;

  //! Manages arrival time estimation
  /*! 
    This class manages the observation, template, shift estimator,
    output format, etc. thereby replacing the old Archive::toas method
    as well as parts of code from pat.
  */

  class ArrivalTime : public Algorithm {

  public:

    //! Default constructor
    ArrivalTime ();

    //! Destructor
    ~ArrivalTime ();

    //! Set the observation from which the arrival times will be derived
    void set_observation (const Archive*);

    //! Set the standard/template used by some phase shift estimators
    void set_standard (const Archive*);

    //! Set the algorithm used to estimate the phase shift
    void set_shift_estimator (ShiftEstimator*);

    //! Set the format of the output time-of-arrival estimates
    void set_format (Tempo::toa::Format);

    //! Set the format as a string
    void set_format (const std::string&);

    //! Set additional format flags
    void set_format_flags (const std::string&);

    //! Skip data with zero weight
    void set_skip_bad (bool flag) { skip_bad = flag; }

    //! Add to the vector of time-of-arrival estimates
    void get_toas (std::vector<Tempo::toa>&);

  protected:

    Reference::To<const Archive> observation;
    Reference::To<const Archive> standard;
    Reference::To<ShiftEstimator> shift_estimator;

    //! the output TOA format
    Tempo::toa::Format format;

    //! additional format-specific flags
    std::string format_flags;

    //! for formats that support it, a list of attributes to add
    std::vector<std::string> attributes;

    //! when true, skip data with zero weight
    bool skip_bad;

    /*
      Perhaps Tempo::toa::Format should be a base class, such that
      each child of the class does its own formatting
    */
    std::string get_tempo2_aux_txt ();

    void get_toas (const Integration* subint,
		   std::vector<Tempo::toa>& toas);

    Tempo::toa get_toa (const Profile* profile,
			const MJD& mjd, double period);

  private:

    Reference::To<ProfileShiftEstimator> profile_shift;
  };

}

#endif
