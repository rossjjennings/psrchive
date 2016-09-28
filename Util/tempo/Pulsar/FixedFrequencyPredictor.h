//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2015 by Matthew Kerr
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FixedFrequencyPredictor_h
#define __FixedFrequencyPredictor_h

#include "Pulsar/Predictor.h"

namespace Pulsar {

  //! Functions that predict pulse phase
  /*! This simple version assumes folding at a fixed topocentric 
      frequency, as e.g. a pulsed cal. */
  class FixedFrequencyPredictor : public Predictor {

  public:

    //! Set frequency later
    FixedFrequencyPredictor ();

    //! Construct from fixed frequency
    FixedFrequencyPredictor (long double Hz);

    //! Set the frequency at which data are folded (Hz)
    void set_topocentric_frequency (long double Hz);

    //! Get the topocentric frequency at which data are folded (Hz)
    long double get_topocentric_frequency () const;

    //! Set the epoch
    void set_epoch (const MJD& t);

    //! Get the epoch
    const MJD& get_epoch () const;

    //! Set the observing frequency at which phase and time are related
    virtual void set_observing_frequency (long double MHz);

    //! Get the observing frequency at which phase and epoch are related
    virtual long double get_observing_frequency () const;

    //! Return the phase, given the epoch
    virtual Phase phase (const MJD& t) const;

    //! Return the epoch, given the phase and, optionally, a first guess
    virtual MJD iphase (const Phase& phase, const MJD* guess = 0) const;

    //! Return the spin frequency, given the epoch
    virtual long double frequency (const MJD& t) const;

    //! Return the phase correction for dispersion delay
    virtual Phase dispersion (const MJD &t, long double MHz) const;

    // "implement" pure virtual functions we do not need
    virtual Predictor* clone() const {return 0;}
    virtual Generator* generator() const {return 0;}
    virtual void insert (const Predictor*) {}
    virtual void keep (const std::vector<MJD>&) {}
    virtual bool matches (const Predictor*) const {return 0;}
    virtual void load (FILE*) {}
    virtual void unload (FILE*) const {}


  protected:

    //! The fixed topocentric frequency
    long double m_freq;

    //! The observing frequency -- irrelevant.
    long double m_obs_freq;

    //! epoch (basically used for reference phase)
    MJD m_epoch;

  };

}

#endif
