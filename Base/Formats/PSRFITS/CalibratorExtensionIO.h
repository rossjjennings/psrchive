#include "Pulsar/CalibratorExtension.h"
#include "FITSError.h"
#include "fitsutil.h"

#include <memory>
#include <fitsio.h>

namespace Pulsar {

  void load (fitsfile* fptr, CalibratorExtension* ext);

  void unload (fitsfile* fptr, const CalibratorExtension* ext);

  template<class T>
    void load_Estimate_vector (fitsfile* fptr, vector< Estimate<T> >& data,
			       char* column_name, const char* method)
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
	throw FITSError (status, method, "fits_read_col %s", column_name);

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
	throw FITSError (status, method, "fits_read_col " + cname);

      for (idim=0; idim < dimension; idim++) {
	float err = temp.get()[idim];
	data[idim].var = err*err;
      }

    }

}

