#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pointing.h"
#include "FITSError.h"

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::unload (fitsfile* fptr, const Pointing* ext, int row)
{
  int status = 0;

  if (verbose == 3)
    cerr << "FITSArchive::unload_Pointing entered" << endl;
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "LST_SUB", &colnum, &status);
  
  double tempdouble = ext->lst_sub;
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col LST_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "RA_SUB", &colnum, &status);
  
  tempdouble = ext->ra_sub.getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col RA_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DEC_SUB", &colnum, &status);
  
  tempdouble = ext->dec_sub.getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col DEC_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "GLON_SUB", &colnum, &status);
  
  tempdouble = ext->glon_sub.getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col GLON_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "GLAT_SUB", &colnum, &status);
  
  tempdouble = ext->glat_sub.getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col GLAT_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "FD_ANG", &colnum, &status);
  
  float tempfloat = ext->fd_ang.getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col FD_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "POS_ANG", &colnum, &status);
  
  tempfloat = ext->pos_ang.getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col POS_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PAR_ANG", &colnum, &status);
  
  tempfloat = ext->par_ang.getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col PAR_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TEL_AZ", &colnum, &status);
  
  tempfloat = ext->tel_az.getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col TEL_AZ");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TEL_ZEN", &colnum, &status);
  
  tempfloat = ext->tel_zen.getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col TEL_ZEN");

  if (verbose == 3)
    cerr << "FITSArchive::unload_Pointing exiting" << endl;
}
