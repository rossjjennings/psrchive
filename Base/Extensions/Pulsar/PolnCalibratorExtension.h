//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/PolnCalibratorExtension.h

#ifndef __PolnCalibratorExtension_h
#define __PolnCalibratorExtension_h

#include "Pulsar/CalibratorExtension.h"

namespace Pulsar {

  class PolnCalibrator;

  //! Stores PolnCalibrator parameters in an Archive instance
  /*! This Archive::Extension implements the storage of PolnCalibrator
    data. */  
  class PolnCalibratorExtension : public CalibratorExtension {
    
  public:
    
    //! Default constructor
    PolnCalibratorExtension ();

    //! Copy constructor
    PolnCalibratorExtension (const PolnCalibratorExtension&);

    //! Operator =
    const PolnCalibratorExtension& operator= (const PolnCalibratorExtension&);

    //! Destructor
    ~PolnCalibratorExtension ();

    //! Clone method
    PolnCalibratorExtension* clone () const
    { return new PolnCalibratorExtension( *this ); }

    //! get the text ineterface 
    TextInterface::Parser* get_interface();

    // Text interface to a PolnCalibratorExtension instance
    class Interface : public TextInterface::To<PolnCalibratorExtension>
    {
      public:
        Interface( PolnCalibratorExtension *s_instance = NULL );
    };

    //! Construct from a PolnCalibrator instance
    PolnCalibratorExtension (const PolnCalibrator*);

    //! Return a short name
    std::string get_short_name () const override { return "pcal"; }

    //! Set the type of the instrumental response parameterization
    void set_type (const Calibrator::Type* type) override;

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan) override;

    //! Remove the inclusive range of channels
    void remove_chan (unsigned first, unsigned last) override;

    //! Set the weight of the specified channel
    void set_weight (unsigned ichan, float weight) override;
    //! Get the weight of the specified channel
    float get_weight (unsigned ichan) const override;

    //! Get the number of parameters describing each transformation
    unsigned get_nparam () const;

    //! Set the number of abscissa / dimensions in the model
    void set_ndim (unsigned);
    //! Get the number of abscissa / dimensions in the model
    unsigned get_ndim () const;

    //! Return true if the covariances of the model parameters are available
    bool get_has_covariance () const;
    //! Set true when the covariances of the model parameters are available
    void set_has_covariance (bool);

    //! Return true if information about the solution is available
    bool get_has_solver () const;
    //! Return true if information about the solution is available
    void set_has_solver (bool);

    //! Return true if the transformation for the specified channel is valid
    bool get_valid (unsigned ichan) const;
    void set_valid (unsigned ichan, bool valid);

    class Transformation;

    //! Get the transformation for the specified frequency channel
    Transformation* get_transformation (unsigned c);
    //! Get the transformation for the specified frequency channel
    const Transformation* get_transformation (unsigned c) const;

    //! Append PolnCalibrator Extension data from another Archive
    void frequency_append (Archive* to, const Archive* from) override;

    void fscrunch (unsigned factor = 0);
    void fscrunch_to_nchan (unsigned new_nchan);

    Estimate<float> get_Estimate ( unsigned iparam, unsigned ichan ) const;
    void set_Estimate (unsigned iparam, unsigned ichan, const Estimate<float>&);

  protected:

    //! The instrumental response as a function of frequency
    std::vector<Transformation> response;

    //! The number of parameters that describe the transformation
    unsigned nparam = 0;

    //! The number of abscissa / dimensions in the model
    unsigned ndim = 0;

    //! The covariances of the transformation parameters are available
    bool has_covariance = false;

    //! The solver statistics are available
    bool has_solver = false;

    //! Construct the response array according to the current attributes
    void construct ();

  private:
    
    void init ();

  };
 
  //! Intermediate storage of MEAL::Complex parameters 
  class PolnCalibratorExtension::Transformation : public Reference::Able
  {
  public:

    //! Default constructor
    Transformation ();

    //! Get the number of model parameters
    unsigned get_nparam() const;
    //! Set the number of model parameters
    void set_nparam (unsigned);

    //! Get the name of the specified model parameter
    std::string get_param_name (unsigned) const;
    //! Set the name of the specified model parameter
    void set_param_name (unsigned, const std::string&);

    //! Get the description of the specified model parameter
    std::string get_param_description (unsigned) const;
    //! Set the description of the specified model parameter
    void set_param_description (unsigned, const std::string&);

    //! Get the value of the specified model parameter
    double get_param (unsigned) const;
    //! Set the value of the specified model parameter
    void set_param (unsigned, double);

    //! Get the variance of the specified model parameter
    double get_variance (unsigned) const;
    //! Set the variance of the specified model parameter
    void set_variance (unsigned, double);

    //! Get the covariance matrix of the model paramters
    std::vector< std::vector<double> > get_covariance () const;
    //! Set the covariance matrix of the model paramters
    void set_covariance (const std::vector< std::vector<double> >&);

    //! Get the covariance matrix efficiently
    void get_covariance (std::vector<double>&) const;
    //! Set the covariance matrix efficiently
    void set_covariance (const std::vector<double>&);

    //! Get the value and variance of the specified model parameter
    Estimate<double> get_Estimate (unsigned) const;
    //! Set the value and variance of the specified model parameter
    void set_Estimate (unsigned, const Estimate<double>&);

    //! Get the model validity flag
    bool get_valid () const;
    //! Set the model validity flag
    void set_valid (bool);

    //! Get the best fit value of chi squared
    double get_chisq () const;
    //! Set the best fit value of chi squared
    void set_chisq (double);

    //! Get the logarithm of the determinant of the curvature matrix
    double get_log_det_curvature () const;
    //! Set the logarithm of the determinant of the curvature matrix
    void set_log_det_curvature (double);

    //! Get the logarithm of the condition number of the curvature matrix
    double get_log_cond_curvature () const;
    //! Set the logarithm of the condition number of the curvature matrix
    void set_log_cond_curvature (double);

    //! Get the number of degrees of freedom
    unsigned get_nfree() const;
    //! Set the number of degress of freedom
    void set_nfree (unsigned);

    //! Get the number of model parameters varied to find best fit
    unsigned get_nfit() const;
    //! Set the number of model parameters varied to find best fit
    void set_nfit (unsigned);

    //! Get the number of abscissa / dimensions
    unsigned get_ndim() const;
    //! Set the number of abscissa / dimensions
    void set_ndim (unsigned);

    //! Get the best fit value of the reduced chi squared = chisq/nfree
    double get_reduced_chisq () const;

    //! Get the Akaike information criterion
    double get_Akaike_information_criterion() const;

    //! Get the Bayesian information criterion
    double get_Bayesian_information_criterion() const;

    //! Get the geometric information criterion
    double get_geometric_information_criterion() const;

    //! Get the stochastic information complexity
    double get_stochastic_information_complexity() const;

    // Text interface to a PolnCalibratorExtension instance
    class Interface : public TextInterface::To<Transformation>
    {
      public:
        Interface();
    };

  protected:

    std::vector< Estimate<double> > params;
    std::vector< std::string > names;
    std::vector< std::string > descriptions;

    std::vector<double> covariance;
    double chisq = 0.0;
    double log_det_curvature = 0.0;
    double log_cond_curvature = 0.0;
    unsigned nfree = 0;
    unsigned nfit = 0;

    /*! Currently (29 Dec 2024), ndim is not stored with the model results;
        rather, it is derived from the ConfigurableProjection */
    unsigned ndim = 0;
    bool valid = false;

  };

}

#endif
