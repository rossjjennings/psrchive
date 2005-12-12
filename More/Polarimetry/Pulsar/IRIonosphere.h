//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/IRIonosphere.h,v $
   $Revision: 1.1 $
   $Date: 2005/12/12 13:41:06 $
   $Author: straten $ */

#ifndef __Calibration_IRIonosphere_H
#define __Calibration_IRIonosphere_H

#include "Calibration/Faraday.h"

namespace Calibration {

  //! International Reference Ionosphere (IRI) transformation
  /*! This class computes the Faraday rotation due to ionospheric
    electrons. */
  class IRIonosphere : public MEAL::Complex2 {

  public:

    //! Default constructor
    IRIonosphere ();

    //! Return the name of the class
    std::string get_name () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the frequency in MHz
    void set_frequency (double MHz);
    //! Get the frequency in MHz
    double get_frequency () const;

    //! Get the Faraday rotation angle
    double get_rotation () const;

  protected:

    //! The Faraday rotation transformation
    Calibration::Faraday faraday;

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedComplex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >* grad);

  };

}

#endif
