//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Faraday.h,v $
   $Revision: 1.2 $
   $Date: 2005/04/06 20:40:21 $
   $Author: straten $ */

#ifndef __Faraday_H
#define __Faraday_H

#include "MEAL/Rotation.h"

namespace Calibration {

  //! Faraday rotation transformation, parameterized by the rotation measure
  class Faraday : public MEAL::Complex2 {

  public:

    //! Default constructor
    Faraday ();

    //! Return the name of the class
    std::string get_name () const;

    //! Set the rotation measure
    void set_rotation_measure (const Estimate<double>& rotation_measure);
    //! Get the rotation measure
    Estimate<double> get_rotation_measure () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the frequency in MHz
    void set_frequency (double MHz);
    //! Get the frequency in MHz
    double get_frequency () const;

    //! Set the axis about which the rotation occurs
    void set_axis (const Vector<double, 3>& axis);

    //! Get the Faraday rotation angle
    double get_rotation () const;

  protected:

    //! The Rotation operation
    MEAL::Rotation rotation;

    //! Reference frequency in MHz
    double reference_frequency;

    //! Frequency in MHz
    double frequency;

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
