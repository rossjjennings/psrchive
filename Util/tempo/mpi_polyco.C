#include <stdio.h>
#include <assert.h>

#define MPI
#include "polyco.h"

int polynomial::mpiPack_size (MPI_Comm comm)
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (9,  MPI_CHAR,  comm, &temp_size);  // psrname
  total_size += temp_size;
  MPI_Pack_size (12, MPI_CHAR,  comm, &temp_size);  // date
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // ph0
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // f0
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // telescope
  total_size += temp_size;
  MPI_Pack_size (1, MPI_FLOAT,  comm, &temp_size);  // freq
  total_size += temp_size;
  MPI_Pack_size (1, MPI_FLOAT,  comm, &temp_size);  // binph
  total_size += temp_size;
  MPI_Pack_size (1, MPI_FLOAT,  comm, &temp_size);  // binp
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // nspan
  total_size += temp_size;
  MPI_Pack_size (1, MPI_FLOAT,  comm, &temp_size);  // dm
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // ncoef
  total_size += temp_size;

  MPI_Pack_size (ncoef, MPI_DOUBLE, comm, &temp_size); // ncoef doubles
  total_size += temp_size;

  total_size += reftime.mpiPack_size (comm);

  return total_size;
}

int polynomial::mpiPack (void* outbuf, int outcount, int* position, 
			  MPI_Comm comm)
{
  MPI_Pack (&psrname,   9, MPI_CHAR,   outbuf, outcount, position, comm);
  MPI_Pack (&date,     12, MPI_CHAR,   outbuf, outcount, position, comm);
  MPI_Pack (&ph0,       1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack (&f0,        1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack (&telescope, 1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack (&freq,      1, MPI_FLOAT,  outbuf, outcount, position, comm);
  MPI_Pack (&binph,     1, MPI_FLOAT,  outbuf, outcount, position, comm);
  MPI_Pack (&binp,      1, MPI_FLOAT,  outbuf, outcount, position, comm);
  MPI_Pack (&nspan,     1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack (&dm,        1, MPI_FLOAT,  outbuf, outcount, position, comm);
  MPI_Pack (&ncoef,     1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack (coefs,  ncoef, MPI_DOUBLE, outbuf, outcount, position, comm);

  reftime.mpiPack                     (outbuf, outcount, position, comm);

  return MPI_SUCCESS;
}

int polynomial::mpiUnpack (void* outbuf, int outcount, int* position, 
			    MPI_Comm comm)
{
  MPI_Unpack (outbuf, outcount, position, &psrname,   9, MPI_CHAR,   comm);
  MPI_Unpack (outbuf, outcount, position, &date,     12, MPI_CHAR,   comm);
  MPI_Unpack (outbuf, outcount, position, &ph0,       1, MPI_DOUBLE, comm);
  MPI_Unpack (outbuf, outcount, position, &f0,        1, MPI_DOUBLE, comm);
  MPI_Unpack (outbuf, outcount, position, &telescope, 1, MPI_INT,    comm);
  MPI_Unpack (outbuf, outcount, position, &freq,      1, MPI_FLOAT,  comm);
  MPI_Unpack (outbuf, outcount, position, &binph,     1, MPI_FLOAT,  comm);
  MPI_Unpack (outbuf, outcount, position, &binp,      1, MPI_FLOAT,  comm);
  MPI_Unpack (outbuf, outcount, position, &nspan,     1, MPI_INT,    comm);
  MPI_Unpack (outbuf, outcount, position, &dm,        1, MPI_FLOAT,  comm);

  int curncoef = ncoef;
  MPI_Unpack (outbuf, outcount, position, &ncoef,     1, MPI_INT,    comm);

  if (curncoef != ncoef) {
    if (coefs != NULL) delete [] coefs;
    coefs = new double [ncoef];
    assert (coefs != (double*)NULL);
  }
  MPI_Unpack (outbuf, outcount, position, coefs,  ncoef, MPI_DOUBLE, comm);

  reftime.mpiUnpack                     (outbuf, outcount, position, comm);

  return MPI_SUCCESS;
}

int polynomial::mpiBcast (int root, MPI_Comm comm)
{
  static void* bcastbuf = NULL;
  static int   bufsize  = 0;

  int  self = 0;
  int  reqdbufsize = 0;
  int  mpi_err;
  char mpi_errstr [MPI_MAX_ERROR_STRING];

  MPI_Comm_rank (comm, &self);

  if (self == root) {
    reqdbufsize = mpiPack_size (comm);
  }
  mpi_err = MPI_Bcast (&reqdbufsize, 1, MPI_INT, root, comm);
  if (mpi_err != MPI_SUCCESS) {
    int len;
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "observation::mpiBcast: err MPI_Bcast %s\n",
             mpi_errstr);
    return mpi_err;
  }

  if (bufsize < reqdbufsize) {
    if (bcastbuf) free (bcastbuf);
    bcastbuf = malloc (reqdbufsize);
    assert (bcastbuf != NULL);
    bufsize = reqdbufsize;
  }

  int position = 0;
  if (self == root) {  
    mpiPack (bcastbuf, reqdbufsize, &position, comm);
  }
  mpi_err = MPI_Bcast (bcastbuf, reqdbufsize, MPI_PACKED, root, comm);
  if (mpi_err != MPI_SUCCESS) {
    int len;
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "observation::mpiBcast: err MPI_Bcast %s\n",
             mpi_errstr);
    return mpi_err;
  }
  if (self != root) {
    mpiUnpack (bcastbuf, reqdbufsize, &position, comm);
  }
  return 0;
}

int polyco::mpiPack_size (MPI_Comm comm)
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (1,  MPI_INT,  comm, &temp_size);  // npollys
  total_size += temp_size;

  for (int i=0; i<npollys; i++) {
    total_size += pollys[i]->mpiPack_size (comm);
  }
  return total_size;
}

int polyco::mpiPack (void* outbuf, int outcount, int* position, 
		     MPI_Comm comm)
{
  MPI_Pack (&npollys,   1, MPI_INT,   outbuf, outcount, position, comm);
  for (int i=0; i<npollys; i++) {
    pollys[i]->mpiPack               (outbuf, outcount, position, comm);
  }
  return MPI_SUCCESS;
}

int polyco::mpiUnpack (void* outbuf, int outcount, int* position, 
		       MPI_Comm comm)
{
  int old_npollys = npollys;
  int i = 0;

  MPI_Unpack (outbuf, outcount, position, &npollys, 1, MPI_INT, comm);

  if (old_npollys != npollys) {
    if (pollys != NULL) {
      for (i=0; i<old_npollys; i++)
	delete pollys[i];
      delete [] pollys;
    }
    pollys = new polynomial* [npollys];
    assert (pollys != (polynomial **) NULL);
    for (i=0; i<npollys; i++) {
      pollys[i]=new polynomial(); 
      assert (pollys[i] != (polynomial*) NULL);
    }
  }
  for (i=0; i<npollys; i++) {
    pollys[i]->mpiUnpack (outbuf, outcount, position, comm);
  }
  return MPI_SUCCESS;
}

int polyco::mpiBcast (int root, MPI_Comm comm)
{
  static void* bcastbuf = NULL;
  static int   bufsize  = 0;

  int  self = 0;
  int  reqdbufsize = 0;
  int  mpi_err;
  char mpi_errstr [MPI_MAX_ERROR_STRING];

  MPI_Comm_rank (comm, &self);

  if (self == root) {
    reqdbufsize = mpiPack_size (comm);
  }
  mpi_err = MPI_Bcast (&reqdbufsize, 1, MPI_INT, root, comm);
  if (mpi_err != MPI_SUCCESS) {
    int len;
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "observation::mpiBcast: err MPI_Bcast %s\n",
             mpi_errstr);
    return mpi_err;
  }

  if (bufsize < reqdbufsize) {
    if (bcastbuf) free (bcastbuf);
    bcastbuf = malloc (reqdbufsize);
    assert (bcastbuf != NULL);
    bufsize = reqdbufsize;
  }

  int position = 0;
  if (self == root) {  
    mpiPack (bcastbuf, reqdbufsize, &position, comm);
  }
  mpi_err = MPI_Bcast (bcastbuf, reqdbufsize, MPI_PACKED, root, comm);
  if (mpi_err != MPI_SUCCESS) {
    int len;
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "observation::mpiBcast: err MPI_Bcast %s\n",
             mpi_errstr);
    return mpi_err;
  }
  if (self != root) {
    mpiUnpack (bcastbuf, reqdbufsize, &position, comm);
  }
  return 0;
}
