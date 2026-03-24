//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_VariableTransformationManager_h
#define __Pulsar_VariableTransformationManager_h

#include "Pulsar/Archive.h"
#include "MEAL/Variable.h"
#include "MEAL/Complex2.h"

namespace Pulsar {
  
  //! Manager of variable transformations
  class VariableTransformationManager : public Reference::Able
  {

  protected:

    Reference::To<const Archive,false> archive;
    unsigned subint;
    unsigned chan;

    mutable std::string description;
    mutable bool built;

  public:

    //! Default constructor
    VariableTransformationManager ();

    typedef MEAL::Variable<MEAL::Complex2> Transformation;
    
    //! Set the Archive for which a tranformation will be computed
    virtual void set_archive (const Archive* _archive);

    //! Set the sub-integration for which a tranformation will be computed
    virtual void set_subint (unsigned _subint);

    //! Set the frequency channel for which a tranformation will be computed
    virtual void set_chan (unsigned _chan);

    //! Update the tranformation for the current archive / subint / chan
    virtual void update ();

    //! Set the number of frequency channels with a unique Transformation
    virtual void set_nchan (unsigned nchan) = 0;

    //! Return the Transformation for the specified channel
    virtual Transformation* get_transformation (unsigned ichan) = 0;

    //! Return a newly constructed Argument::Value for the current archive / subint / chan
    virtual MEAL::Argument::Value* new_value () = 0;

    //! Get the description of the transformation
    virtual std::string get_description () const { return description; }

    //! Return true if the transformation is required
    virtual bool required () const { return true; }
  };
}

#endif
