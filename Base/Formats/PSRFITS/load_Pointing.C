#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pointing.h"
#include "FITSError.h"

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::load_Pointing (fitsfile* fptr, int row,
						    Pulsar::Integration* integ)
{
  if (verbose == 3)
    cerr << "FITSArchive::load_Pointing" << endl;
  
  Reference::To<Pointing> ext = new Pointing;

  // status returned by FITSIO routines
  int status = 0;

  int initflag = 0;
  int colnum = 0;
  float nullfloat = 0.0;
  double nulldouble = 0.0;

  fits_get_colnum (fptr, CASEINSEN, "LST_SUB", &colnum, &status);
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &(ext->lst_sub), &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col LST_SUB");

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "RA_SUB", &colnum, &status);
  
  double my_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &my_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col RA_SUB");
  
  ext->ra_sub.setDegrees(my_angle);

  initflag = 0;
  colnum = 0;

  fits_get_colnum (fptr, CASEINSEN, "DEC_SUB", &colnum, &status);
  
  my_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &my_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col DEC_SUB");
  
  ext->dec_sub.setDegrees(my_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "GLON_SUB", &colnum, &status);
  
  my_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &my_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col GLON_SUB");
  
  ext->glon_sub.setDegrees(my_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "GLAT_SUB", &colnum, &status);
  
  my_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &my_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col GLAT_SUB");
  
  ext->glat_sub.setDegrees(my_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "FD_ANG", &colnum, &status);
  
  float my_other_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &my_other_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col FD_ANG");
  
  ext->fd_ang.setDegrees(my_other_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "POS_ANG", &colnum, &status);
  
  my_other_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &my_other_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col POS_ANG");
  
  ext->pos_ang.setDegrees(my_other_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "PAR_ANG", &colnum, &status);
  
  my_other_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &my_other_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col PAR_ANG");
  
  ext->par_ang.setDegrees(my_other_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "TEL_AZ", &colnum, &status);
  
  my_other_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &my_other_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col TEL_AZ");
  
  ext->tel_az.setDegrees(my_other_angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "TEL_ZEN", &colnum, &status);
  
  my_other_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &my_other_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col TEL_ZEN");
  
  ext->tel_zen.setDegrees(my_other_angle);

  if (status == 0) {
    integ->add_extension (ext);
    return;
  }

  if (verbose == 3)
    cerr << FITSError (status, "FITSArchive::load_Pointing").warning() 
	 << endl;

}





