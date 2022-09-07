//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/CalibrationInterpolatorExtension.h

#ifndef __CalibrationInterpolatorExtension_h
#define __CalibrationInterpolatorExtension_h

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/CalibratorType.h"
#include "Pulsar/CalibratorStokes.h"

namespace Pulsar {

  //! Stores Polarization Calibration Model Interpolator information
  /*! This Archive::Extension class implements the storage of
    Calibration Interpolator data, enabling such a calibrator to be
    constructed from file. */
  class CalibrationInterpolatorExtension : public Archive::Extension
  {   
  public:
    
    //! Default constructor
    CalibrationInterpolatorExtension
    (const char* name = "CalibrationInterpolatorExtension");

    //! Copy constructor
    CalibrationInterpolatorExtension (const CalibrationInterpolatorExtension&);

    //! Operator =
    const CalibrationInterpolatorExtension&
    operator= (const CalibrationInterpolatorExtension&);

    //! Destructor
    ~CalibrationInterpolatorExtension ();

    //! Clone method
    CalibrationInterpolatorExtension* clone () const
    { return new CalibrationInterpolatorExtension( *this ); }

    //! Returns true if the Extension contains useful data
    bool has_data () const;

    //! Set the type of the calibrator
    void set_type (const Calibrator::Type* t);// { type = t; }
    //! Get the type of the calibrator
    const Calibrator::Type* get_type () const;// { return type; }

    //! Set the point where the reference source signal is coupled
    void set_coupling_point (CalibratorStokes::CouplingPoint point)
    { coupling_point = point; }
    
    //! The point where the reference source signal is coupled
    CalibratorStokes::CouplingPoint get_coupling_point () const
    { return coupling_point; }

    //! Set the number of receptors in flux calibrator
    void set_nreceptor (unsigned n) { nreceptor = n; }
    //! Get the number of receptors in flux calibrator
    unsigned get_nreceptor () const { return nreceptor; }

    //! Returns true if scale is native, false if scale is relative
    bool get_native_scale () const { return native_scale; }
    void set_native_scale (bool flag) { native_scale = flag; }

    //! Get the number of epochs in input data
    unsigned get_nsub_input () const { return nsub_input; }
    void set_nsub_input (unsigned n) { nsub_input = n; }

    //! Set the reference epoch of the interpolator
    void set_reference_epoch (const MJD& epoch) { reference_epoch = epoch; }
    //! Get the reference epoch of the interpolator
    MJD get_reference_epoch () const { return reference_epoch; }

    //! Set the minimum epoch of the interpolator
    void set_minimum_epoch (const MJD& epoch) { minimum_epoch = epoch; }
    //! Get the minimum epoch of the interpolator
    MJD get_minimum_epoch () const { return minimum_epoch; }

    //! Set the maximum epoch of the interpolator
    void set_maximum_epoch (const MJD& epoch) { maximum_epoch = epoch; }
    //! Get the maximum epoch of the interpolator
    MJD get_maximum_epoch () const { return maximum_epoch; }

    //! Get the number of frequency channels in input data
    unsigned get_nchan_input () const { return nchan_input; }
    void set_nchan_input (unsigned n) { nchan_input = n; }

    //! Set the reference frequency of the interpolator
    void set_reference_frequency (double freq) { reference_frequency = freq; }
    //! Get the reference frequency of the interpolator
    double get_reference_frequency () const { return reference_frequency; }

    //! Set the minimum frequency of the interpolator
    void set_minimum_frequency (double freq) { minimum_frequency = freq; }
    //! Get the minimum frequency of the interpolator
    double get_minimum_frequency () const { return minimum_frequency; }

    //! Set the maximum frequency of the interpolator
    void set_maximum_frequency (double freq) { maximum_frequency = freq; }
    //! Get the maximum frequency of the interpolator
    double get_maximum_frequency () const { return maximum_frequency; }

    //! Return a short name
    std::string get_short_name () const { return "pcmint"; }

    //! Text interface to CalibrationInterpolatorExtension extension
    class Interface
      : public TextInterface::To<CalibrationInterpolatorExtension>
    {
    public:
      Interface (CalibrationInterpolatorExtension* = 0);
    };
    
    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    class Parameter : public Reference::Able
    {
    public:

      //! Text interface to CalibrationInterpolatorExtension::Parameter
      class Interface
	: public TextInterface::To<Parameter>
      {
      public:
	Interface (Parameter* = 0);
      };

      //! The type of model parameter
      enum Type
      {
        FrontendParameter = 1,
        CalibratorStokesParameter = 2,
	FluxCalibratorParameter = 3
      };
      
      Type code;
      Type get_code () const { return code; }

      unsigned iparam;
      unsigned get_iparam () const { return iparam; }

      float log10_smoothing_factor;
      float get_log10_smoothing_factor () const { return log10_smoothing_factor; }
      float total_chi_squared;
      float get_total_chi_squared () const { return total_chi_squared; }
      
      unsigned ndat_input;
      unsigned get_ndat_input () const { return ndat_input; }
      
      unsigned ndat_flagged_before;
      unsigned get_ndat_flagged_before () const { return ndat_flagged_before; }
      
      unsigned ndat_flagged_after;
      unsigned get_ndat_flagged_after () const { return ndat_flagged_after; }
      
      std::string interpolator;
    };

    void add_parameter (Parameter* p) { parameter.push_back(p); }
    unsigned get_nparam () const { return parameter.size(); }
    
    Parameter* get_parameter (unsigned iparam)
    { return parameter.at(iparam); }
    const Parameter* get_parameter (unsigned iparam) const
    { return parameter.at(iparam); }
    
    protected:

    //! Type of the calibrator
    Reference::To<const Calibrator::Type> type;

    //! The point where the reference source signal is coupled
    CalibratorStokes::CouplingPoint coupling_point;

    //! Number of receptors in flux calibrator
    unsigned nreceptor;

    //! True if scale is native; false if scale is relative to reference
    bool native_scale;
    
    //! The number of epochs in the input data
    unsigned nsub_input;
    
    //! The reference epoch
    MJD reference_epoch;
    //! The minimum epoch
    MJD minimum_epoch;
    //! The maximum epoch
    MJD maximum_epoch;

    //! The number of frequency channels in the input data
    unsigned nchan_input;
    
    //! The reference frequency in MHz
    double reference_frequency;
    //! The minimum frequency in MHz
    double minimum_frequency;
    //! The maximum frequency in MHz
    double maximum_frequency;

    std::vector< Reference::To<Parameter> > parameter;
  };
 
  std::ostream& operator << (std::ostream& ostr,
			     CalibrationInterpolatorExtension::Parameter::Type);

}

#endif
