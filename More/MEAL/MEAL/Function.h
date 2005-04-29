//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Function.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/29 12:58:29 $
   $Author: straten $ */

/*! \mainpage 
 
  \section intro Introduction
 
  The Measurement and Error Analysis Library (MEAL) implements a set
  of classes that may be used to perform non-linear least-squares and
  error propagation.  Arbitrary functions are built by modular
  construction of elementary functions, which may be parameterized by
  any number or type of arguments.

  For more details, please read the
  <a href="MEAL.pdf">Introduction to M.E.A.L.</a>

*/

#ifndef __MEAL_Function_H
#define __MEAL_Function_H

#include "MEAL/Argument.h"
#include "MEAL/ArgumentPolicy.h"
#include "MEAL/ParameterPolicy.h"
#include "Callback.h"
#include "Estimate.h"

#include <string>

//! Namespace in which all modeling and calibration related code is declared
/*! The MEAL namespace is documented in the introduction. */
namespace MEAL {

  //! Pure virtual base class of all functions
  /*! A Function may consist of an arbitrary number of parameters. Using
    the Abscissa class, a Function may also be constrained by one or more
    independent variables. The specification of the Function base class
    promotes its use with the LevenbergMarquardt template class. */
  class Function : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Low-level verbosity flag
    static bool very_verbose;

    //! When set, some Functions will throw an Error if they evaluate to zero
    static bool check_zero;

    //! When set, some Functions will throw an Error if input variance <= 0
    static bool check_variance;

    //! Construct a new model instance from a file
    static Function* load (const std::string& filename);

    //! Construct a new model instance from a string
    static Function* new_Function (const std::string& text);

    //! Default constructor
    Function ();

    //! Copy constructor
    Function (const Function& model);

    //! Assignment operator
    Function& operator = (const Function& model);

    //! Virtual destructor
    virtual ~Function ();

    //! Does the work for operator =
    virtual void copy (const Function* model);

    //! Parses the values of model parameters and fit flags from a string
    virtual void parse (const std::string& text);

    //! Prints the values of model parameters and fit flags to a string
    virtual void print (std::string& text) const;

    //! Prints the values of model parameters and fit flags to a string
    virtual void print_parameters (std::string& text, const std::string& sep) const;

    //! Return the name of the class
    virtual std::string get_name () const = 0;

    //! Return the number of parameters
    unsigned get_nparam () const
    { return parameter_policy->get_nparam (); }

    //! Return the name of the specified parameter
    std::string get_param_name (unsigned index) const
    { return parameter_policy->get_param_name (index); }

    //! Return the value of the specified parameter
    double get_param (unsigned index) const
    { return parameter_policy->get_param (index); }

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value)
    { parameter_policy->set_param (index,value); }

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const
    { return parameter_policy->get_variance (index); }

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double value)
    { parameter_policy->set_variance (index,value); }

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const
    { return parameter_policy->get_infit (index); }
    
    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag)
    { parameter_policy->set_infit (index,flag); }
    
    //! Set the independent variable of the specified dimension
    void set_argument (unsigned dimension, Argument* axis)
    { if (argument_policy) argument_policy->set_argument (dimension, axis); }

    //! Return an Estimate of the specified parameter
    Estimate<double> get_Estimate (unsigned index) const;

    //! Set the Estimate of the specified parameter
    void set_Estimate (unsigned index, const Estimate<double>& param);

    //! Function attributes that require the attention of Composite models
    enum Attribute {
      //! Number of Function parameters, as returned by get_nparam
      ParameterCount,
      //! Function evaluation, as returned by the evaluate method
      Evaluation
    };

    //! Callback executed when a Function Attribute has been changed
    Callback<Attribute> changed;

    //! Set true if the Function evaluation has changed
    void set_evaluation_changed (bool _changed = true) 
    {
      if (!evaluation_changed && _changed)
        changed.send (Evaluation);
      evaluation_changed = _changed;
    }

    //! Return true if the Function evaluation has changed
    bool get_evaluation_changed () const { return evaluation_changed; }

   protected:

    friend class ParameterPolicy;

    //! The policy for managing function parameters
    Reference::To< ParameterPolicy > parameter_policy;

    friend class ArgumentPolicy;

    //! The policy for managing function arguments
    Reference::To< ArgumentPolicy > argument_policy;

    //! Copy the evaluation changed state of another model instance
    void copy_evaluation_changed (const Function& model) 
    { set_evaluation_changed (model.get_evaluation_changed()); }

    //! Copy the parameter policy of another instance
    void copy_parameter_policy (const Function*);

  private:

    //! Flag set when the model evaluation has changed
    bool evaluation_changed;

  };

}

#endif
