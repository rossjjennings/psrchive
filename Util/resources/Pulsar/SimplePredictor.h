//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/resources/Pulsar/SimplePredictor.h

#ifndef __SimplePredictor_h
#define __SimplePredictor_h

#include "Pulsar/Predictor.h"
#include "sky_coord.h"

#include <string>
#include <vector>


namespace Pulsar {

  //! Simple pulse phase model
  class SimplePredictor : public Predictor {

  public:

    //! The precision of the prediction in seconds
    static double precision;

    //! Default constructor
    SimplePredictor (const char* filename = 0);

    //! Destructor
    ~SimplePredictor ();

    //! Return a new, copy constructed instance of self
    Predictor* clone () const;

    //! Set up Generator to produce a new Predictor like self
    Generator* generator () const;

    //! Add the information from the supplied predictor to self
    void insert (const Pulsar::Predictor*);

    //! Keep only the components required to span the given epochs
    void keep (const std::vector<MJD>& epochs);

    //! Return true if reference attributes are equal
    bool matches (const Pulsar::Predictor*) const;

    //! Set the observing frequency at which predictions will be made
    void set_observing_frequency (long double MHz);

    //! Get the observing frequency at which phase and epoch are related
    long double get_observing_frequency () const;

    //! Return the phase, given the epoch
    Pulsar::Phase phase (const MJD& t) const;

    //! Return the epoch, given the phase
    MJD iphase (const Pulsar::Phase& phase, const MJD* guess) const;

    //! Return the spin frequency, given the epoch
    long double frequency (const MJD& t) const;

    //! Return the phase correction for dispersion delay
    Pulsar::Phase dispersion (const MJD &t, long double MHz) const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

    //! Source name
    std::string get_name () const { return name; }

    //! Coordinates
    sky_coord get_coordinates () const { return coordinates; }

    //! Dispersion measure
    double get_dispersion_measure () const { return dispersion_measure; }

    //! Set the reference epoch
    void set_reference_epoch (const MJD& epoch) { reference_epoch = epoch; }

    //! Get the reference epoch
    MJD get_reference_epoch () const { return reference_epoch; }

    //! alias for reference epoch used by Pulsar::inverse_phase
    MJD    get_reftime       () const { return reference_epoch; }
    //! reference phase is always zero; used by Pulsar::inverse_phase
    Pulsar::Phase  get_refphase      () const { return 0.0; }
    //! spin frequency at reference epoch; used by Pulsar::inverse phase
    double get_reffrequency  () const { return coefs.at(0); }
    
  private:

    //! Polynomial coefficients
    std::vector<long double> coefs;

    //! Reference epoch
    MJD reference_epoch;

    //! Source name
    std::string name;

    //! Coordinates
    sky_coord coordinates;

    //! Dispersion measure
    double dispersion_measure;

    //! Telescope name
    std::string telescope;

    void parse(std::string key, std::string value);

  };

}

#endif
