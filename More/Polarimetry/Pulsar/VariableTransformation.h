//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/VariableTransformation.h

#ifndef __CalibrationVariableTransformation_H
#define __CalibrationVariableTransformation_H

#include "MEAL/Complex2.h"
#include "MEAL/Nvariate.h"
#include "MEAL/ProductRule.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Value.h"

#include "Estimate.h"
#include "Stokes.h"

#include <map>

namespace Calibration {

  //! Product of multi-variate model of antenna and known corrections
  /*! result = post_correction * model * pre_correction */
  class VariableTransformation : public MEAL::ProductRule<MEAL::Complex2>
  {
    //! Chain rule used to constrain model parameters with functions
    MEAL::ChainRule<MEAL::Complex2> chain;

    //! Any known pre-model correction
    MEAL::Value<MEAL::Complex2> pre_correction;

    //! The model to be constrained
    Reference::To<MEAL::Complex2> model;

    //! Any known post-model correction
    MEAL::Value<MEAL::Complex2> post_correction;

    //! Map of model index to constraining multivariate function
    std::map< unsigned, Reference::To< MEAL::Nvariate<MEAL::Scalar> > > function;

  public:

    //! Default Constructor
    VariableTransformation ();

    //! Copy Constructor
    VariableTransformation (const VariableTransformation& s);

    //! Assignment Operator
    const VariableTransformation& operator = (const VariableTransformation& s);

    //! Destructor
    ~VariableTransformation ();

    //! Clone operator
    VariableTransformation* clone () const;

    //! Set the model that is constrained by abscissae
    void set_model (MEAL::Complex2*);

    //! Get the model that is constrained by abscissae
    const MEAL::Complex2* get_model () const;
    MEAL::Complex2* get_model ();

    //! Set the multivariate function that constrains the specified parameter
    void set_constraint (unsigned index, MEAL::Nvariate<MEAL::Scalar>*);

    //! Get the number of constrained parameters
    unsigned get_nconstraint () const { return function.size(); }

    //! Get the index of the jth constrained parameter
    unsigned get_index (unsigned j);
    
    //! Return true if the index is constrained
    bool has_constraint (unsigned index) { return function.find(index) != function.end(); }

    //! Get the constraining function for the specified index
    MEAL::Nvariate<MEAL::Scalar>* get_constraint (unsigned index) { return function.at(index); }

    class Argument
    {
    public:

      //! the known pre-model correction
      Jones<double> pre_correction;

      //! the known post-model correction
      Jones<double> post_correction;

      //! the arguments for each abscissa
      std::map< unsigned, std::vector<double> > arguments;
    };

    void set_argument (const Argument&);

  private:

    //! Initialize function used by constructors
    void init ();

  };

  //! Specialize the template defined in MEAL/Axis.h
  std::string axis_value_to_string(const VariableTransformation::Argument&);

}

#endif

