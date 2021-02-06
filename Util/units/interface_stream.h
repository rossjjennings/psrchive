//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/interface_stream.h

#ifndef __interface_stream_h
#define __interface_stream_h

//! insertion operator outputs object state as defined by its text interface
/*!
 */
template<class T>
std::ostream& interface_insertion (std::ostream& ostr, const T* e) try
{
  if (!e)
    return ostr;
 
  Reference::To<TextInterface::Parser> interface 
    = const_cast<T*>(e)->get_interface();

  ostr << interface->get_interface_name ();

  if (interface->get_nvalue() == 0)
    return ostr;

  ostr << ":{";

  for (unsigned i=0; i<interface->get_nvalue(); i++)
  {
    if (i > 0)
      ostr << ",";

    ostr << interface->get_name(i) << "=" << interface->get_value(i);
  }

  return ostr << "}";
}
catch (Error& error)
{
  ostr.setstate (std::ios::failbit);
  return ostr;
}

template<class T>
std::ostream&
interface_insertion (std::ostream& ostr, const Reference::To<T>& e)
{
  return interface_insertion (ostr, e.get());
}

//! extraction operator creates a new object using its text-based factory
/*!
 */

template<typename T>
std::istream& interface_extraction (std::istream& istr, T* &e) try
{
  std::string parse;
  istr >> parse;

  e = T::factory (parse);
  return istr;
}
catch (Error& error)
{
  if (error.get_code() == HelpMessage)
    throw error;

  istr.setstate (std::ios::failbit);
  return istr;
}

template<typename T>
std::istream& interface_extraction (std::istream& istr, Reference::To<T>& e)
{
  T* ptr = 0;
  interface_extraction (istr, ptr);
  e = ptr;
  return istr;
}

//
//
//

namespace Reference
{
  template<typename T>
  std::ostream& operator<< (std::ostream& ostr, const To<T>& e)
  {
    return interface_insertion (ostr, e);
  }
  
  template<typename T>
  std::istream& operator >> (std::istream& istr, To<T>& e)
  {
    return interface_extraction (istr, e);
  }
}

#endif
