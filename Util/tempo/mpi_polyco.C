#include <assert.h>

#define MPI
#include "polyco.h"

int polynomial::mpiPack_size (MPI_Comm comm, int* size) const
{
  int total_size = 0;
  int temp_size = 0;

  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // psrname.length
  total_size += temp_size;
  MPI_Pack_size ((int)psrname.size(),  MPI_CHAR,  comm, &temp_size);  // psrname
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // date.length
  total_size += temp_size;
  MPI_Pack_size ((int)date.size(), MPI_CHAR,  comm, &temp_size);  // date
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // utc.length
  total_size += temp_size;
  MPI_Pack_size ((int)utc.size(), MPI_CHAR,  comm, &temp_size);  // date
  total_size += temp_size;
  MPI_Pack_size (1, MPI_INT,    comm, &temp_size);  // tempov11
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
  int length, mpi_bool;
  const char * data;
  length = (int)(psrname.length());
  data = psrname.c_str();
  MPI_Pack (&length,     1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)data,   (int)(psrname.length()), MPI_CHAR,   outbuf, outcount, position, comm);
  length = (int)(date.length());
  data = date.c_str();
  MPI_Pack (&length,        1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)data,         (int)(date.length()), MPI_CHAR,   outbuf, outcount, position, comm);
  length = (int)(utc.length());
  data = utc.c_str();
  MPI_Pack (&length,         1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)data,           (int)(utc.length()), MPI_CHAR,   outbuf, outcount, position, comm);
  mpi_bool = tempov11;
  MPI_Pack (&mpi_bool,        	   1, MPI_INT,    outbuf, outcount, position, comm);
  if(tempov11){
    MPI_Pack ((void*)&doppler_shift,    	   1, MPI_DOUBLE, outbuf, outcount, position, comm);
    MPI_Pack ((void*)&log_rms_resid,    	   1, MPI_DOUBLE, outbuf, outcount, position, comm);
  }
  ref_phase.mpiPack(outbuf, outcount, position, comm);

  MPI_Pack ((void*)&f0,               	   1, MPI_DOUBLE, outbuf, outcount, position, comm);
  MPI_Pack ((void*)&telescope,        	   1, MPI_INT,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)&freq,             	   1, MPI_DOUBLE,  outbuf, outcount, position, comm);
  mpi_bool = binary;
  MPI_Pack (&mpi_bool,        	   1, MPI_INT,    outbuf, outcount, position, comm);
  if(binary){
    MPI_Pack ((void*)&binph,            	   1, MPI_DOUBLE,  outbuf, outcount, position, comm);
    MPI_Pack ((void*)&binfreq,         	   1, MPI_DOUBLE,  outbuf, outcount, position, comm);
  }
  MPI_Pack ((void*)&nspan_mins,       	   1, MPI_DOUBLE,    outbuf, outcount, position, comm);
  MPI_Pack ((void*)&dm,               	   1, MPI_DOUBLE,  outbuf, outcount, position, comm);
  length = (int)(coefs.size());
  MPI_Pack (&length,     	   1, MPI_INT,    outbuf, outcount, position, comm);
  for(int i=0; i<coefs.size(); ++i)
    MPI_Pack ((void*)&(coefs[i]),        1, MPI_DOUBLE, outbuf, outcount, position, comm);

  reftime.mpiPack                     (outbuf, outcount, position, comm);

  return MPI_SUCCESS;
}

int polynomial::mpiUnpack (void* inbuf, int insize, int* position, 
			    MPI_Comm comm)
{
  int tmpint;
  int mpi_bool;

  MPI_Unpack (inbuf, insize, position, &tmpint, 	 1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &psrname,   	 tmpint, MPI_CHAR,   comm);
  MPI_Unpack (inbuf, insize, position, &tmpint, 	 1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &date,      	 tmpint, MPI_CHAR,   comm);
  MPI_Unpack (inbuf, insize, position, &tmpint, 	 1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &utc,       	 tmpint, MPI_CHAR,   comm);
  MPI_Unpack (inbuf, insize, position, &mpi_bool, 	 1, MPI_INT,    comm);
  tempov11 = mpi_bool;
  if(tempov11){
    MPI_Unpack (inbuf, insize, position, &doppler_shift,   1, MPI_DOUBLE, comm);
    MPI_Unpack (inbuf, insize, position, &log_rms_resid,   1, MPI_DOUBLE, comm);
  }
  ref_phase.mpiUnpack(inbuf, insize, position, comm);

  MPI_Unpack (inbuf, insize, position, &f0,        	 1, MPI_DOUBLE, comm);
  MPI_Unpack (inbuf, insize, position, &telescope, 	 1, MPI_INT,    comm);
  MPI_Unpack (inbuf, insize, position, &freq,      	 1, MPI_DOUBLE,  comm);
  MPI_Unpack (inbuf, insize, position, &mpi_bool, 	 1, MPI_INT,    comm);
  binary = mpi_bool;
  if(binary){
    MPI_Unpack (inbuf, insize, position, &binph,     	 1, MPI_DOUBLE,  comm);
    MPI_Unpack (inbuf, insize, position, &binfreq,     	 1, MPI_DOUBLE,  comm);
  }
  MPI_Unpack (inbuf, insize, position, &nspan_mins,	 1, MPI_DOUBLE,  comm);
  MPI_Unpack (inbuf, insize, position, &dm,        	 1, MPI_DOUBLE,  comm);
  MPI_Unpack (inbuf, insize, position, &tmpint, 	 1, MPI_INT,    comm);

  coefs.resize(tmpint);
  for(int i=0; i<coefs.size(); ++i)  
    MPI_Unpack (inbuf, insize, position, &(coefs[i]),  1, MPI_DOUBLE, comm);

  reftime.mpiUnpack                     (inbuf, insize, position, comm);

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
  MPI_Pack (&length,   1, MPI_INT,   outbuf, outcount, position, comm);
  for (int i=0; i<pollys.size(); i++) {
    pollys[i].mpiPack               (outbuf, outcount, position, comm);
  }
  return MPI_SUCCESS;
}

int polyco::mpiUnpack (void* inbuf, int insize, int* position, 
		       MPI_Comm comm)
{
  int npollys;
  int i = 0;

  MPI_Unpack (inbuf, insize, position, &npollys, 1, MPI_INT, comm);
  pollys = vector<polynomial>(npollys);
  for (i=0; i<npollys; i++) 
    pollys[i].mpiUnpack (inbuf, insize, position, comm);

  return MPI_SUCCESS;
}
