//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/OneParameter.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/06 20:13:52 $
   $Author: straten $ */

#ifndef __OneParameter_H
#define __OneParameter_H

#include "MEAL/ParameterPolicy.h"
#include "Estimate.h"
#include "Error.h"

namespace MEAL {
  
  //! Abstract base class implements parameter storage and access
  class OneParameter : public ParameterPolicy {

  public:

    //! Default constructor
    OneParameter (Function* context);

    //! Copy constructor
    OneParameter (const OneParameter& np);

    //! Assignment operator
    OneParameter& operator = (const OneParameter& np);

    //! Clone operator
    OneParameter* clone (Function* context) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const
    {
      return 1;
    }

    //! Return the name of the specified parameter
    std::string get_param_name (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_param_name");
      return name;
    }

    //! Return the name of the specified parameter
    void set_param_name (unsigned index, const std::string& _name)
    {
      range_check (index, "MEAL::OneParameter::set_param_name");
      name = _name;
    }

    //! Return the value of the specified parameter
    double get_param (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_param");
      return param.val;
    }

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value);

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_variance");
      return param.var;
    }

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double value)
    {
      range_check (index, "MEAL::OneParameter::set_variance");
      param.var = value;
    }

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const
    {
      range_check (index, "MEAL::OneParameter::get_infit");
      return fit;
    }

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag)
    {
      range_check (index, "MEAL::OneParameter::set_infit");
      fit = flag;
    }

  protected:

    //! Ensure that index <= get_nparam
    void range_check (unsigned index, const char* method) const
    {
      if (index != 0)
	throw Error (InvalidRange, method, "index=%d != 0", index);
    }

  private:

    //! The Estimate of the parameter
    Estimate<double> param;

    //! Fit flag for the parameter
    bool fit;

    //! The name of the parameter
    std::string name;

  };

}

#endif
