/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/ConfigurableProjection.h"
#include "Pulsar/TransformationFactory.h"

#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/IntegrationInterface.h"
#include "Pulsar/IntegrationExpert.h"
#include "MEAL/NvariateScalarFactory.h"

// #define _DEBUG 1
#include "debug.h"

#if HAVE_YAMLCPP
#include <yaml-cpp/yaml.h>
#endif

using namespace Pulsar;
using namespace std;

#if HAVE_YAMLCPP

//! Construct an object from a YAML::Node
template<typename T, typename Factory>
T* construct (const YAML::Node& node, Factory factory)
{
  if (!node.IsMap())
    throw Error (InvalidParam, "construct<T> (YAML::Node)",
                 "YAML::Node is not a Map");

  if ( !node["model"] )
    throw Error (InvalidParam, "construct<T> (YAML::Node)",
                 "map does not contain 'model'");

  YAML::Node model = node["model"];

  if (model.IsScalar())
    return factory( model.as<std::string>() );

  if (!model.IsMap())
    throw Error (InvalidParam, "construct<T> (YAML::Node)",
                 "YAML::Node for 'model' is neither a Scalar nor a Map");

  if ( !model["name"] )
    throw Error (InvalidParam, "ConfigurableProjection::load",
                 "model map does not contain 'name'");

  Reference::To<T> object = factory( model["name"].as<std::string>() );

  Reference::To<TextInterface::Parser> interface = object->get_interface();

  for (auto it=model.begin(); it!=model.end(); ++it)
  {
    string key = it->first.as<string>();

    if (key == "name")
      continue;

    string value = it->second.as<string>();

    DEBUG("parsing key='" << key << "' value='" << value << "'");
    interface->set_value (key, value);
  }

  // before releasing the object, delete the interface so that its passive reference does not delete object
  interface = nullptr;

  return object.release();
}

#endif

typedef MEAL::Nvariate<MEAL::Scalar> NvariateScalar;

ConfigurableProjection::ConfigurableProjection (const ConfigurableProjectionExtension* ext)
{
  if (Archive::verbose > 2)
    cerr << "ConfigurableProjection construct from ConfigurableProjectionExtension" << endl;

  construct (ext->get_configuration());

  unsigned nchan = ext->get_nchan();
  set_nchan (nchan); // also resizes

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    Transformation* transformation = get_transformation(ichan);
    MEAL::Complex2* xform = transformation->get_transformation ();

    copy (xform, ext->get_transformation(ichan));
  }
}

ConfigurableProjection::ConfigurableProjection (const string& filename)
{
#if HAVE_YAMLCPP

  YAML::Node node = YAML::LoadFile(filename);

  YAML::Emitter out;

  node.SetStyle(YAML::EmitterStyle::Flow);

  out << node;
  
  construct (out.c_str());

#else
  throw Error (InvalidState, "ConfigurableProjection::construct",
               "yaml-cpp required and not available");
#endif
}

void ConfigurableProjection::construct (const string& text)
{
#if HAVE_YAMLCPP

  if (Archive::verbose > 2)
    cerr << "ConfigurableProjection::construct cfg='" << text << "'" << endl;

  YAML::Node node = YAML::Load (text);

  configuration = text;

  transformation = new Calibration::VariableTransformation;

  Calibration::TransformationFactory xform_factory;
  MEAL::Complex2* model = ::construct<MEAL::Complex2> (node, xform_factory);
  transformation->set_model (model);

  if (Archive::verbose > 2)
    cerr << "ConfigurableProjection::construct model parsed" << endl;

  MEAL::NvariateScalarFactory function_factory;

  effective_ndim = 0;

  for (auto it=node.begin(); it!=node.end(); ++it)
  {
    string key = it->first.as<string>();

    DEBUG("ConfigurableProjection::construct key=" << key);

    if ( key == "chain" )
    {
      YAML::Node chain = it->second;
      if (! chain.IsMap())
        throw Error (InvalidParam, "ConfigurableProjection::load",
                     "YAML value for 'chain' is not a map");

      DEBUG("parsing chain param");
      if ( !chain["param"] )
        throw Error (InvalidParam, "ConfigurableProjection::load",
                     "YAML chain map does not contain 'param'");

      string param_name = chain["param"].as<string>();

      unsigned iparam = 0;
      while (iparam < model->get_nparam())
      {
        if (model->get_param_name(iparam) == param_name)
          break;
        else
          iparam ++;
      }

      if (iparam == model->get_nparam())
        throw Error (InvalidParam, "ConfigurableProjection::load",
                     "no parameter named '" + param_name + "' "
                     "in model named '" + model->get_name() + "'");


      DEBUG("parsing chain model");
      NvariateScalar* func = ::construct<NvariateScalar> (chain, function_factory);

      transformation->set_constraint (iparam, func);

      if ( !chain["args"] )
        throw Error (InvalidParam, "ConfigurableProjection::load",
                     "YAML chain map does not contain 'args'");

      DEBUG("parsing arguments");
      YAML::Node args = chain["args"];

      vector<string> params;

      if (args.IsScalar())
      {
        if (func->get_ndim() != 1)
          cerr << "WARNING: number of arguments = 1 != "
                "number of dimensions = " << func->get_ndim() << endl;

        params.resize (1, args.as<string>());
      }

      else
      {
        if (! args.IsSequence())
          throw Error (InvalidParam, "ConfigurableProjection::load",
                       "YAML value for 'args' is not a map");

        if (args.size() != func->get_ndim())
          cerr << "WARNING: number of arguments = " << args.size() << " != "
                  "number of dimensions = " << func->get_ndim() << endl;

        params.resize (args.size());
        for (unsigned i=0; i<args.size(); i++)
          params[i] = args[i].as<string>();
      }

      unsigned infit_count = 0;
      for (unsigned i=0; i<func->get_nparam(); i++)
        if (func->get_infit(i))
          infit_count ++;

      // consider the dimension of the function only if it has free parameters
      if (infit_count)
      {
        if (Archive::verbose)
          cerr << "ConfigurableProjection::load adding " << func->get_ndim() << " to effective ndim" << endl;
        effective_ndim = std::max(effective_ndim,func->get_ndim());
      }
      else if (Archive::verbose)
        cerr << "ConfigurableProjection::load ignoring ndim=" << func->get_ndim() << " of model with no free parameters" << endl;

      parameters[iparam] = params;
    }
  }

#else
  throw Error (InvalidState, "ConfigurableProjection::construct",
               "yaml-cpp required and not available");
#endif

  projection = new VariableProjectionCorrection;
}

//! Set the Archive for which a tranformation will be computed
void ConfigurableProjection::set_archive (const Archive* _archive)
{
  VariableTransformationManager::set_archive (_archive);
  projection->set_archive (_archive);
}

//! Set the sub-integration for which a tranformation will be computed
void ConfigurableProjection::set_subint (unsigned _subint)
{
  VariableTransformationManager::set_subint (_subint);
  projection->set_subint (_subint);
}

//! Set the frequency channel for which a tranformation will be computed
void ConfigurableProjection::set_chan (unsigned _chan)
{
  VariableTransformationManager::set_chan (_chan);
  projection->set_chan (_chan);
}

void ConfigurableProjection::set_nchan (unsigned nchan)
{
  xforms.resize (nchan);
}

unsigned ConfigurableProjection::get_nchan () const
{
  return xforms.size();
}

unsigned ConfigurableProjection::get_ndim () const
{
  return effective_ndim;
}

ConfigurableProjection::Transformation*
ConfigurableProjection::get_transformation (unsigned ichan)
{
  if (ichan >= xforms.size())
    throw Error (InvalidParam, "ConfigurableProjection::get_transformation",
                 "ichan=%u >= nchan=%u", ichan, xforms.size());

  if (!xforms[ichan])
  {
    Calibration::VariableTransformation* xform = transformation->clone();
    xforms[ichan] = new ConfigurableProjection::Transformation (xform);
  }

  return xforms[ichan];
}

const ConfigurableProjection::Transformation*
ConfigurableProjection::get_transformation (unsigned ichan) const
{
  if (ichan >= xforms.size())
    throw Error (InvalidParam, "ConfigurableProjection::get_transformation",
                 "ichan=%u >= nchan=%u", ichan, xforms.size());

  if (!xforms[ichan])
    throw Error (InvalidParam, "ConfigurableProjection::get_transformation",
                 "ichan=%u invalid");

  return xforms[ichan];
}

bool ConfigurableProjection::get_transformation_valid (unsigned ichan) const
{
  return xforms[ichan];
}

using Calibration::VariableTransformation;

MEAL::Argument::Value* ConfigurableProjection::new_value ()
{
  MEAL::Argument* argument = xforms[chan]->get_argument ();

  typedef MEAL::Axis< Calibration::VariableTransformation::Argument > VariableArgument;

  VariableArgument* vararg = dynamic_cast<VariableArgument*> (argument);

  if (!vararg)
    throw Error (InvalidParam, "ConfigurableProjection::new_value",
		 "Transformation does not have a VariableArgument");

  Calibration::VariableTransformation::Argument arg;

  auto var = dynamic_cast<VariableProjectionCorrection*>(projection.get());

  if (var)
  {
    arg.pre_correction = var->get_antenna_projection();
    arg.post_correction = var->get_feed_projection();
  }
  else
  {
    arg.pre_correction = projection->get_value();
    arg.post_correction = Jones<double>::identity();
  }

  if (Archive::verbose > 3)
  {
    cerr << "ConfigurableProjection::new_value pre=" << arg.pre_correction << endl;
    cerr << "ConfigurableProjection::new_value post=" << arg.post_correction << endl;
  }

  for (auto param : parameters)
  {
    unsigned index = param.first;

    unsigned narg = param.second.size();
    vector<double> args (narg);

    for (unsigned iarg=0; iarg < narg; iarg++)
      args[iarg] = get_value( param.second[iarg] );

    arg.arguments[index] = args;
  }

  return vararg->new_Value( arg );
}

//! Return the value associated with the parameter name
double ConfigurableProjection::get_value (const std::string& name)
{
  // until a const interface with only get access is implemented ...
  Archive* data = const_cast<Archive*> ( archive.get() );

  TextInterface::Parser* parser = 0;
  string param;

  if ( name.substr (0, 4) == "int:")
  {
    parser = data -> get_Integration (subint) -> get_interface();
    param = name.substr (4);
  }
  else
  {
    parser = data->get_interface ();
    param = name;
  }

  // print with 15 digits of precision
  param += "%15";

  // ensure that angles are reported in radians
  Angle::default_type = Angle::Radians;

  double result = fromstring<double>( parser->get_value (param) );

  if (Archive::verbose > 2)
    cerr << "ConfigurableProjection::get_value name=" << param << " val=" << result << endl;

  return result;
}

void Pulsar::ConfigurableProjection::calibrate (Archive* arch)
{
  transform_work (arch, "calibrate", true);
}

void Pulsar::ConfigurableProjection::transform (Archive* arch)
{
  transform_work (arch, "transform", false);
}

void Pulsar::ConfigurableProjection::transform_work (Archive* arch, const std::string& name, bool invert) try
{
  if (Archive::verbose)
    cerr << "ConfigurableProjection::"+name << endl;

  set_archive (arch);

  unsigned nsubint = arch->get_nsubint();
  unsigned nchan = arch->get_nchan();

  if (nchan != get_nchan())
    throw Error (InvalidState, "ConfigurableProjection::"+name,
                 "Archive::nchan=%d != ConfigurableProjection::nchan=%d",
                 nchan, get_nchan());

  vector< Jones<float> > response (nchan);

  for (unsigned isub=0; isub < nsubint; isub++)
  {
    set_subint (isub);

    Integration* subint = arch->get_Integration (isub);

    for (unsigned ichan=0; ichan < nchan; ichan++) try
    {
      set_chan (ichan);
      update ();

      if (get_transformation_valid(ichan))
      {
        auto resp = get_transformation(ichan)->get_transformation()->evaluate();
        if (Archive::verbose > 2)
          cerr << "ConfigurableProjection::"+name+" isub=" << isub << " ichan=" << ichan 
               << " response=" << resp << endl;  

        if (invert)
          response[ichan] = inv(resp);
        else
          response[ichan] = resp;
      }
      else
      {
        DEBUG("ConfigurableProjection::"+name+" bad chan=" << ichan);
        response[ichan] = 0.0;
        subint->set_weight(ichan,0);
      }
    }
    catch (Error& error)
    {
      cerr << "ConfigurableProjection::"+name+" ichan=" << ichan << " error=" << error << endl;
      response[ichan] = 0.0;
      subint->set_weight(ichan,0);
    }

    subint->expert()->transform (response);
  }
}
catch (Error& error)
{
  throw error += "ConfigurableProjection::" + name;
}