  /* $Log: MJD.h,v $
  /* Revision 1.1  1998/08/03 06:45:23  mbritton
  /* Initial revision
  /*
 * Revision 1.5  1996/09/13 23:13:59  mbailes
 * more fns
 *
 * Revision 1.4  1996/09/09 23:11:28  mbailes
 * Added new functions.
 * */
// MJDs should always be positive, these variables do
// not exist to calculate time differences. For that you
// should stick to 
#ifndef _MJD_H
#define _MJD_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef MPI
#include "mpi.h"
#endif

class MJD {
private:
  int    days;
  int    secs;
  double fracsec;
public:
  MJD(){days=0;secs=0;fracsec=0.0;};
  MJD(int dd,int ss,double fs);       // Create and verify an MJD
  MJD(char * yyyydddhhmmss);          // For use from Header
  MJD(double yy,double ss, double fs);
  double in_seconds();
  double in_days();
  double in_minutes();
  friend MJD operator + (const MJD &, double);  // Add seconds to an MJD
  friend MJD operator - (const MJD &, double);  // Take seconds from an MJD
  friend MJD operator + (const MJD &, const MJD &);  // Add two MJDs
  friend MJD operator - (const MJD &, const MJD &);
  friend MJD operator / (const MJD &, double);  // divide an MJD by a double
  friend int operator == (const MJD &, const MJD &);
  friend int operator > (const MJD &, const MJD &);
  friend int operator < (const MJD &, const MJD &);
  // These bits are useful for tempo
  int intday();                       // To access the integer day
  double fracday();                   // To access fractional day
  int print (FILE *stream);
  int println (FILE *stream);
  char * printall();
  char * printdays();
  char * printhhmmss();
  char * printfs();
  char * strtempo();

#ifdef MPI
  int mpiBcast (MPI_Comm comm, int master, int self);
  int mpiSend (int dest, int tag, MPI_Comm comm);
  int mpiRecv (int source, int tag, MPI_Comm comm, MPI_Status *status);
#endif

};

int something (MJD dum, int there);

#endif  


