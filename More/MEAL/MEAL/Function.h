//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Function.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 15:00:51 $
   $Author: straten $ */

/*! \mainpage 
 
  \section intro Introduction
 
  The Polarimetric Model Library implements a set of classes
  that may be used in the calibration of polarization data.  The
  measurement model may be built by modular construction of source
  states, signal paths, and the functional variation of these
  components with any number or type of independent variables.

  \section model Function Components

  All model components that inherit the Model::Function abstract
  base class represent functions of an arbitrary number of variables.
  A distinction is made between independent variables, or arguments,
  \f${\bf x}=(x_0, x_1, ... , x_M)\f$, and model parameters, \f${\bf
  a}=(a_0, a_1, ... , a_N)\f$.  Through use of the
  Model::Argument and Model::Argument::Value abstract base
  classes, model components may be constrained by one or more
  independent variables of arbitrary type.  The model parameters,
  \f${\bf a}\f$, represent double precision floating point values
  that may need to be constrained by some fitting technique.  Function
  classes should define an evaluation function that returns a result,
  \f$M\f$, and the partial derivative of this result with respect to
  each of the model parameters, \f$\partial M/\partial a_i\f$.  The
  independent variables,\f$\bf x\f$, represent known values, such as
  observing frequency and epoch, that may be used to further constrain
  a model.

  The Model::Function class does not define the type of value that
  it represents.  This is defined by derived types, which must define
  a type named Result and a method named evaluate:

  virtual Result evaluate (vector<Result>* gradient = 0) const = 0;

  The evaluate method returns a value of the type specified by Result
  and, if a pointer to a vector of Result is passed as the first
  argument, the vector will return the gradient of the return value
  with respect to the model parameters.

  The Return type and evaluate method are implemented by two main
  classes of Model::Function derived components:

  <UL> 
  <LI> Model::Scalar - a scalar function, 
  \f$f({\bf a}; {\bf x})\f$, such as the Model::Polynomial
  <LI> Model::Complex2 - a complex 2x2 matrix function,
  \f$J({\bf a}; {\bf x})\f$, such as the Model::Coherency matrix 
  and the Model::Rotation transformation.
  </UL>


  \subsection calculus Partial Derivatives

  A number of template classes may be used to simplify the modular
  construction of more complicated functions.  These templates
  implement the following basic rules of differentiation:

  <UL> 

  <LI> Model::ChainRule - an arbitrary function in which
  one or more parameters is set equal to the ordinate of a 
  Model::Scalar function

  <LI> Model::BinaryRule - an associative binary operation, such
  as the sum (Model::SumRule) or product
  (Model::ProductRule).

  </UL>

*/

#ifndef __Model_Function_H
#define __Model_Function_H

#include "MEPL/Argument.h"
#include "Callback.h"
#include "Estimate.h"

#include <string>

//! Namespace in which all modeling and calibration related code is declared
/*! The Model namespace is documented in the introduction. */
namespace Model {

  //! Pure virtual base class of all models
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
    static Function* load (const string& filename);

    //! Construct a new model instance from a string
    static Function* new_Function (const string& text);

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
    virtual void parse (const string& text);

    //! Prints the values of model parameters and fit flags to a string
    virtual void print (string& text) const;

    //! Prints the values of model parameters and fit flags to a string
    virtual void print_parameters (string& text, const string& sep) const;

    //! Return the name of the class
    virtual string get_name () const = 0;

    //! Return the number of parameters
    virtual unsigned get_nparam () const = 0;

    //! Return the name of the specified parameter
    virtual string get_param_name (unsigned index) const = 0;

    //! Return the value of the specified parameter
    virtual double get_param (unsigned index) const = 0;

    //! Set the value of the specified parameter
    virtual void set_param (unsigned index, double value) = 0;

    //! Return the variance of the specified parameter
    virtual double get_variance (unsigned index) const = 0;

    //! Set the variance of the specified parameter
    virtual void set_variance (unsigned index, double value) = 0;

    //! Return true if parameter at index is to be fitted
    virtual bool get_infit (unsigned index) const = 0;
    
    //! Set flag for parameter at index to be fitted
    virtual void set_infit (unsigned index, bool flag) = 0;

    //! Set the independent variable of the specified dimension
    virtual void set_argument (unsigned dimension, Argument* axis) { }

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

   protected:

    //! Set true if the Function evaluation has changed
    void set_evaluation_changed (bool _changed = true) 
    {
      if (!evaluation_changed && _changed)
        changed.send (Evaluation);
      evaluation_changed = _changed;
    }

    //! Return true if the Function evaluation has changed
    bool get_evaluation_changed () const { return evaluation_changed; }

    //! Copy the evaluation changed state of another model instance
    void copy_evaluation_changed (const Function& model) 
    { set_evaluation_changed (model.get_evaluation_changed()); }

  private:

    //! Flag set when the model evaluation has changed
    bool evaluation_changed;

  };

}

#endif
