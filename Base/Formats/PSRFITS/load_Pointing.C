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

  double lst_in_seconds;

  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &lst_in_seconds, &initflag, &status);

  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col LST_SUB");

  ext->set_local_sidereal_time (lst_in_seconds);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "RA_SUB", &colnum, &status);
  
  double double_angle = 0.0;
  Angle angle;

  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col RA_SUB");

  angle.setTurns (double_angle);  
  ext->set_right_ascension (angle);

  initflag = 0;
  colnum = 0;

  fits_get_colnum (fptr, CASEINSEN, "DEC_SUB", &colnum, &status);
  
  double_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col DEC_SUB");
  
  angle.setTurns (double_angle);
  ext->set_declination (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "GLON_SUB", &colnum, &status);
  
  double_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col GLON_SUB");
  
  angle.setDegrees (double_angle);
  ext->set_galactic_longitude (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "GLAT_SUB", &colnum, &status);
  
  double_angle = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &double_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col GLAT_SUB");
  
  angle.setDegrees (double_angle);
  ext->set_galactic_latitude (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "FD_ANG", &colnum, &status);
  
  float float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col FD_ANG");
  
  angle.setDegrees (float_angle);
  ext->set_feed_angle (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "POS_ANG", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col POS_ANG");
  
  angle.setDegrees (float_angle);
  ext->set_position_angle (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "PAR_ANG", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col PAR_ANG");
  
  angle.setDegrees (float_angle);
  ext->set_parallactic_angle (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "TEL_AZ", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col TEL_AZ");
  
  angle.setDegrees (float_angle);
  ext->set_telescope_azimuth (angle);

  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "TEL_ZEN", &colnum, &status);
  
  float_angle = 0.0;
  
  fits_read_col (fptr, TFLOAT, colnum, row, 1, 1, &nullfloat,
		 &float_angle, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Pointing", 
		     "fits_read_col TEL_ZEN");
  
  angle.setDegrees (float_angle);
  ext->set_telescope_zenith (angle);

  if (status == 0) {
    integ->add_extension (ext);
    return;
  }

  if (verbose == 3)
    cerr << FITSError (status, "FITSArchive::load_Pointing").warning() 
	 << endl;

}





