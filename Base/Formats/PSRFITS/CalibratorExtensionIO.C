#include "Pulsar/CalibratorExtensionIO.h"

void Pulsar::load_Estimates (fitsfile* fptr, vector< Estimate<double> >& data,
			     char* column_name)
{
  long dimension = data.size();
  
  auto_ptr<float> temp ( new float[dimension] );
  
  int status = 0;
  int colnum = 0;
  int initflag = 0;
  
  // Read the data values
  fits_get_colnum (fptr, CASEINSEN, column_name, &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &fits_nullfloat,
		 temp.get(), &initflag, &status);
  
  if (status)
    throw FITSError (status, "Pulsar::load_Estimates",
		     "fits_read_col %s", column_name);
  
  int idim = 0;
  
  for (idim=0; idim < dimension; idim++)
    data[idim].val = temp.get()[idim];
  
  // name of column containing data errors
  string cname = column_name;
  cname += "ERR";
  column_name = const_cast<char*>(cname.c_str());
  
  colnum = 0;
  initflag = 0;
  
  // Read the data errors
  fits_get_colnum (fptr, CASEINSEN, column_name, &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, 1, 1, dimension, &fits_nullfloat, 
		 temp.get(), &initflag, &status);
  
  if (status)
    throw FITSError (status, "Pulsar::load_Estimates",
		     "fits_read_col " + cname);
  
  for (idim=0; idim < dimension; idim++) {
    float err = temp.get()[idim];
    data[idim].var = err*err;
  }
  
}

void Pulsar::unload_Estimates (fitsfile* fptr, vector<Estimate<double> >& data,
			     char* column_name)
{
  long dimension = data.size();
  
  auto_ptr<float> temp ( new float[dimension] );
  
  int idim;

  // Write the data values
  for (idim = 0; idim < dimension; idim++)
    temp.get()[idim] = data[idim].val;

  int status = 0;
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, column_name, &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  temp.get(), &status);

  if (status)
    throw FITSError (status, "Pulsar::unload_Estimates", 
		     "fits_write_col %s", column_name);

  // name of column containing data errors
  string cname = column_name;
  cname += "ERR";
  column_name = const_cast<char*>(cname.c_str());
  
  // Write the data errors
  for (idim = 0; idim < dimension; idim++)
    temp.get()[idim] = sqrt(data[idim].var);

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, column_name, &colnum, &status);
  fits_modify_vector_len (fptr, colnum, dimension, &status);
  fits_write_col (fptr, TFLOAT, colnum, 1, 1, dimension,
		  temp.get(), &status);

  if (status)
    throw FITSError (status, "Pulsar::unload_Estimates", 
		     "fits_write_col %s", column_name);

}
