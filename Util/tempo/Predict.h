//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Predict.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

#ifndef __Tempo_Predict_h
#define __Tempo_Predict_h

#include "psrephem.h"
#include "polyco.h"

namespace Tempo {

  //! Uses tempo orediction mode to produce polynomial ephemerides (polyco)
  class Predict  {

  public:

    //! Default constructor
    Predict ();

    //! Set the observatory site code
    /*! Correponds to ASITE in tz.in */
    void set_asite (char code);

    //! Set the observing frequency
    /*! Corresponds to FREQ in tz.in */
    void set_frequency (double MHz);

    //! Set the maximum hour angle of observations
    /*! Corresponds to MAXHA in tz.in */
    void set_maxha (unsigned hours);

    //! Set the time spanned by each polynomial ephemeris
    /*! Corresponds to NSPAN in tz.in */
    void set_nspan (unsigned minutes);

    //! Set the number of coefficients per polynomial ephemeris
    /*! Corresponds to NCOEFF in tz.in */
    void set_ncoef (unsigned ncoef);

    //! Set the pulsar parameters
    void set_parameters (const psrephem& parameters);

    //! Set flag to verify time spanned by polynomial ephemerides
    void set_verify (bool verify);

    //! Returns a polyco valid over the range in MJD specified by m1 and m2
    polyco get_polyco (const MJD& m1, const MJD& m2) const;

    //! Write the tz.in file according to the tempo definition
    void write_tzin () const;

  protected:

    //! The observatory site code
    char asite;

    //! The observing frequency in MHz
    double frequency;

    //! The maximum hour angle of observations in hours
    unsigned maxha;

    //! The time spanned by each polynomial ephemeris in minutes
    unsigned nspan;

    //! The number of coefficients per polynomial ephemeris
    unsigned ncoef;

    //! The pulsar parameters
    psrephem parameters;

    //! The pulsar name
    std::string psrname;

    //! Verify the time spanned by polynomial ephemerides
    bool verify;

  private:

    //! Cached polynomial
    Reference::To<polyco> cached;

  };

}

#endif
