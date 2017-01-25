//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/ModifyRestore.h

#ifndef __ModifyRestore_h
#define __ModifyRestore_h

//! Modifies a value on construction and restores it on destruction
template<class T>
class ModifyRestore
{
 public:

  //! Modify constructor
  ModifyRestore (T& value, const T& modified_value) : the_value (value)
    { restore_value = value; the_value = modified_value; }

  //! Modify constructor
  ModifyRestore (T& value) : the_value (value)
    { restore_value = value; }

  //! Restore destructor
  ~ModifyRestore ()
    { the_value = restore_value; }

 protected:

  //! Reference to the value to be restored
  T& the_value;

  //! Copy of the value before modification
  T restore_value;

};

#endif
