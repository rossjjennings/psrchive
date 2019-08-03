//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_VariableTransformation_h
#define __Pulsar_VariableTransformation_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Analysis of the impact of instrumental boost on phase shift estimate
  class VariableTransformation : public Reference::Able
  {

  protected:
    Reference::To<const Archive,false> archive;
    unsigned subint;
    unsigned chan;

    mutable std::string description;
    bool built;

  public:

    //! Default constructor
    VariableTransformation ();

    //! Set the Archive for which a tranformation will be computed
    virtual void set_archive (const Archive* _archive);

    //! Set the sub-integration for which a tranformation will be computed
    virtual void set_subint (unsigned _subint);

    //! Set the frequency channel for which a tranformation will be computed
    virtual void set_chan (unsigned _chan);

    //! Get the transformation
    virtual Jones<double> get_transformation () = 0;

    //! Get the description of the transformation
    virtual std::string get_description () const { return description; }

    //! Return true if the transformation is required
    virtual bool required () const { return true; }

  };
}

#endif
