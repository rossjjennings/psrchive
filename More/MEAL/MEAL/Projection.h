//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Projection.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __Projection_H
#define __Projection_H

#include "Reference.h"

namespace Calibration {

  class Model;

  //! Manages a Model and the mapping of its parameters into a Composite
  class Projection : public Reference::Able {

    friend class Composite;

  public:
    //! Default constructor
    Projection (Model* model=0, Composite* meta=0);

    //! Return the mapped index
    unsigned get_imap (unsigned index) const;

    //! Return the model
    Model* get_Model();

    //! Set the model
    void set_Model (Model* _model);

  protected:

    //! The Model that is mapped into a Composite
    Reference::To<Model> model;

    //! The parameter index mapping into Composite
    vector<unsigned> imap;

    //! The Composite into which the model is mapped
    Reference::To<Composite, false> meta;
  };


  //! Template combines a reference to a Model type and its Projection
  template<class Model>
  class Project : public Reference::To<Model> {

  public:

    //! Default constructor
    Project (Model* model = 0)
    { 
      Reference::To<Model>::operator = (model);
      projection = new Projection (model);
    }

    //! Assignment operator
    const Project& operator = (Model* model) 
    {
      Reference::To<Model>::operator = (model);
      projection->set_Model (model);
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

