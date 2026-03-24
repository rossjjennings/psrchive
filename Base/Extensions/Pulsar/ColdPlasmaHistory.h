//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/ColdPlasmaHistory.h

#ifndef __Pulsar_ColdPlasmaHistory_h
#define __Pulsar_ColdPlasmaHistory_h

#include "Pulsar/IntegrationExtension.h"

namespace Pulsar {

  //! Stores parameters used to correct dispersion and Faraday rotation
  class ColdPlasmaMeasure
  {
  public:

    //! Set the corrected measure
    void set_measure (double val) { measure = val; }
    //! Get the corrected measure
    double get_measure () const { return measure; }

    //! Set true when the measure has been corrected
    void set_corrected (bool flag);
    //! Return true when the measure has been corrected
    bool get_corrected () const { return corrected; }

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres) { reference_wavelength = metres; }
    //! Get the reference wavelength
    double get_reference_wavelength () const { return reference_wavelength; }

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency
    double get_reference_frequency () const;

    //! The type of measurement
    typedef enum { DispersionMeasure, RotationMeasure, Unknown } Measurement;
    void set_measurement (Measurement m) { measurement = m; }

    //! The reference frequency policy
    typedef enum { Relative, Absolute, None } Reference;
    void set_reference (Reference r) { reference = r; }

  protected:

    //! The corrected measure
    double measure = 0.0;

    //! The reference wavelength in metres
    double reference_wavelength = 0.0;

    //! Set true when the measure has been corrected
    bool corrected = false;

    //! The type of measure
    Measurement measurement = Unknown;

    //! The reference frequency policy
    Reference reference = None;
  };

  //! Stores parameters used to correct dispersion and Faraday rotation in each Integration
  class ColdPlasmaHistory : public Pulsar::Integration::Extension
  {
    //! The correction history for the measure corrected with respect to centre frequency
    ColdPlasmaMeasure relative;

    //! The correction history for the measure corrected with respect to infinite frequency
    ColdPlasmaMeasure absolute;

  public:
    
    //! Default constructor
    ColdPlasmaHistory (const char* name);

    //! Copy constructor
    ColdPlasmaHistory (const ColdPlasmaHistory&);

    //! Get the correction history for the measure corrected with respect to centre frequency
    ColdPlasmaMeasure* get_relative() { return &relative; }
    const ColdPlasmaMeasure* get_relative() const { return &relative; }

    //! Get the correction history for the measure corrected with respect to infinite frequency
    ColdPlasmaMeasure* get_absolute() { return &absolute; }
    const ColdPlasmaMeasure* get_absolute() const { return &absolute; }

    //! Set the measurement type
    void set_measurement (ColdPlasmaMeasure::Measurement);
  };
  
}

#endif

