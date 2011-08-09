//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Convert.h

#ifndef __MEAL_Convert_H
#define __MEAL_Convert_H

namespace MEAL {

  //! Convert a function to another type
  /*! The parameter policy is copied and a smart pointer is managed. */
  template<typename From, typename To>
  class Convert : public To
  {

  public:

    //! Set the function to be converted
    void set_model (From* _model)
    { 
      model = _model; 
      this->copy_parameter_policy (model);
    }

    //! Get the function to be converted
    From* get_model () { return model; }

    //! Get the function to be converted
    const From* get_model () const { return model; }

    //! Return true if the function to be converted has been set
    bool has_model () const { return model; }

  protected:

    //! The function to be converted
    Reference::To<From> model;
  };

}

#endif
