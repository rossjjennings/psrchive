//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/ConfigurableProjectionExtension.h

#ifndef __ConfigurableProjectionExtension_h
#define __ConfigurableProjectionExtension_h

#include "Pulsar/HasChannels.h"

namespace Pulsar {

  class ConfigurableProjection;

  //! Stores ConfigurableProjection parameters in an Archive instance
  /*! This Archive::Extension implements the storage of ConfigurableProjection data. */

  class ConfigurableProjectionExtension : public HasChannels
  {
    
  public:
    
    //! Default constructor
    ConfigurableProjectionExtension ();

    //! Copy constructor
    ConfigurableProjectionExtension (const ConfigurableProjectionExtension&);

    //! Operator =
    const ConfigurableProjectionExtension& operator= (const ConfigurableProjectionExtension&);

    //! Destructor
    ~ConfigurableProjectionExtension ();

    //! Clone method
    ConfigurableProjectionExtension* clone () const
    { return new ConfigurableProjectionExtension( *this ); }

    //! get the text ineterface 
    TextInterface::Parser* get_interface();

    // Text interface to a ConfigurableProjectionExtension instance
    class Interface : public TextInterface::To<ConfigurableProjectionExtension>
    {
      public:
	      Interface( ConfigurableProjectionExtension *s_instance = NULL );
    };

    //! Construct from a ConfigurableProjection instance
    ConfigurableProjectionExtension (const ConfigurableProjection*);

    //! Return a short name
    std::string get_short_name () const { return "proj"; }

    //! Set the string that configures this projection
    void set_configuration (const std::string& text) { configuration = text; }
    //! Get the string that configures this projection
    const std::string& get_configuration () const { return configuration; }

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Remove the inclusive range of channels
    void remove_chan (unsigned first, unsigned last);

    //! Set the weight of the specified channel
    void set_weight (unsigned ichan, float weight);
    //! Get the weight of the specified channel
    float get_weight (unsigned ichan) const;

    //! Get the number of parameters describing each transformation
    unsigned get_nparam () const;
    //! Set the number of parameters describing each transformation
    void set_nparam (unsigned);

    //! Get if the covariances of the transformation parameters
    bool get_has_covariance () const;
    //! Set if the covariances of the transformation parameters
    void set_has_covariance (bool);

    //! Return true if the transformation for the specified channel is valid
    bool get_valid (unsigned ichan) const;
    void set_valid (unsigned ichan, bool valid);

    class Transformation;

    //! Get the transformation for the specified frequency channel
    Transformation* get_transformation (unsigned c);
    //! Get the transformation for the specified frequency channel
    const Transformation* get_transformation (unsigned c) const;

    //! Append ConfigurableProjection Extension data from another Archive
    void frequency_append (Archive* to, const Archive* from);

    void fscrunch (unsigned factor = 0);
    void fscrunch_to_nchan (unsigned new_nchan);

    Estimate<float> get_Estimate ( unsigned iparam, unsigned ichan ) const;
    void set_Estimate (unsigned iparam, unsigned ichan, const Estimate<float>&);

  protected:

    //! Configures this projection
    std::string configuration;

    //! The instrumental response as a function of frequency
    std::vector<Transformation> response;

    //! The number of parameters that describe the transformation
    unsigned nparam;

    //! The covariances of the transformation parameters are available
    bool has_covariance;

    //! Construct the response array according to the current attributes
    void construct ();

    void range_check (unsigned ichan, const char* method) const;

  private:
    
    void init ();

  };
 
  //! Intermediate storage of MEAL::Complex parameters 
  class ConfigurableProjectionExtension::Transformation : public Reference::Able
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

    // Text interface to a ConfigurableProjectionExtension instance
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
    bool valid;

  };

}

#endif
