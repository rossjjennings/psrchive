//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/units/ReferenceTo.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 09:31:49 $
   $Author: straten $ */

#ifndef __ReferenceTo_h
#define __ReferenceTo_h

#include "Reference.h"
#include <typeinfo>
#include <string>

namespace Reference {

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
    Type& operator * () const;
    
    //! Member dereferencing operator
    Type* operator -> () const { return this -> operator Type* (); }
    
    //! Cast to Type* operator
    operator Type* () const;

    //! Return the pointer
    Type* get () const;
    
    //! Return the pointer and unhook without deleting the object
    Type* release ();
    
    //! Return pointer without testing for validity
    const Type* ptr () const { return obj_reference; }

    //! Return pointer without testing for validity
    Type* ptr () { return obj_reference; }

    //! Return the name of the object, as returned by typeid
    std::string name () const;

  private:

    void unhookRef ();

    void hookRef ();

    //! The reference to the object
    Type* obj_reference;
    
  };
}

template<class Type, bool active>
std::string Reference::To<Type,active>::name () const
{
  if (obj_reference)
    return typeid(obj_reference).name();
  else
    return typeid(Type).name();
}

template<class Type, bool active>
Reference::To<Type,active>::To (Type* ref_pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::To (Type*="<< ref_pointer <<")"<< endl;
#endif

  obj_reference = ref_pointer;
  hookRef();
}

template<class Type, bool active>
Reference::To<Type,active>::~To ()
{ 
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::~To" << endl;
#endif

  unhookRef();
}

template<class Type, bool active>
bool Reference::To<Type,active>::operator ! () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator !" << endl;
#endif

  return obj_reference == 0;
}

template<class Type, bool active>
Reference::To<Type,active>::operator bool () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator bool" << endl;
#endif

  return obj_reference != 0;
}

template<class Type, bool active>
Reference::To<Type,active>::To (const To& another_reference)
{
  obj_reference = another_reference.obj_reference;

#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::To (To<Type>->" << obj_reference << ")" << endl;
#endif

  hookRef();
}


// operator =
template<class Type, bool active>
Reference::To<Type,active>&
Reference::To<Type,active>::operator = (const To& oref)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator = (To<Type>->" 
       << oref.obj_reference << ")" << endl;
#endif

  if (obj_reference == oref.obj_reference)
    return *this;

  unhookRef();
  obj_reference = oref.obj_reference;
  hookRef();
  return *this;
}


template<class Type, bool active>
Reference::To<Type,active>& 
Reference::To<Type,active>::operator = (Type* ref_pointer)
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator = (Type*=" << ref_pointer <<")"<<endl;
#endif

  if (obj_reference == ref_pointer)
    return *this;

  unhookRef();
  obj_reference = ref_pointer;
  hookRef();
  return *this;
}

// de-To operators
template<class Type, bool active>
Type& Reference::To<Type,active>::operator * () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator *" << endl;
#endif

  if (obj_reference == 0) {
    throw Error (InvalidPointer, "Reference::To<"+name()+">::operator *");
  }

  return *obj_reference;
}

template<class Type, bool active>
Reference::To<Type,active>::operator Type* () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::operator Type*" << endl;
#endif

  if (obj_reference == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::operator Type*");

  return obj_reference;
}

template<class Type, bool active>
Type* Reference::To<Type,active>::get () const
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::get" << endl;
#endif

  if (obj_reference == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::get");
  return obj_reference;
}


template<class Type, bool active>
Type* Reference::To<Type,active>::release ()
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::release" << endl;
#endif

  if (obj_reference == 0)
    throw Error (InvalidPointer, "Reference::To<"+name()+">::release");

  Type* copy = obj_reference;

  obj_reference->__remove_reference (active, (Able**)&obj_reference, false);
  obj_reference = 0;

  return copy;
}

template<class Type, bool active>
void Reference::To<Type,active>::unhookRef ()
{
#ifdef _DEBUG
  cerr<<"Reference::To<"+name()+">::unhookRef obj_reference="<< obj_reference <<endl;
#endif

  if (!obj_reference)
    return;

  obj_reference->__remove_reference (active, (Able**)&obj_reference);

  obj_reference = 0;
}

template<class Type, bool active>
void Reference::To<Type,active>::hookRef ()
{
#ifdef _DEBUG
  cerr << "Reference::To<"+name()+">::hookRef obj_reference="<< obj_reference <<endl;
#endif

  if (!obj_reference)
    return;

  obj_reference->__add_reference (active, (Able**)&obj_reference);
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

