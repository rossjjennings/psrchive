#ifndef __PHASE_H
#define __PHASE_H

#include <math.h>
#include "environ.h"
#ifdef MPI
#include "mpi.h"
#endif
#include <string>

#include "psr_cpp.h"
#include "MJD.h"

class Phase {

 private:
  int64 turns;
  double fturns;

  void settle ();

 public:
  Phase();
  ~Phase(){turns=0; fturns=0.0;};
  Phase (int64 tns, double ftns);
  Phase (double turns);

  Phase& operator= (const Phase &in_Phase);
  Phase& operator= (double turns);

  friend Phase operator + (const Phase &, double); 
  friend Phase operator - (const Phase &, double); 
  friend Phase operator + (const Phase &, const Phase &);
  friend Phase operator - (const Phase &, const Phase &);

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

  double in_turns() const;
  int64  intturns() const;
  double fracturns() const;
  string strprint(int precision) const;

#ifdef MPI
  friend int mpiPack_size (const Phase&, MPI_Comm comm, int* size);
  friend int mpiPack   (const Phase&, void* outbuf, int outcount, 
			int* position, MPI_Comm comm);
  friend int mpiUnpack (void* inbuf, int insize, int* position, 
			Phase*, MPI_Comm comm);
#endif

};

inline ostream& operator<< (ostream& ostr, const Phase& sz) 
{ return ostr << sz.strprint(8); }

#endif
