//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/lazy.h

#ifndef __lazy_h
#define __lazy_h

#include "debug.h"

//! Lazy construction of global and static variables

/*!
  The order in which the constructors of global variables are called
  is undefined.  Therefore, it is better to never use global instances
  of classes; rather, global pointers to classes should be used.  The
  following compiler macros takes a little bit of the inconvenience out of
  declaring/initializing a static pointer and implementing access to it.
 */


/* 
  LAZY_GLOBAL implements a static pointer and (static) class method
  that provides access to it.

  Two symbols are created:

  static type* the_name
  type& class::get_name ()

  and, when constructed:

  the_name = new type (value)
*/

#define LAZY_GLOBAL(class,type,name,value)		 \
  static type* the_##name = 0;				 \
  type& class :: get_##name ()				 \
  {							 \
    if (! the_##name)					 \
      the_##name = new type ( value );			 \
    DEBUG( #class "::get_" #name " ptr=" << the_##name); \
    return *the_##name;					 \
  }

/*
  LAZY_STATIC implements a static pointer and static function
  that provides access to it.

  Two symbols are created:

  static type* the_name
  type& get_name ()

  and, when constructed:

  the_name = new type (value)
*/

#define LAZY_STATIC(type,name,value)                     \
  static type* the_##name = 0;                           \
  static type& get_##name ()                             \
  {                                                      \
    if (! the_##name)                                    \
      the_##name = new type ( value );                   \
    DEBUG( "get_" #name " ptr=" << the_##name);          \
    return *the_##name;                                   \
  }

#endif

