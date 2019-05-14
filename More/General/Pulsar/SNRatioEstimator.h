//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/SNRatioEstimator.h

#ifndef __Pulsar_SNRatioEstimator_h
#define __Pulsar_SNRatioEstimator_h

#include "Pulsar/Algorithm.h"
#include "TextInterface.h"

namespace Pulsar {

  class Profile;

  //! Algorithms that estimate the signal-to-noise ratio of pulse profiles
  class SNRatioEstimator : public Algorithm
  {

  public:

    //! Return the signal to noise ratio
    virtual float get_snr (const Profile* profile) = 0;

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Return a copy constructed instance of self
    virtual SNRatioEstimator* clone () const = 0;

    //! Construct a new SNRatioEstimator from a string
    static SNRatioEstimator* factory (const std::string& name_and_parse);

  };

  std::ostream& operator<< (std::ostream&, SNRatioEstimator*);

  std::istream& operator>> (std::istream&, SNRatioEstimator*&);

}


#endif // !defined __Pulsar_SNRatioEstimator_h
