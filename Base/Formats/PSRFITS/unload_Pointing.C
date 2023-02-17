/***************************************************************************
 *
 *   Copyright (C) 2004 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pointing.h"

#include "psrfitsio.h"
#include "FITSError.h"

#include <cassert>

using namespace std;

/*!
  \pre The current HDU is the SUBINT HDU
*/
void Pulsar::FITSArchive::unload (fitsfile* fptr, const Pointing* ext, int row) const
{
  int status = 0;

  if (verbose > 2)
    cerr << "FITSArchive::unload_Pointing entered" << endl;
  
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "LST_SUB", &colnum, &status);
  
  double tempdouble = ext->get_local_sidereal_time ();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col LST_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "RA_SUB", &colnum, &status);
  
  tempdouble = ext->get_right_ascension().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col RA_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DEC_SUB", &colnum, &status);
  
  tempdouble = ext->get_declination().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col DEC_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "GLON_SUB", &colnum, &status);
  
  tempdouble = ext->get_galactic_longitude().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col GLON_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "GLAT_SUB", &colnum, &status);
  
  tempdouble = ext->get_galactic_latitude().getDegrees();
  fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &tempdouble, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col GLAT_SUB");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "FD_ANG", &colnum, &status);
  
  float tempfloat = ext->get_feed_angle().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col FD_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "POS_ANG", &colnum, &status);
  
  tempfloat = ext->get_position_angle().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col POS_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "PAR_ANG", &colnum, &status);
  
  tempfloat = ext->get_parallactic_angle().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col PAR_ANG");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TEL_AZ", &colnum, &status);
  
  tempfloat = ext->get_telescope_azimuth().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col TEL_AZ");

  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TEL_ZEN", &colnum, &status);
  
  tempfloat = ext->get_telescope_zenith().getDegrees();
  fits_write_col (fptr, TFLOAT, colnum, row, 1, 1, &tempfloat, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::unload_Pointing", 
                     "fits_write_col TEL_ZEN");

  unsigned ninfo = ext->get_ninfo();

  if (ninfo == 0)
  {
    if (verbose > 2)
      cerr << "FITSArchive::unload_Pointing no Pointing::Info" << endl;
    return;
  }

  assert (ninfo <= extra_pointing_columns.size());

  colnum = get_last_pointing_column (fptr);
  if (verbose > 2)
    cerr << "FITSArchive::unload_Pointing last Pointing "
         " colnum=" << colnum << endl;

  if (colnum+1 != extra_pointing_columns[0].colnum)
  {
    if (verbose > 2)
      cerr << "FITSArchive::unload_Pointing "
                  "correcting offset in last column index" << endl;

    for (unsigned i=0; i < extra_pointing_columns.size(); i++)
      extra_pointing_columns[i].colnum = colnum + i + 1;
  } 

  for (unsigned i=0; i < ninfo; i++)
  { 
    const Pointing::Info* info = ext->get_info(i);

    pointing_info_column match;
    bool found = false;
    for (unsigned icol=0; icol < extra_pointing_columns.size(); icol++)
    {
       match = extra_pointing_columns[icol];
       if (match.name == info->get_name())
       {
         found = true;
         break;
       }
    }

    if (!found)
      throw Error (InvalidState, "FITSArchive::unload_Pointing",
                   "no matching columns found for Pointing::Info "
                   "with name='" + info->get_name() + "'");

    colnum = match.colnum;

    double value = info->get_value();

    if (verbose > 2)
      cerr << "FITSArchive::unload_Pointing Info::name=" << info->get_name()
           << " value=" << value << " colnum=" << colnum << endl;

    fits_write_col (fptr, TDOUBLE, colnum, row, 1, 1, &value, &status);

    if (status != 0)
      throw FITSError (status, "FITSArchive::unload_Pointing",
                       "fits_write_col " + info->get_name());
  }

  if (verbose > 2)
    cerr << "FITSArchive::unload_Pointing exiting" << endl;
}

