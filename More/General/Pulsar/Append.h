//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Append.h,v $
   $Revision: 1.3 $
   $Date: 2007/07/14 22:05:12 $
   $Author: straten $ */

#ifndef _Pulsar_Append_H
#define _Pulsar_Append_H

#include "ArchiveMatch.h"
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
    /*! 

    The combine method should not make copies of the data in 'from';
    rather, 'into' should be made to point to the data in 'from'. 

    By sharing the data in 'from', it can be corrected through the
    'from' interface without concern for how the data is incorporated
    in 'into'.

    */
    virtual void combine (Archive* into, Archive* from) = 0;

    ArchiveMatch match;

  };
  
}

#endif

