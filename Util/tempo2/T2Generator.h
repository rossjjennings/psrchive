//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Generator.h,v $
   $Revision: 1.3 $
   $Date: 2007/06/21 17:32:46 $
   $Author: straten $ */

#ifndef __Tempo2Generator_h
#define __Tempo2Generator_h

#include "Pulsar/Generator.h"

namespace Tempo2 {

  class Predictor;
  class Parameters;

  //! Generates a Tempo2::Predictor
  class Generator : public Pulsar::Generator {

  public:

    //! Default constructor
    Generator (const Parameters* parameters = 0);

    //! Copy constructor
    Generator (const Generator&);

    //! Destructor
    ~Generator ();

    //! Set the parameters used to generate the predictor
    void set_parameters (const Pulsar::Parameters*);

    //! Set the range of epochs over which to generate
    void set_time_span (const MJD& start, const MJD& finish);

    //! Set the number of time coefficients
    void set_time_ncoeff (unsigned ncoeff);

    //! Set the range of frequencies over which to generate
    void set_frequency_span (long double low, long double high);

    //! Set the number of frequency coefficients
    void set_frequency_ncoeff (unsigned ncoeff);

    //! Set the site at which the signal is observed
    void set_site (const std::string&);

    //! Set the length of each polynomial segment in days
    void set_segment_length (long double days);

    //! Return a new Predictor instance
    Pulsar::Predictor* generate () const;

  private:

    //! The parameters used to generate the predictor
    Reference::To<const Parameters> parameters;

    long double epoch1, epoch2;
    long double freq1, freq2;
    std::string sitename;
    int ntimecoeff, nfreqcoeff;

    //! length of each segment in days
    long double segment_length;

  };

}

#endif
