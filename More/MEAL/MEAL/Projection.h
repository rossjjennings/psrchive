//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Projection.h,v $
   $Revision: 1.4 $
   $Date: 2005/02/13 08:33:03 $
   $Author: straten $ */

#ifndef __Projection_H
#define __Projection_H

#include "Reference.h"

namespace MEAL {

  class Function;

  //! Manages a Function and the mapping of its parameters into a Composite
  class Projection : public Reference::Able {

    friend class Composite;

  public:
    //! Default constructor
    Projection (Function* model=0, Composite* meta=0);

    //! Destructor
    ~Projection ();

    //! Return the mapped index
    unsigned get_imap (unsigned index) const;

    //! Return the model
    Function* get_Function();

    //! Set the model
    void set_Function (Function* _model);

  protected:

    //! The Function that is mapped into a Composite
    Reference::To<Function> model;

    //! The parameter index mapping into Composite
    std::vector<unsigned> imap;

    //! The Composite into which the model is mapped
    Reference::To<Composite, false> meta;
  };


  //! Template combines a reference to a Function type and its Projection
  template<class Function>
  class Project : public Reference::To<Function> {

  public:

    //! Default constructor
    Project (Function* model = 0)
    { 
      Reference::To<Function>::operator = (model);
      projection = new Projection (model);
    }

    //! Assignment operator
    const Project& operator = (Function* model) 
    {
      Reference::To<Function>::operator = (model);
      projection->set_Function (model);
      return *this; 
    }

    //! Return the Projection
    Projection* get_map ()
    {
      return projection;
    }

    //! Return the const Projection
    const Projection* get_map () const
    {
      return projection;
    }

    //! Return the mapped index
    unsigned get_imap (unsigned index) const
    {
      return projection->get_imap (index);
    }

  protected:

    //! The Projection
    Reference::To<Projection> projection;

  };

}


#endif

