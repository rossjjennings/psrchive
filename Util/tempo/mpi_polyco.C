#include <stdio.h>
#include <assert.h>

#define MPI
#include "polyco.h"

int polynomial::mpiBcast (MPI_Comm comm, int master, int self) {
  int mpi_err;
  char mpi_errstr [MPI_MAX_ERROR_STRING];
  int len;
  int ci;

  mpi_err = MPI_Bcast (psrname, 9, MPI_CHAR,     master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (date, 12, MPI_CHAR,       master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&ph0, 1, MPI_DOUBLE,      master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&f0, 1, MPI_DOUBLE,       master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&telescope, 1, MPI_INT,   master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&freq, 1, MPI_FLOAT,      master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&binph, 1, MPI_FLOAT,     master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&binp, 1, MPI_FLOAT,      master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&nspan, 1, MPI_INT,      master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }
  mpi_err = MPI_Bcast (&dm, 1, MPI_FLOAT,      master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }

  mpi_err = reftime.mpiBcast (comm, master, self);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MJD::mpiBcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }

  mpi_err = MPI_Bcast (&ncoef, 1, MPI_INT,       master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }

  if (self != master) {
    if (coefs != NULL) delete [] coefs;
    coefs = new double [ncoef];
    assert (coefs != (double*)NULL);
  }

  for (ci=0; ci<ncoef; ci++) {
    mpi_err = MPI_Bcast (&coefs[ci], 1, MPI_DOUBLE,       master, comm);
    if (mpi_err != MPI_SUCCESS) {
      MPI_Error_string (mpi_err, mpi_errstr, &len);
      fprintf (stderr, "polynomial::mpiBcast: err MPI_Bcast %s\n",
	       mpi_errstr);
      return mpi_err;
    }
  }

  return MPI_SUCCESS;
}

int polyco::mpiBcast (MPI_Comm comm, int master, int self) {
  int mpi_err;
  char mpi_errstr [MPI_MAX_ERROR_STRING];
  int len;
  int i;

  int old_npollys = npollys;

  mpi_err = MPI_Bcast (&npollys, 1, MPI_INT, master, comm);
  if (mpi_err != MPI_SUCCESS) {
    MPI_Error_string (mpi_err, mpi_errstr, &len);
    fprintf (stderr, "polyco::Bcast: err MPI_Bcast %s\n",
	     mpi_errstr);
    return mpi_err;
  }

  if (self != master) {
    if (pollys != NULL) {
      for (i=0; i<old_npollys; i++)
	delete pollys[i];
      delete [] pollys;
    }
    pollys = new polynomial* [npollys];
    assert (pollys != (polynomial **)NULL);
    for (i=0; i<npollys; i++) {
      pollys[i]=new polynomial(); 
      assert (pollys[i] != (polynomial*)NULL);
    }
  }

  for (i=0; i<npollys; i++) {
    mpi_err = pollys[i]->mpiBcast (comm, master, self);
    if (mpi_err != MPI_SUCCESS) {
      MPI_Error_string (mpi_err, mpi_errstr, &len);
      fprintf (stderr, "polyco::Bcast: err MPI_Bcast %s\n",
	       mpi_errstr);
      return mpi_err;
    }
  }

  return MPI_SUCCESS;
}

