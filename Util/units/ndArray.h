//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <inttypes.h>
#include <vector>

// psrchive/Util/units/ndArray.h

#ifndef __ndArray_H
#define __ndArray_H

//! An N-dimensional array of elements of type T, defined recursively
/*! This template implements row-major order calculation of offsets using
  C-style array index notation; e.g. data[i][j][k] */

template<unsigned N, typename T> class ndArray
{
  ndArray<N-1,T> next;
  uint64_t ndat;

  uint64_t multiplicity;
  mutable uint64_t offset;

  friend class ndArray<N+1,T>;

public:

  ndArray () { offset = 0; multiplicity = 1; }

  //! Set the dimension along this index
  ndArray<N-1,T>& operator* (uint64_t sz)
  { ndat = sz; next.multiplicity = multiplicity * ndat; return next; }

  const ndArray<N-1,T>& operator[] (uint64_t idat) const
  { next.offset = offset + idat * next.stride(); return next; }

  ndArray<N-1,T>& operator[] (uint64_t idat)
  { next.offset = offset + idat * next.stride(); return next; }

  uint64_t stride () const { return ndat * next.stride(); }
  uint64_t size () const { return ndat; }
};

template<typename T>
class ndArray<1,T>
{
  uint64_t ndat;

  uint64_t multiplicity;
  mutable uint64_t offset;

  friend class ndArray<2,T>;

  std::vector<T> data;

public:
  ndArray () { offset = 0; multiplicity = 1; }

  void operator* (uint64_t sz) { ndat = sz; data.resize (ndat * multiplicity); }
  const T& operator[] (uint64_t idat) const { return data[offset + idat]; }
  T& operator[] (uint64_t idat) { return data[offset + idat]; }

  uint64_t stride () const { return ndat; }
  uint64_t size () const { return ndat; }
};

#endif

