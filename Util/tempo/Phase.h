#ifndef __PHASE_H
#define __PHASE_H

#include <math.h>
#include "environ.h"
#ifdef MPI
#include "mpi.h"
#endif
#include <string>

class Phase {

 private:
  int64 turns;
  double fturns;

 public:
  Phase();
  ~Phase(){turns=0; fturns=0.0;};
  Phase(int64 tns, double ftns);
  
  Phase& operator= (const Phase &in_Phase);

  friend Phase operator + (const Phase &, double); 
  friend Phase operator - (const Phase &, double); 
  friend Phase operator + (const Phase &, const Phase &);
  friend Phase operator - (const Phase &, const Phase &);
  friend Phase operator / (const Phase &, double);
  Phase& operator += (double);
  Phase& operator -= (double);
  friend int operator > (const Phase &, const Phase &) ;
  friend int operator < (const Phase &, const Phase &) ;
  friend int operator >= (const Phase &, const Phase &);
  friend int operator <= (const Phase &, const Phase &);
  friend int operator == (const Phase &, const Phase &);
  friend int operator != (const Phase &, const Phase &);

  double in_turns() const;
  int64 intturns() const;
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

#endif
