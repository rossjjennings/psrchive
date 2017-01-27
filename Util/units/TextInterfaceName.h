//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextInterfaceName.h

#ifndef __TextInterfaceName_h
#define __TextInterfaceName_h

#include "Reference.h"

namespace TextInterface {

  //! Defines the sequence of characters that constitute a valid name
  class Name : public Reference::Able
  {
  public:

    //! Default constructor
    Name ();

    //! Operator interface
    bool operator () (char c) const { return valid(c); }

    //! Return true if c is the next character in a valid name
    bool valid (char c) const;

    //! Return true if c is not the next character in a valid name
    bool invalid (char c) const { return ! valid (c); }

    //! Reset to the start of the next variable name 
    void reset ();

  protected:
    mutable unsigned count;
    mutable bool in_range;
    mutable bool in_precision;
  };

}

#endif
