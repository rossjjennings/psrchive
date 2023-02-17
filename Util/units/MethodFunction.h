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

template<class C, class T, class Get, class Arg>
class MethodGetFunction
{
  Get get;
  Arg arg;

 public:

  typedef C* first_argument_type;
  typedef T result_type;

  MethodGetFunction (Get _get, Arg _arg)
  {
    get = _get;
    arg = _arg;
  }

  T operator () (C* ptr)
  {
    return (ptr->*get) (arg);
  }

  const T operator () (const C* ptr) const
  {
    return (const_cast<C*>(ptr)->*get) (arg);
  }
};

template<class C, class T, class Arg>
MethodGetFunction<C, T, T (C::*)(Arg), Arg>
method_function (T (C::*get)(Arg), Arg arg)
{
  return MethodGetFunction<C, T, T (C::*)(Arg), Arg> (get, arg);
}

template<class C, class T, class Arg>
MethodGetFunction<C, T, T (C::*)(const Arg&), Arg>
method_function (T (C::*get)(const Arg&), Arg arg)
{
  return MethodGetFunction<C, T, T (C::*)(const Arg&), Arg> (get, arg);
}

template<class C, class T, class Arg>
MethodGetFunction<C, T, T (C::*)(const Arg&) const, Arg>
method_function (T (C::*get)(const Arg&) const, Arg arg)
{
  return MethodGetFunction<C, T, T (C::*)(const Arg&) const, Arg> (get, arg);
}

template<class C, class T, class Set, class Arg, class Value>
class MethodSetFunction
{
  Set set;
  Arg arg;

 public:

  typedef C* first_argument_type;
  typedef Value second_argument_type;

  MethodSetFunction (Set _set, Arg _arg)
  {
    set = _set;
    arg = _arg;
  }

  void operator () (C* ptr, const Value& value)
  {
    (ptr->*set) (arg, value);
  }
};

template<class C, class T, class Arg>
MethodSetFunction<C, T, void (C::*)(Arg, const T&), Arg, T>
method_function (void (C::*set)(Arg, const T&), const Arg& arg)
{
  return MethodSetFunction<C, T, void (C::*)(Arg, const T&), Arg, T> (set, arg);
}

template<class C, class T, class Arg>
MethodSetFunction<C, T, void (C::*)(Arg, T), Arg, T>
method_function (void (C::*set)(Arg, T), const Arg& arg)
{
  return MethodSetFunction<C, T, void (C::*)(Arg, T), Arg, T> (set, arg);
}

template<class C, class T, class Arg>
MethodSetFunction<C, T, void (C::*)(const Arg&, const T&), Arg, T>
method_function (void (C::*set)(const Arg&, const T&), const Arg& arg)
{
  return MethodSetFunction<C, T, void (C::*)(const Arg&, const T&), Arg, T> (set, arg);
}

template<class C, class T, class Arg>
MethodSetFunction<C, T, void (C::*)(const Arg&, T), Arg, T>
method_function (void (C::*set)(const Arg&, T), const Arg& arg)
{
  return MethodSetFunction<C, T, void (C::*)(const Arg&, T), Arg, T> (set, arg);
}

#endif

