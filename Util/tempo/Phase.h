#ifndef __POLYCO_PHASE_H
#define __POLYCO_PHASE_H

#include "environ.h"
#include "MJD.h"

#include <string>
#include <math.h>

#ifdef HAVE_MPI
#include <mpi.h>
#endif

class Phase {

 private:
  int64 turns;
  double fturns;

  void settle ();

 public:

  //! The largest number to be considered "close enough" to zero
  static double rounding_threshold;

  static const Phase zero;

  ~Phase(){}
  Phase (int64 tns, double ftns);
  Phase (double turns=0);

  Phase& operator= (const Phase &in_Phase);
  Phase& operator= (double turns);

  friend Phase operator + (const Phase &, double); 
  friend Phase operator - (const Phase &, double); 
  friend Phase operator + (const Phase &, const Phase &);
  friend Phase operator - (const Phase &, const Phase &);
  friend Phase operator - (const Phase &);

  // some may disagree with this usage of the operator
  friend MJD operator * (const Phase &, double period);
  friend MJD operator / (const Phase &, double frequency);

  Phase& operator += (const Phase &);
  Phase& operator -= (const Phase &);
  Phase& operator += (double);
  Phase& operator -= (double);
  Phase& operator += (int);
  Phase& operator -= (int);
  // increment/decrement by one turn
  Phase& operator ++ ();
  Phase& operator -- ();

  friend int operator > (const Phase &, const Phase &) ;
  friend int operator < (const Phase &, const Phase &) ;
  friend int operator >= (const Phase &, const Phase &);
  friend int operator <= (const Phase &, const Phase &);
  friend int operator == (const Phase &, const Phase &);
  friend int operator != (const Phase &, const Phase &);

  Phase Ceil  ();
  Phase Floor ();
  Phase Rint  ();

  double in_turns() const;
  int64  intturns() const;
  double fracturns() const;
  std::string strprint(int precision) const;

#ifdef HAVE_MPI
  friend int mpiPack_size (const Phase&, MPI_Comm comm, int* size);
  friend int mpiPack   (const Phase&, void* outbuf, int outcount, 
			int* position, MPI_Comm comm);
  friend int mpiUnpack (void* inbuf, int insize, int* position, 
			Phase*, MPI_Comm comm);
#endif

};

inline std::ostream& operator<< (std::ostream& ostr, const Phase& sz) 
{ return ostr << sz.strprint(8); }

#endif
