/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Reference.h"
#include "Error.h"
#include "strutil.h"

#include <vector>

//! Factory returns a new instance of a Parent-derived class
/*! Parent must define a static member function

    static void children (std::vector<Reference::To<Parent>&);

    that returns a vector of newly constructed children.
    Parent must also declare a pure virtual member function

    virtual void load (FILE*) = 0;

    The load method of each Parent-derived class will be attempted,
    and any Error exceptions that are thrown will be caught and
    properly handled.
*/

template<class Parent>
Parent* factory (FILE* fptr)
{
  long current = ftell (fptr);

  std::vector< Reference::To<Parent> > candidates;

  Parent::children (candidates);

  for (unsigned i=0; i < candidates.size(); i++) {

    if (fseek (fptr, current, SEEK_SET) < 0)
      throw Error (FailedSys, "factory (FILE*)", "fseek");

    try {
      candidates[i]->load (fptr);
      return candidates[i].release();
    }
    catch (Error& error) { }

  }
  throw Error (InvalidParam, "factory (FILE*)",
	       "no child recognizes contents of file");
}

template<class Parent>
Parent* factory (FILE* fptr, size_t nbytes)
{
  FILE* temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "factory (FILE*, size_t)", "tmpfile");

  ::copy (fptr, temp, nbytes);

  Parent* model = 0;

  try {
    rewind (temp);
    model = factory<Parent> (temp);
  }
  catch (Error& error) { }

  fclose (temp);
  return model;
}

template<class Parent>
Parent* factory (const std::string& filename)
{
  FILE* temp = fopen (filename.c_str(), "r");

  if (!temp)
    throw Error (FailedSys, "factory (std::string&)",
		 "fopen (%s)", filename.c_str());

  try {
    Parent* model = factory<Parent> (temp);
    fclose (temp);
    return model;
  }
  catch (Error& error) {
    fclose (temp);
    throw error += "factory (std::string&)";
  }
}

//
// the following functions are not really factories, but were inspired
// by the ideas implemented above
//
template<class Any>
Any* load (const std::string& filename)
{
  FILE* temp = fopen (filename.c_str(), "r");
  if (!temp)
    throw Error (FailedSys, "Any* load (std::string&)",
		 "fopen (%s)", filename.c_str());

  try {
    Any* any = new Any;
    any -> load (temp);
    fclose (temp);
    return any;
  }
  catch (Error& error) {
    fclose (temp);
    throw error += "Any* load (std::string&)";
  }
}

template<class Any>
size_t nbytes (const Any* any)
{
  FILE* temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "nbytes (Any*)", "tmpfile");

  size_t nbytes = 0;
  try {
    any->unload (temp);
    nbytes = ftell(temp);
  }
  catch (Error& error)
    {
      fclose (temp);
      throw error += "nbytes (Any*)";
    }

  fclose (temp);
  return nbytes;
}
