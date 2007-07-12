//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Append.h,v $
   $Revision: 1.1 $
   $Date: 2007/07/12 05:59:04 $
   $Author: straten $ */

#ifndef _Pulsar_Append_H
#define _Pulsar_Append_H

#include "Reference.h"

namespace Pulsar {

  class Archive;

  //! Algorithms that combine Integration data
  class Append : public Reference::Able {

  public:

    //! Default constructor
    Append ();

    //! Copy the data in 'from' to 'into'
    void append (Archive* into, const Archive* from);

    bool must_match;

  protected:

    //! Return true if there is a benign reason to stop
    virtual bool stop (Archive* into, const Archive* from);

    //! Throw an exception if there is a fatal reason to stop
    virtual void check (Archive* into, const Archive* from);

    //! Add the data in 'from' to 'into'
    virtual void combine (Archive* into, Archive* from) = 0;

  };
  
}

#endif

