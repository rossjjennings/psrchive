#include <assert.h>

#define MPI
#include "Phase.h"

int Phase::mpiPack_size (MPI_Comm comm, int* size)
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // turns
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // fracturns
  total_size += temp_size;

  *size = total_size;
  return(0);
}

int Phase::mpiPack (void* outbuf, int outcount, int* position, 
		    MPI_Comm comm){
  MPI_Pack (&turns,        	   1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack (&fturns,        	   1, MPI_DOUBLE,  outbuf, outcount, position, comm);
  return MPI_SUCCESS;
}

int Phase::mpiUnpack (void* inbuf, int insize, int* position, 
		      MPI_Comm comm)
{

  MPI_Unpack (inbuf, insize, position, &turns, 	                 1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &fturns,         	 1, MPI_DOUBLE,  comm);
  return MPI_SUCCESS;
}
