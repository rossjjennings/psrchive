//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_ConfigurableProjection_h
#define __Pulsar_ConfigurableProjection_h

#include "Pulsar/VariableProjectionCorrection.h"
#include "Pulsar/VariableTransformation.h"

#include "Pulsar/ConfigurableProjectionExtension.h"

#include "MEAL/Axis.h"

namespace Pulsar 
{
  //! Manager of variable transformations
  class ConfigurableProjection : public VariableTransformationManager
  {
  public:

    //! Construct from a configuration file
    ConfigurableProjection (const std::string& filename);

    //! Construct from a ConfigurableProjectionExtension
    ConfigurableProjection (const ConfigurableProjectionExtension*);

    //! Construct from a configuration string
    void construct (const std::string&);

    //! Set the Archive for which a tranformation will be computed
    void set_archive (const Archive* _archive);

    //! Set the sub-integration for which a tranformation will be computed
    void set_subint (unsigned _subint);

    //! Set the frequency channel for which a tranformation will be computed
    void set_chan (unsigned _chan);

    //! Set the number of frequency channels with a unique Transformation
    void set_nchan (unsigned nchan) override;

    //! Return a newly constructed Argument::Value for the current archive / subint / chan
    MEAL::Argument::Value* new_value () override;

    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Get the number of abscissa / dimensions that describe the variability
    unsigned get_ndim () const;

    //! Return the value associated with the parameter name
    double get_value (const std::string& name);

    //! Return the attribute names for each abscissa of the function constraining the specified index
    const std::vector<std::string>& get_abscissa_names (unsigned index) { return parameters.at(index); }

    //! Get the configuration text from which this instance was constructed
    const std::string& get_configuration() const { return configuration; }

    //! Communicates parameters to plotting routines
    class Info;

    //! Calibrate an observation
    /*! \pre The backend should be fully corrected and calibrated, and the basis should be corrected */
    void calibrate (Archive*);

    //! Subject an observation to the transformation (inverse of calibration)
    void transform (Archive*);

    //! Set the projection from the antenna to the celestial reference frame
    void set_projection (KnownVariableTransformation* known) { projection = known; }

    //! Get the projection from the antenna to the celestial reference frame
    KnownVariableTransformation* get_projection () { return projection; }

  protected:

    //! Configuration string
    std::string configuration;

    //! Known/fixed projection correction
    Reference::To<KnownVariableTransformation> projection;

    //! Model inserted between instrument and projection
    /*! This attribute is cloned in each new Transformation */
    Reference::To< Calibration::VariableTransformation > transformation;

    //! Names of Archive attributes assigned to each abscissa/dimension
    std::map< unsigned, std::vector<std::string> > parameters;

    //! The effective number of dimensions / abscissa
    unsigned effective_ndim = 0;
    
    //! Performs the work for calibrate and transform
    void transform_work (Archive* arch, const std::string& name, bool invert);

  public:

    class Transformation : public VariableTransformationManager::Transformation
    {
    protected:
  
      //! The variable transformation argument
      MEAL::Axis< Calibration::VariableTransformation::Argument > argument;
  
      //! The variable transformation
      Reference::To<Calibration::VariableTransformation> transformation;

    public:
  
      Transformation (Calibration::VariableTransformation* xform)
      {
        transformation = xform;
        argument.signal.connect (xform, &Calibration::VariableTransformation::set_argument);
      }
  
      //! The transformation
      Calibration::VariableTransformation* get_transformation ()
      { return transformation; }
 
      //! The transformation
      const Calibration::VariableTransformation* get_transformation () const
      { return transformation; }
 
      //! The argument to the transformation
      MEAL::Argument* get_argument () { return &argument; }
    };

    //! Return the Transformation for the specified channel
    Transformation* get_transformation (unsigned ichan) override;

    //! Return the Transformation instance for the specified channel
    const Transformation* get_transformation (unsigned ichan) const;

    //! Return true if the speficied channel has a valid solution
    bool get_transformation_valid (unsigned ichan) const;

  protected:

    //! The Transformation instances for each channel
    std::vector< Reference::To<Transformation> > xforms;
  };

  void copy (MEAL::Complex2* to, const ConfigurableProjectionExtension::Transformation* from);

  void copy (ConfigurableProjectionExtension::Transformation* to, const MEAL::Complex2* from);

}

#endif
