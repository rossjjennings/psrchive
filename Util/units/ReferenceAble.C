#include <iostream>
#include <vector>

#include "ReferenceAble.h"

// #define _DEBUG 1

#if _DEBUG
std::vector<Reference::Able*> Reference::Able::null_ables(){
  std::vector<Able*> nullie;
  return nullie;
}

int Reference::Able::instantiation_count = 0;
int Reference::Able::full_count = 0;
std::vector<Reference::Able*> Reference::Able::ables = Reference::Able::null_ables();
#endif

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able ()
{
#if _DEBUG
  instantiation_count++;
  full_count++;
  ables.push_back(this);
#endif
  __reference_count = 0;
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::Able (const Able&)
{
#if _DEBUG
  instantiation_count++;
  full_count++;
  ables.push_back(this);
#endif
  __reference_count = 0;
}

//////////////////////////////////////////////////////////////////////////
//
Reference::Able::~Able ()
{ 
#if _DEBUG
  instantiation_count--;
  for( unsigned iable=0; iable<ables.size(); iable++){
    if( ables[iable]==this ){
      ables.erase(ables.begin()+iable);
      break;
    }
  }

  cerr << "Reference::Able::~Able with " << __reference_list.size()
       << " references" <<endl;
#endif

  std::vector<Able**>::iterator it = __reference_list.begin();
  while (it != __reference_list.end()) {
    *(*it) = 0;
    it ++;
  }

#ifdef _DEBUG
  cerr << "Reference::Able::~Able exit" <<endl;
#endif

}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const T> */
void Reference::Able::__add_reference (bool active, Able** ref_address) const
{

#ifdef _DEBUG
  cerr << "Reference::Able::__add_reference ptr=" << ref_address 
       << " this=" << this << " active=" << active << endl;
#endif

  // function is declared const, but __reference_list must be modified
  Able* thiz = const_cast<Able*> (this);

  thiz->__is_on_heap();
  thiz->__reference_list.push_back (ref_address);

  if (active)
    thiz->__reference_count ++;

#ifdef _DEBUG
  cerr << "Reference::Able::__add_reference count="
       << __reference_count << endl;
#endif

}

//////////////////////////////////////////////////////////////////////////
//
/*! Declared const in order to enable Reference::To<const Klass> */
void Reference::Able::__remove_reference (bool active, Able** ref_address,
					  bool auto_delete) const
{ 

#ifdef _DEBUG
  cerr << "Reference::Able::__remove_reference=" << ref_address 
       << " this=" << this << " active=" << active << endl;
#endif

  // function is declared const, but __reference_list must be modified
  Able* thiz = const_cast<Able*> (this);

  std::vector<Able**>::iterator it = thiz->__reference_list.begin();
  while (it != thiz->__reference_list.end())  {
    if (*it == ref_address) {
      thiz->__reference_list.erase(it);
      break;
    }
    else
      it ++;
  }

  if (active)
    thiz->__reference_count --;

#ifdef _DEBUG
  cerr << "Reference::Able::__remove_reference count="
       << __reference_count << endl;
#endif

  // delete when reference count reaches zero and instance is on heap
  if (active && auto_delete && __is_on_heap() && __reference_count == 0) {

#ifdef _DEBUG
  cerr << "Reference::Able::__remove_reference delete this" << endl;
#endif

    delete this;

  }

}
