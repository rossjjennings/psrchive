//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Pulsar/TextParameters.h,v $
   $Revision: 1.3 $
   $Date: 2009/03/15 14:32:43 $
   $Author: straten $ */

#ifndef __TextParameters_h
#define __TextParameters_h

#include "Pulsar/Parameters.h"

namespace Pulsar {

  //! Stores pulsar parameters as uninterpreted text
  class TextParameters : public Parameters {

  public:

    //! Return a new, copy constructed instance of self
    Parameters* clone () const;

    //! Return true if *this == *that
    bool equals (const Parameters* that) const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

    //! Return the pulsar name
    std::string get_name () const;

    //
    //
    //

    //! Retrieve a string from the text
    std::string get_value (const std::string& keyword) const;

    //! Retrieve a value from the text, return true if found
    template<typename T>
    bool get_value (T& value, const std::string& keyword, 
		    bool exception=true) const;

  protected:

    std::string text;
  };

}

//! Retrieve a value from the text
template<typename T>
bool Pulsar::TextParameters::get_value (T& value, 
					const std::string& keyword, 
					bool exception) const
{
  std::string value_string = get_value (keyword);

  if (value_string.empty())
  {
    if (!exception)
      return false;

    throw Error (InvalidParam, "Pulsar::TextParameters::get_value<T>",
		 "keyword='" + keyword + "' not found");
  }

  value = fromstring<T> (value_string);
  return true;
}

#endif
