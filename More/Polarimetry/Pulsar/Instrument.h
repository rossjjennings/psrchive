//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/Instrument.h

#ifndef __CalibrationInstrument_H
#define __CalibrationInstrument_H

#include "Pulsar/BackendFeed.h"

namespace Calibration {

  class Feed;

  //! Phenomenological description of the instrument
  /*! The transformation is represented by the product of a SingleAxis
    transformation and Feed transformation. */
  class Instrument : public BackendFeed {

  public:

    //! Default Constructor
    Instrument ();

    //! Copy Constructor
    Instrument (const Instrument& s);

    //! Assignment Operator
    const Instrument& operator = (const Instrument& s);

    //! Clone operator
    Instrument* clone () const;

    //! Destructor
    ~Instrument ();

    //! Model the ellipticities of both receptors using the same value
    void equal_ellipticities ();

    //! Model the ellipticities of the receptors using independent values
    void independent_ellipticities ();

    //! Return the single model parameter that constrains both ellipticities
    MEAL::ScalarParameter* get_ellipticities () { return ellipticities; }

    //! Return true if both ellipticities are constrained by a single parameter
    bool has_equal_ellipticities () const { return ellipticities; }

    //! Model the orientations of both receptors using the same value
    void equal_orientations ();
    void independent_orientations ();

    //! Set cyclical limits on the model parameters
    void set_cyclic (bool flag = true);

    //! Fix the orientation of the frontend
    void set_constant_orientation (bool);
    
    const MEAL::Complex2* get_frontend () const;

    //! Get the orientation, \f$\theta\f$, of the specified receptor in radians
    Estimate<double> get_orientation (unsigned ireceptor) const;

    //! Get the ellipticity, \f$\chi\f$, of the specified receptor in radians
    Estimate<double> get_ellipticity (unsigned ireceptor) const;

    //! Get the index of the ellipticity parameter for the specified receptor
    unsigned get_ellipticity_index (unsigned receptor) const;

    //! Set the orientation, \f$\theta\f$, of the specified receptor in radians
    void set_orientation (unsigned ireceptor, const Estimate<double>& theta);

    //! Set the ellipticity, \f$\chi\f$, of the specified receptor in radians
    void set_ellipticity (unsigned ireceptor, const Estimate<double>& chi);

    //! Provide access to the Feed model
    const Feed* get_feed () const;
    Feed* get_feed ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Feed model: \f$\theta_0-1\f$ and \f$\chi_0-1\f$
    Reference::To<Feed> feed;

    //! ChainRule used to bind Feed parameters
    Reference::To< MEAL::ChainRule<MEAL::Complex2> > feed_chain;

    //! ScalarParameter used to bind ellipticities
    Reference::To<MEAL::ScalarParameter> ellipticities;

    //! ScalarParameter used to bind orientations
    Reference::To<MEAL::ScalarParameter> orientations;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

