//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/ReferenceAble.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:55 $
   $Author: straten $ */

#ifndef __ReferenceAble_h
#define __ReferenceAble_h

#include <vector>
#include <string>

#include "HeapTracked.h"

namespace Reference {

  //! Method verbosity flag
  extern bool verbose;

  //! Manages Reference::To references to the instance
  /*! Combined with the Reference::To template class, this class may
    be used to eliminate both memory leaks and dangling references.
    Simply inherit this class as follows:
 
    class myKlass : public Reference::Able  {
    ...
    };

    myKlass can now be managed using a Reference::To<myKlass>.
  */
  class Able : public HeapTracked {

    template<class Type, bool active> friend class To;

  public:

#if _DEBUG
    static std::vector<Able*> null_ables();

    //! Counts how many Reference::Able's are in existence
    static int instantiation_count;
    //! How many Able's have ever been in existence
    static int full_count;
    //! Stores pointers to every Able that has ever been in existence
    static std::vector<Able*> ables;
#endif

    //! Default constructor
    Able ();

    //! Copy constructor
    /*! Disables the compiler-generated copy of __reference_list. */
    Able (const Able&);

    //! Assignment operator
    /*! Disables the compiler-generated copy of __reference_list. */
    Able& operator = (const Able&) { return *this; }

    //! Destructor
    /*! Invalidates all Reference::To references to this instance. */
    virtual ~Able();
    
    //! Returns the number of references there are
    unsigned get_reference_count() const { return __reference_count; }

  protected:

    //! Add the address of a reference to this object
    void __add_reference (bool active, Able** ref_address) const;

    //! Remove the address of a reference to this object
    void __remove_reference (bool active, Able** ref_address,
			     bool auto_delete = true) const;

  private:

    //! Addresses of all references to this instance
    std::vector <Able**> __reference_list;

    //! Count of active references to this instance
    unsigned __reference_count;
  };

}

#endif // #ifndef __ReferenceAble_h



