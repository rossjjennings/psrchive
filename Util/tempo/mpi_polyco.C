//#include <assert.h>
#include <iostream>
#define MPI
#include "polyco.h"
#include "stdmpi.h"

int polynomial::mpiPack_size (MPI_Comm comm, int* size) const
{
  int total_size = 0;
  int temp_size = 0;

  stdmpi::Pack_size (psrname, comm, &temp_size);
  total_size += temp_size;
  stdmpi::Pack_size (date, comm, &temp_size);
  total_size += temp_size;
  stdmpi::Pack_size (utc,  comm, &temp_size);
  total_size += temp_size;

  MPI_Pack_size (1, MPI_CHAR,    comm, &temp_size);  // tempov11
  total_size += temp_size;
  if(tempov11){
    MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // doppler_shift
    total_size += temp_size;
    MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // log_rms_resid
    total_size += temp_size;
  }
  ref_phase.mpiPack_size(comm, &temp_size);
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE, comm, &temp_size);  // f0
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // telescope
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // freq
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // binary
  total_size += temp_size;
  if(binary){
    MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // binph
    total_size += temp_size;
    MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // binfreq
    total_size += temp_size;
  }
  MPI_Pack_size (1, MPI_DOUBLE,    comm, &temp_size);  // nspan_mins
  total_size += temp_size;
  MPI_Pack_size (1, MPI_DOUBLE,  comm, &temp_size);  // dm
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // coefs.size()
  total_size += temp_size;

  MPI_Pack_size ((int)coefs.size(), MPI_DOUBLE, comm, &temp_size); // ncoef doubles
  total_size += temp_size;

  reftime.mpiPack_size (comm, &temp_size);
  total_size += temp_size;

  *size = total_size;
  return 0; // no error, not dynamic
}

int polynomial::mpiPack (void* outbuf, int outcount, int* position, 
			  MPI_Comm comm) const
{
  stdmpi::Pack (psrname, outbuf, outcount, position, comm);
  stdmpi::Pack (date, outbuf, outcount, position, comm);
  stdmpi::Pack (utc, outbuf, outcount, position, comm);

cerr << "PACKED: '" << psrname << "' '" << date << "' '" << utc << "'\n";

  char boolean = tempov11;
  MPI_Pack (&boolean, 1, MPI_CHAR,    outbuf, outcount, position, comm);

  double temp = 0;
  if (tempov11) {
    temp = doppler_shift;
    MPI_Pack (&temp,  1, MPI_DOUBLE, outbuf, outcount, position, comm);
    temp = log_rms_resid;
    MPI_Pack (&temp,  1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }
  ref_phase.mpiPack(outbuf, outcount, position, comm);

  MPI_Pack ((void*)&f0,        1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack ((void*)&telescope, 1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)&freq,      1, MPI_DOUBLE, outbuf, outcount, position, comm);
  boolean = binary;
  MPI_Pack (&boolean,          1, MPI_CHAR,   outbuf, outcount, position, comm);
  if (binary) {
    MPI_Pack ((void*)&binph,   1, MPI_DOUBLE, outbuf, outcount, position, comm);
    MPI_Pack ((void*)&binfreq, 1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }
  MPI_Pack ((void*)&nspan_mins,1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack ((void*)&dm,        1, MPI_DOUBLE, outbuf, outcount, position, comm);
  int length = (int)(coefs.size());
  MPI_Pack (&length,           1, MPI_INT,    outbuf, outcount, position, comm);
  for (int i=0; i<coefs.size(); ++i) {
    temp = coefs[i];
    MPI_Pack (&temp, 1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }

  reftime.mpiPack (outbuf, outcount, position, comm);

  return MPI_SUCCESS;
}

int polynomial::mpiUnpack (void* inbuf, int insize, int* position, 
			MPI_Comm comm)
{
  char boolean;

  stdmpi::Unpack  (inbuf, insize, position, &psrname, comm);
  stdmpi::Unpack  (inbuf, insize, position, &date, comm);
  stdmpi::Unpack  (inbuf, insize, position, &utc, comm);
  MPI_Unpack (inbuf, insize, position, &boolean, 1, MPI_CHAR, comm);
  tempov11 = boolean;
  if (tempov11) {
    MPI_Unpack (inbuf, insize, position, &doppler_shift, 1, MPI_DOUBLE, comm);
    MPI_Unpack (inbuf, insize, position, &log_rms_resid, 1, MPI_DOUBLE, comm);
  }
  ref_phase.mpiUnpack(inbuf, insize, position, comm);

  MPI_Unpack (inbuf, insize, position, &f0,        	 1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &telescope, 	 1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &freq,      	 1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &boolean, 	 1, MPI_CHAR,   comm);
  binary = boolean;
  if (binary) {
    MPI_Unpack (inbuf, insize, position, &binph,     	 1, MPI_DOUBLE, comm);
    MPI_Unpack (inbuf, insize, position, &binfreq,     	 1, MPI_DOUBLE, comm);
  }
  MPI_Unpack (inbuf, insize, position, &nspan_mins,	 1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &dm,        	 1, MPI_DOUBLE, comm);

  int tmpint = 0;
  MPI_Unpack (inbuf, insize, position, &tmpint, 	 1, MPI_INT,    comm);
  coefs.resize(tmpint);
  for(int i=0; i<coefs.size(); ++i)  
    MPI_Unpack (inbuf, insize, position, &(coefs[i]),  1, MPI_DOUBLE,   comm);

  reftime.mpiUnpack (inbuf, insize, position, comm);

  return MPI_SUCCESS;
}

int polyco::mpiPack_size (MPI_Comm comm, int* size) const
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (1,  MPI_INT,  comm, &temp_size);  // npollys
  total_size += temp_size;

  for (int i=0; i<pollys.size(); i++) {
    pollys[i].mpiPack_size (comm, &temp_size);
    total_size += temp_size;
  }
  *size = total_size;
  return 1; // no error, dynamic
}

int polyco::mpiPack (void* outbuf, int outcount, int* position, 
		     MPI_Comm comm) const
{
  int length = (int)(pollys.size());
  MPI_Pack (&length, 1, MPI_INT, outbuf, outcount, position, comm);
  for (int i=0; i<pollys.size(); i++) {
    pollys[i].mpiPack (outbuf, outcount, position, comm);
  }
  return MPI_SUCCESS;
}

int polyco::mpiUnpack (void* inbuf, int insize, int* position, 
		       MPI_Comm comm)
{
  int npollys;
  int i = 0;

  MPI_Unpack (inbuf, insize, position, &npollys, 1, MPI_INT, comm);
  pollys.resize(npollys);
  for (i=0; i<npollys; i++) 
    pollys[i].mpiUnpack (inbuf, insize, position, comm);

  return MPI_SUCCESS;
}

