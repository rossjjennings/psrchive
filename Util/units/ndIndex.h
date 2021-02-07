//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/ndIndex.h

#ifndef __ndIndex_H
#define __ndIndex_H

#include <inttypes.h>

template<unsigned N> class ndIndex
{
  ndIndex<N-1> next;
  uint64_t ndat;
  mutable uint64_t offset;

  friend class ndIndex<N+1>;

public:
  ndIndex () { offset = 0; }
  //! Set the dimension along this index
  ndIndex<N-1>& operator* (uint64_t sz) { ndat = sz; return next; }

  const ndIndex<N-1>& operator[] (uint64_t idat) const 
  { next.offset = offset + idat * next.stride(); return next; }

  uint64_t stride () const { return ndat * next.stride(); }
};

template<>
class ndIndex<1>
{
  uint64_t ndat;
  mutable uint64_t offset;

  friend class ndIndex<2>;

public:
  ndIndex () { offset = 0; }
  void operator* (uint64_t sz) { ndat = sz; }
  uint64_t operator[] (uint64_t idat) const { return offset + idat; }
  uint64_t stride () const { return ndat; }
};

#endif

