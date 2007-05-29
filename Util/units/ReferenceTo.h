//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/ReferenceTo.h,v $
   $Revision: 1.5 $
   $Date: 2007/05/29 00:50:08 $
   $Author: straten $ */

#ifndef __ReferenceTo_h
#define __ReferenceTo_h

#include "ReferenceAble.h"
#include <typeinfo>
#include <string>

namespace Reference {

  //! A smart pointer that gets invalidated when its target is destroyed
  template<class Type, bool active = true> class To {

  public:

    //! Returns true if reference is null
    bool operator ! () const;
    
    //! Returns true if reference is other than null
    operator bool () const;
    
    //! Default constructor
    To (Type* ptr = 0);
    
    //! Copy constructor
    To (const To&);
    
    //! Destructor
    ~To ();
    
    //! Assignment operator
    To& operator = (const To&);

    //! Assignment operator
    To& operator = (Type *);
    
    //! Object dereferencing operator
    Type& operator * () const { return *get(); }
    
    //! Member dereferencing operator
    Type* operator -> () const { return get(); }
    
    //! Cast to Type* operator
    operator Type* () const { return get(); }

    //! Return the pointer
    Type* get () const;
    
    //! Return the pointer and unhook without deleting the object
    Type* release ();
    
    //! Return pointer without testing for validity
    const Type* ptr () const;

    //! Return pointer without testing for validity
    Type* ptr ();

    //! Return the name of the object, as returned by typeid
    std::string name () const;

  private:

    //! Set to an existing handle
    void handle (const Able::Handle*);

    //! Set to an existing instance
    void handle (const Able*);

    //! Unhandle
    void unhandle (bool auto_delete = true);

    //! The handle to the object
    Able::Handle* the_handle;
    
  };
}

template<class Type, bool active>
void Reference::To<Type,active>::unhandle (bool auto_delete)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::unhandle handle=" << the_handle << endl;
#endif

  if (!the_handle)
    return;

  if (the_handle->count == 1 && the_handle->pointer)
    // the handle is about to be deleted, ensure that Able knows it
    the_handle->pointer->__reference_handle = 0;

  if (active && the_handle->pointer)
    // decrease the active reference count
    the_handle->pointer->__dereference (auto_delete);

  // decrease the total reference count
  the_handle->count --;

  // delete the handle
  if (the_handle->count == 0)
    delete the_handle;

#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::unhandle count=" << the_handle->count << endl;
#endif

  the_handle = 0;
}

template<class Type, bool active>
void Reference::To<Type,active>::handle (const Able::Handle* _handle)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::handle handle=" << _handle << endl;
#endif

  the_handle = const_cast<Able::Handle*>( _handle );

  if (!the_handle)
    return;

  if (the_handle->pointer)
    handle (the_handle->pointer);
  else // still have to keep the handle alive
    the_handle->count ++;

#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::handle count=" << the_handle->count << endl;
#endif
}

template<class Type, bool active>
void Reference::To<Type,active>::handle (const Able* pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::handle Able*="<< pointer << endl;
#endif

  if (!pointer)
    the_handle = 0;

  else {
    the_handle = pointer->__reference (active);
    the_handle->count ++;

#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::handle Able* count=" << the_handle->count << endl;
#endif
  }

}

template<class Type, bool active>
std::string Reference::To<Type,active>::name () const
{
  return typeid(Type).name();
}

template<class Type, bool active>
Reference::To<Type,active>::To (Type* ref_pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::To (Type*="<< ref_pointer <<")"<< endl;
#endif

  handle (ref_pointer);
}

template<class Type, bool active>
Reference::To<Type,active>::~To ()
{ 
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::~To" << endl;
#endif

  unhandle ();
}

template<class Type, bool active>
bool Reference::To<Type,active>::operator ! () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator !" << endl;
#endif

  return !the_handle || the_handle->pointer == 0;
}

template<class Type, bool active>
Reference::To<Type,active>::operator bool () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator bool" << endl;
#endif

  return the_handle && the_handle->pointer;
}

template<class Type, bool active>
Reference::To<Type,active>::To (const To& another_reference)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::To (To<Type>)" << endl;
#endif

  handle (another_reference.the_handle);
}


// operator =
template<class Type, bool active>
Reference::To<Type,active>&
Reference::To<Type,active>::operator = (const To& oref)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator = (To<Type>)" << endl;
#endif

  if (the_handle == oref.the_handle)
    return *this;

  unhandle ();

  handle (oref.the_handle);

  return *this;
}


template<class Type, bool active>
Reference::To<Type,active>& 
Reference::To<Type,active>::operator = (Type* ref_pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator = (Type*=" << ref_pointer <<")"<<endl;
#endif

  if (the_handle && the_handle->pointer == ref_pointer)
    return *this;

  unhandle ();

  handle (ref_pointer);

  return *this;
}

template<class Type, bool active>
Type* Reference::To<Type,active>::get () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::get" << endl;
#endif

  if (!the_handle || the_handle->pointer == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::get");

  return reinterpret_cast<Type*>( the_handle->pointer );
}


template<class Type, bool active>
Type* Reference::To<Type,active>::release ()
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::release" << endl;
#endif

  if (!the_handle || the_handle->pointer == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::release");

  Type* copy = reinterpret_cast<Type*>( the_handle->pointer );

  unhandle (false);

  return copy;
}

    //! Return pointer without testing for validity
template<class Type, bool active>
const Type* Reference::To<Type,active>::ptr () const
{
  if (the_handle)
    return reinterpret_cast<const Type*>( the_handle->pointer );
  else
    return 0;
}

    //! Return pointer without testing for validity
template<class Type, bool active>
Type* Reference::To<Type,active>::ptr ()
{
  if (the_handle)
    return reinterpret_cast<Type*>( the_handle->pointer );
  else
    return 0;
}

template<class Type, bool active>
void swap (Reference::To<Type,active>& ref1, Reference::To<Type,active>& ref2)
{
#ifdef _DEBUG
  cerr << "swap (Reference::To<Type>, ditto)" << endl;
#endif

  Type* ref1_ptr = ref1.release();
  Type* ref2_ptr = ref2.release();

  ref1 = ref2_ptr;
  ref2 = ref1_ptr;
}

//! return true if two Reference::To objects refer to the same instance
template<class Type1, bool active1, class Type2, bool active2>
bool operator == (const Reference::To<Type1,active1>& ref1,
                  const Reference::To<Type2,active2>& ref2)
{
#ifdef _DEBUG
  cerr << "operator == (Reference::To<Type>&, Reference::To<Type2>&)" << endl;
#endif

  return ref1.ptr() == ref2.ptr();
}

//! return false if two Reference::To objects refer to the same instance
template<class Type1, bool active1, class Type2, bool active2>
bool operator != (const Reference::To<Type1,active1>& ref1,
                  const Reference::To<Type2,active2>& ref2)
{
#ifdef _DEBUG
  cerr << "operator != (Reference::To<Type>&, Reference::To<Type2>&)" << endl;
#endif

  return ref1.ptr() != ref2.ptr();
}


//! return true if Reference::To refers to instance
template<class Type, bool active, class Type2>
bool operator == (const Reference::To<Type,active>& ref, const Type2* instance)
{
#ifdef _DEBUG
  cerr << "operator == (Reference::To<Type>&, Type*)" << endl;
#endif

  return ref.ptr() == instance;
}

//! return true if Reference::To refers to instance
template<class Type, bool active, class Type2>
bool operator == (const Type2* instance, const Reference::To<Type,active>& ref)
{
#ifdef _DEBUG
  cerr << "operator == (T2*, Reference::To<T1>&)" << endl;
#endif

  return ref.ptr() == instance;
}



#endif // #ifndef __ReferenceTo_h

