/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConfigurableProjectionExtension.h"

using namespace std;

Pulsar::ConfigurableProjectionExtension::Interface::Interface
( ConfigurableProjectionExtension *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  // read-only: requires resize
  add( &ConfigurableProjectionExtension::get_nchan,
       "nchan", "Number of frequency channels" );

  add( &ConfigurableProjectionExtension::get_nparam,
       "nparam", "Number of model parameters" );

  add( &ConfigurableProjectionExtension::get_configuration,
       &ConfigurableProjectionExtension::set_configuration,
       "config", "Configuration string" );

  VGenerator<float> fgenerator;
  add_value(fgenerator( "wt", "Weight assigned to each channel",
                        &ConfigurableProjectionExtension::get_weight,
                        &ConfigurableProjectionExtension::set_weight,
                        &ConfigurableProjectionExtension::get_nchan ));
  import( "eqn", Transformation::Interface(),
          (Transformation*(ConfigurableProjectionExtension::*)(unsigned))
	  &ConfigurableProjectionExtension::get_transformation,
          &ConfigurableProjectionExtension::get_nchan );
}

Pulsar::ConfigurableProjectionExtension::Transformation::Interface::Interface ()
{
  {
    VGenerator<string> generator;
    add_value(generator( "name", "Parameter name",
			 &Transformation::get_param_name,
			 &Transformation::set_param_name,
			 &Transformation::get_nparam ));

    add_value(generator( "desc", "Parameter description",
			 &Transformation::get_param_description,
			 &Transformation::set_param_description,
			 &Transformation::get_nparam ));
  }

  {
    VGenerator<double> generator;
    add_value(generator( "val", "Parameter value",
			 &Transformation::get_param,
			 &Transformation::set_param,
			 &Transformation::get_nparam ));

    add_value(generator( "var", "Parameter variance",
			 &Transformation::get_variance,
			 &Transformation::set_variance,
			 &Transformation::get_nparam ));
  }
}

