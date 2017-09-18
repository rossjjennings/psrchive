//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextIndex.h

#ifndef __TextIndex_h
#define __TextIndex_h

#include "TextInterface.h"

//! Parses and manages a named set of indeces
class TextIndex : public Reference::Able
{
 public:

  //! Construct from string of the form 'name=[range]'
  TextIndex (const std::string& named_range);

  //! Set the interface to the container to which the named index applies
  void set_container (TextInterface::Parser*);

  //! Set the name of the index dimension size in the container interface
  /*! \default "n" + name  */
  void set_size_name (const std::string& name);

  //! Get the number of indeces
  unsigned size () const;

  //! Get the index command for the specified index
  std::string get_index (unsigned i);

  //! Get the index command for the current index
  std::string get_current_index ();
  
 protected:

  //! The text interface to the container
  Reference::To< TextInterface::Parser, false > container;
  
  //! The name of the index
  std::string name;

  //! The name of the index dimension in the container
  std::string size_name;

  //! The range of the index set
  std::string range;

  //! The indeces
  std::vector<unsigned> indeces;

};

#endif
