//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/MethodFunction.h

#ifndef __MethodFunction_h
#define __MethodFunction_h

template<class C, class Type, class Get, class Arg>
class MethodFunction
{
  Get get;
  Arg arg;

 public:

  MethodFunction (Get _get, Arg _arg)
  {
    get = _get;
    arg = _arg;
  }

  Type operator () (C* ptr)
  {
    return (ptr->*get) (arg);
  }

  const Type operator () (const C* ptr) const
  {
    return (const_cast<C*>(ptr)->*get) (arg);
  }
};

template<class C, class Type, class Arg>
MethodFunction<C, Type, Type (C::*)(Arg), Arg> 
method_function (Type (C::*get)(Arg), Arg arg)
{
  return MethodFunction<C, Type, Type (C::*)(Arg), Arg> (get, arg);
}

template<class C, class Type, class Arg>
MethodFunction<C, Type, Type (C::*)(const Arg&), Arg>
method_function (Type (C::*get)(const Arg&), Arg arg)
{
  return MethodFunction<C, Type, Type (C::*)(const Arg&), Arg> (get, arg);
}

template<class C, class Type, class Arg>
MethodFunction<C, Type, Type (C::*)(const Arg&) const, Arg>
method_function (Type (C::*get)(const Arg&) const, Arg arg)
{
  return MethodFunction<C, Type, Type (C::*)(const Arg&) const, Arg> (get, arg);
}

#endif
