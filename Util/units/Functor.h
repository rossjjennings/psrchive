//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Functor.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/23 12:14:35 $
   $Author: straten $ */

#ifndef __Swinburne_Functor_h
#define __Swinburne_Functor_h

#include "Reference.h"

// an empty type used to allow one template name
struct __functor_empty {};

//! Implements an adaptable function object in compliance with the STL
/*!  The Functor template class implements a an adaptable function
  object interface using the excellent syntax found in the Tiny
  Template Library, http://www.codeproject.com/cpp/TTLFunction.asp.
  Functors are instantiated as in the following examples:

  // one integer argument, returns double
  Functor< double(int) > f1;

  // two string arguments, returns boolean
  Functor< bool(string, string) > f2;

  They are then called as a normal function would be called, e.g.

  string s1, s2;
  if( f2(s2, s2) )
    [blah blah blah]

*/
template<typename R, typename T1=__functor_empty, typename T2=__functor_empty>
class Functor;

// ///////////////////////////////////////////////////////////////////////////
//
// Generator Functor specialization
//
// ///////////////////////////////////////////////////////////////////////////

//! Template specialization of generators (functions with no arguments)
template< typename R, typename T1, typename T2 > 
class Functor< R (), T1, T2 >
{
 public:

  typedef R result_type;

  //
  // the interface
  //

  //! Generator function call
  R operator() () { return functor->call(); }

  //! Construct from a class instance and generator method
  template<class C, typename M> Functor (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Construct from a generator function
  template<typename F> Functor (F function)
    { functor = new Function<F> (function); }

  //! Set equal to a class instance and generator method
  template<class C, typename M> void set (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Set equal to a generator function
  template<typename F> void set (F function)
    { functor = new Function<F> (function); }

  //
  // the implementation
  //

  //! Pure virtual base class of generator functor implementations
  class Base : public Reference::Able {
  public:  
    //! The function call
    virtual R call () = 0;

    //! Return true if valid (able to be called)
    virtual bool is_valid () = 0;
  };
  
  //! Implementation calls generator function F
  template<typename F> class Function : public Base {
  public:
      
    //! Construct from a pointer to a function
    Function (F _function)
      { function = _function; }
    
    //! Call the function
    R call ()
      { return R( (*function) () ); }
      
    //! Return true if valid (able to be called)
    bool is_valid () { return function != 0; }

  protected:
    
    //! The function to be called
    F function;
    
  };

  //! Implementation calls generator method M of class C
  template<class C, typename M> class Method : public Base {
  public:
      
    //! Construct from a pointer to a class instance and method
    Method (C* _instance, M _method)
      { instance = _instance; method = _method; }
    
    //! Call the method through the class instance
    R call ()
      { return R( (instance->*method) () ); }
      
    //! Return true if the instance and method match
    bool matches (const C* _instance, M _method)
      { return (instance && instance == _instance && method == _method); }
    
    //! Return true if valid (able to be called)
    bool is_valid () { return instance && method != 0; }

  protected:
    
    //! The instance of the class
    Reference::To<C,false> instance;
    
    //! The method to be called
    M method;
    
  };
  
 protected: 
  
  //! The implementation
  Reference::To<Base> functor;

};

// ///////////////////////////////////////////////////////////////////////////
//
// Unary Functor specialization
//
// ///////////////////////////////////////////////////////////////////////////

//! Template specialization of unary functions
template< typename R, typename T1, typename T2 > 
class Functor< R (T1), T2 >
{
 public:

  typedef T1 argument_type;
  typedef R result_type;

  //
  // the interface
  //

  //! Unary function call
  R operator() (const T1& p1) { return functor->call(p1); }

  //! Construct from a class instance and unary method
  template<class C, typename M> Functor (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Construct from a unary function
  template<typename F> Functor (F function)
    { functor = new Function<F> (function); }

  //! Set equal to a class instance and unary method
  template<class C, typename M> void set (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Set equal to a unary function
  template<typename F> void set (F function)
    { functor = new Function<F> (function); }

  //
  // the implementation
  //

  //! Pure virtual base class of unary functor implementations
  class Base : public Reference::Able {
  public:  
    //! The function call
    virtual R call (const T1& p1) = 0;

    //! Return true if valid (able to be called)
    virtual bool is_valid () = 0;
  };
  
  //! Implementation calls unary function F
  template<typename F> class Function : public Base {
  public:
      
    //! Construct from a pointer to a function
    Function (F _function)
      { function = _function; }
    
    //! Call the function
    R call (const T1& p1)
      { return R( (*function) (p1) ); }
      
    //! Return true if valid (able to be called)
    bool is_valid () { return function != 0; }

  protected:
    
    //! The function to be called
    F function;
    
  };

  //! Implementation calls unary method M of class C
  template<class C, typename M> class Method : public Base {
  public:
      
    //! Construct from a pointer to a class instance and method
    Method (C* _instance, M _method)
      { instance = _instance; method = _method; }
    
    //! Call the method through the class instance
    R call (const T1& p1) try
    { return R( (instance->*method) (p1) ); }
    catch (Error& error) { throw error += "Functor<R(T)>::Method::call"; }

    //! Return true if valid (able to be called)
    bool is_valid () { return instance && method != 0; }

    //! Return true if the instance and method match
    bool matches (const C* _instance, M _method)
      { return (instance && instance == _instance && method == _method); }

  protected:
    
    //! The instance of the class
    Reference::To<C,false> instance;
    
    //! The method to be called
    M method;
    
  };
  
 protected: 
  
  //! The implementation
  Reference::To<Base> functor;
  
};


// ///////////////////////////////////////////////////////////////////////////
//
// Binary Functor specialization
//
// ///////////////////////////////////////////////////////////////////////////

//! Template specialization of binary functions
template< typename R, typename T1, typename T2 > 
class Functor< R (T1, T2) >
{
 public:

  typedef T1 first_argument_type;
  typedef T2 second_argument_type;
  typedef R result_type;

  //
  // the interface
  //

  //! Binary function call
  R operator() (const T1& p1, const T2& p2) { return functor->call(p1,p2); }
  
  //! Construct from a class instance and binary method
  template<class C, typename M> Functor (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Construct from a binary function
  template<typename F> Functor (F function)
    { functor = new Function<F> (function); }

  //! Set equal to a class instance and binary method
  template<class C, typename M>  void set (C* instance, M method)
    { functor = new Method<C, M> (instance, method); }

  //! Set equal to a binary function
  template<typename F> void set (F function)
    { functor = new Function<F> (function); }

  //
  // the implementation
  //

  //! Pure virtual base class of binary functor implementations
  class Base : public Reference::Able {
  public:  
    //! The function call
    virtual R call (const T1& p1, const T2& p2) = 0;

    //! Return true if valid (able to be called)
    virtual bool is_valid () = 0;
  };
  
  //! Implementation calls binary function F
  template<typename F> class Function : public Base {
  public:
      
    //! Construct from a pointer to a function
    Function (F _function)
      { function = _function; }
    
    //! Call the function
    R call (const T1& p1, const T2& p2)
      { return R( (*function) (p1, p2) ); }
      
    //! Return true if valid (able to be called)
    bool is_valid () { return function != 0; }

  protected:
    
    //! The function to be called
    F function;
    
  };

  //! Implementation calls binary method M of class C
  template<class C, typename M> class Method : public Base {
  public:
      
    //! Construct from a pointer to a class instance and method
    Method (C* _instance, M _method)
      { instance = _instance; method = _method; }
    
    //! Call the method through the class instance
    R call (const T1& p1, const T2& p2)
      { return R( (instance->*method) (p1, p2) ); }
      
    //! Return true if valid (able to be called)
    bool is_valid () { return instance && method != 0; }

    //! Return true if the instance and method match
    bool matches (const C* _instance, M _method)
      { return (instance && instance == _instance && method == _method); }
    
  protected:
    
    //! The instance of the class
    Reference::To<C,false> instance;
    
    //! The method to be called
    M method;
    
  };
  
 protected: 
  
  //! The implementation
  Reference::To<Base> functor;
  
};


#endif
