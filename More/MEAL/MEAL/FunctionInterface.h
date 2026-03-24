//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/FunctionInterface.h

#ifndef __MEAL_FunctionInterface_h
#define __MEAL_FunctionInterface_h

#include "MEAL/Function.h"
#include "TextInterface.h"

#include <algorithm>

namespace MEAL
{

  //! Provides a text interface to get and set Function attributes
  template<class T>
  class Function::Interface : public TextInterface::To<T>
  {
    std::string name;
    std::string description;

  public:

    //! Default constructor that takes an optional instance
    Interface ( T* = 0 );

    //! Get the interface name
    std::string get_interface_name() const { return name; }

    //! Get a short description of this interface
    std::string get_interface_description () const { return description; }

  };

  //! Provides name-based access to parameter attributes in Function::Interface
  class Parameter : public Reference::Able
  {
    Reference::To<ParameterPolicy, false> parameter_policy;
    unsigned index;

    public:

    Parameter (ParameterPolicy* policy, unsigned i) : parameter_policy(policy)
    { index = i; }

    // Text interface to a Parameter instance
    class Interface : public TextInterface::To<Parameter>
    {
      public:
          Interface( Parameter *s_instance = NULL );
    };

    //! Description of the parameter
    std::string get_description () const
    { return parameter_policy->get_description (index); }

    //! Return the value of the specified parameter
    double get_param () const
    { return parameter_policy->get_param (index); }

    //! Set the value of the specified parameter
    void set_param (double value)
    { parameter_policy->set_param (index,value); }

    //! Return the variance of the specified parameter
    double get_variance () const
    { return parameter_policy->get_variance (index); }

    //! Set the variance of the specified parameter
    void set_variance (double value)
    { parameter_policy->set_variance (index,value); }

    //! Return true if parameter at index is to be fitted
    bool get_infit () const
    { return parameter_policy->get_infit (index); }

    //! Set flag for parameter at index to be fitted
    void set_infit (bool flag)
    { parameter_policy->set_infit (index,flag); }
  };
}


template<typename T>
MEAL::Function::Interface<T>::Interface( T *c )
{
  if (c)
  {
    name = c->get_name();
    description = c->get_description();

    this->set_instance (c);
  }

  this->add( &Function::get_nparam, "nparam", "Number of parameters" );

  {
    typename TextInterface::To<T>::template VGenerator<std::string> generator;

    this->add_value(generator( "name", std::string("Parameter name"),
			  &Function::get_param_name,
			  &Function::get_nparam ));

    this->add_value(generator( "help", std::string("Parameter description"),
			  &Function::get_param_description,
			  &Function::get_nparam ));
  }

  {
    typename TextInterface::To<T>::template VGenerator<double> generator;

    this->add_value(generator( "val", std::string("Parameter value"),
			  &Function::get_param,
			  &Function::set_param,
			  &Function::get_nparam ));

    this->add_value(generator( "var", std::string("Parameter variance"),
			  &Function::get_variance,
			  &Function::set_variance,
			  &Function::get_nparam ));
  }

  {
    typename TextInterface::To<T>::template VGenerator<bool> generator;

    this->add_value(generator( "fit", std::string("Fit flag"),
			  &Function::get_infit,
			  &Function::set_infit,
			  &Function::get_nparam ));
  }

  if (!c)
    return;

  if (!c->has_parameter_policy())
    return;

  ParameterPolicy* policy = c->get_parameter_policy();

  unsigned nparam = c->get_nparam();

  // check for duplicate names

  std::vector<std::string> names (nparam);
  for (unsigned iparam=0; iparam < nparam; iparam++)
    names[iparam] = c->get_param_name (iparam);

  std::sort (names.begin(), names.end());
  if (std::adjacent_find(names.begin(), names.end()) != names.end())
    return;

  // there are no duplicate names

  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    std::string name = c->get_param_name (iparam);
    Parameter* parameter = new Parameter (policy, iparam);
    this->insert( name, new Parameter::Interface (parameter) );
  }

#if 0
  add( &Function::get_,
       &Function::set_,
       "", "" );
#endif

}

#endif

