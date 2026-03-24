/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/FunctionInterface.h"

MEAL::Parameter::Interface::Interface ( Parameter *s_instance )
{
  std::string value_name = "Parameter value";

  if ( s_instance )
  {
    set_instance( s_instance );
    std::string help = s_instance->get_description();
    if ( help != "" )
      value_name = help;
  }

  add( &Parameter::get_param,
       &Parameter::set_param,
       "val", value_name.c_str() );

  add( &Parameter::get_variance,
       &Parameter::set_variance,
       "var", "Parameter variance" );

  add( &Parameter::get_infit,
       &Parameter::set_infit,
       "fit", "Fit flag" );
}

