//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Composite.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Composite_H
#define __Composite_H

#include "Calibration/ParameterBehaviour.h"
#include "Calibration/ArgumentBehaviour.h"
#include "Calibration/Projection.h"

namespace Calibration {

  //! Abstract base class representing a model composed of models
  class Composite : public ParameterBehaviour, public ArgumentBehaviour {

  public:

    //! Default constructor
    Composite ();

    //! Get the number of models mapped into this model
    unsigned get_nmodel () const;

    //! Get the name used in verbose output
    string class_name() const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const;

    //! Return the name of the specified parameter
    string get_param_name (unsigned index) const;

    //! Return the value of the specified parameter
    double get_param (unsigned index) const;

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value);

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const;

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double variance);

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const;

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag);

    //! Set the independent variable of the specified dimension
    void set_argument (unsigned dimension, Argument* axis);

  protected:

    //! Provide access to Projection base class
    friend class Projection;

    //! Convenience interface to map (Projection*)
    template <class Type>
    void map (Project<Type>& model, bool signal_changes = true)
    {
      if (very_verbose)
	cerr << class_name() + "map (Project<Type>)" << endl;
      map (model.get_map(), signal_changes);
    }

    //! Convenience interface to unmap (Projection*)
    template <class Type>
    void unmap (Project<Type>& model, bool signal_changes = true)
    {
      if (very_verbose)
	cerr << class_name() + "unmap (Project<Type>)" << endl;
      unmap (model.get_map(), signal_changes);
    }

    //! Map the Projection into the composite mappting
    void map (Projection* model, bool signal_changes = true);

    //! Remove the Projection from the composite mapping
    void unmap (Projection* model, bool signal_changes = true);

    //! Return the index for the specified model
    unsigned find_Model (Model* model) const;

    //! Return the index for the specified model
    unsigned find_Projection (Projection* model) const;

  private:

    //! References to Projection instances
    vector< Reference::To<Projection> > maps;

    //! References to Model instances
    vector< Reference::To<Model> > models;
    
    //! The total number of Model parameters
    unsigned nparameters;
    
    //! Optimization: keep track of the current model
    unsigned current_model;

    //! Optimization: keep track of the base index of the current model
    unsigned current_index;

    //! Method called when a Model attribute has changed
    void attribute_changed (Attribute attribute);

    //! Recursive function does the work for map
    void add_component (Model* model, vector<unsigned>& imap);

    //! Recursive function does the work for unmap
    void remove_component (Model* model);

    //! Remap the parameter indeces
    void remap (bool signal_changes = true);

    //! Get the const Model that corresponds to the given index
    const Model* get_Model (unsigned& index) const;

    //! Get the Model that corresponds to the given index
    Model* get_Model (unsigned& index);

    //! Check the the reference to the specified model is still valid
    void reference_check (unsigned imodel, char* method) const;

  };

}


#endif

