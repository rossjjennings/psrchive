/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileColumn.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Pulsar.h"

#include "psrfitsio.h"
#include "templates.h"
#include "RealTimer.h"

#include <float.h>
#include <math.h>

using namespace std;

void Pulsar::ProfileColumn::reset ()
{
  data_colnum = -1;
  offset_colnum = -1;
  scale_colnum = -1;
}

Pulsar::ProfileColumn::ProfileColumn ()
{
  fptr = 0;
  nbin = nchan = nprof = 0;
  verbose = false;
  reset ();
}

//! Set the fitsfile to/from which data are written/read
void Pulsar::ProfileColumn::set_fitsfile (fitsfile* f)
{
  fptr = f;
  reset ();
}

//! Set the name of the data column
void Pulsar::ProfileColumn::set_data_colname (const std::string& name)
{
  data_colname = name;
  reset ();
}

//! Get the index of the data column
unsigned Pulsar::ProfileColumn::get_data_colnum ()
{
  if (data_colnum <= 0)
    data_colnum = get_colnum (data_colname);

  return data_colnum;
}

//! Set the name of the offset column
void Pulsar::ProfileColumn::set_offset_colname (const std::string& name)
{
  offset_colname = name;
  reset ();
}

//! Get the index of the data column
unsigned Pulsar::ProfileColumn::get_offset_colnum ()
{
  if (offset_colnum <= 0)
    offset_colnum = get_colnum (offset_colname);

  return offset_colnum;
}
    
//! Set the name of the scale column
void Pulsar::ProfileColumn::set_scale_colname (const std::string& name)
{
  scale_colname = name;
  reset ();
}

//! Get the index of the data column
unsigned Pulsar::ProfileColumn::get_scale_colnum ()
{
  if (scale_colnum <= 0)
    scale_colnum = get_colnum (scale_colname);

  return scale_colnum;
}

//! Set the number of phase bins
void Pulsar::ProfileColumn::set_nbin (unsigned n)
{
  nbin = n;
}

//! Set the number of frequency channels
void Pulsar::ProfileColumn::set_nchan (unsigned n)
{
  nchan = n;
}

//! Set the number of profiles in each frequency channel
void Pulsar::ProfileColumn::set_nprof (unsigned n)
{
  nprof = n;
}

char* fits_str (const string& txt)
{
  return const_cast<char*>( txt.c_str() );
}

//! Insert the columns and resize
void Pulsar::ProfileColumn::create (unsigned start_column)
{
  if (!fptr)
    throw Error (InvalidState, "Pulsar::ProfileColumn::create",
		 "fitsfile not set");

  int status = 0;

  offset_colnum = start_column;
  fits_insert_col (fptr, offset_colnum,
		   fits_str(offset_colname), "E", &status);

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::create", 
                     "error inserting " + offset_colname);

  scale_colnum = start_column + 1;
  fits_insert_col (fptr, scale_colnum,
		   fits_str(scale_colname), "E", &status);

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::create", 
                     "error inserting " + scale_colname);

  data_colnum = start_column + 2;
  fits_insert_col (fptr, data_colnum,
		   fits_str(data_colname), "I", &status);

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::create", 
                     "error inserting " + data_colname);

  resize ();
}

int Pulsar::ProfileColumn::get_colnum (const string& name)
{
  if (!fptr)
    throw Error (InvalidState, "Pulsar::ProfileColumn::get_colnum",
		 "fitsfile not set");

  int status = 0;
  int colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, fits_str(name), &colnum, &status);

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::get_colnum", 
                     "fits_get_colnum " + name);

  return colnum;
}

void Pulsar::ProfileColumn::resize ()
{
  if (!fptr)
    throw Error (InvalidState, "Pulsar::ProfileColumn::resize",
		 "fitsfile not set");

  int status = 0;

  RealTimer clock;
  clock.start();
  fits_modify_vector_len (fptr, get_offset_colnum(), nchan*nprof, &status);
  clock.stop();
  cerr << "Pulsar::ProfileColumn::resize fits_modify_vector_len ("
       << offset_colname << "," << nchan*nprof << ") took " << clock.get_elapsed() << " sec" << endl;

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::resize", 
                     "error resizing " + offset_colname);

  if (verbose)
    cerr << "Pulsar::ProfileColumn::resize " << offset_colname 
	 << " resized to " << nchan*nprof << endl;

  clock.start();
  fits_modify_vector_len (fptr, get_scale_colnum(), nchan*nprof, &status);
  clock.stop();
  cerr << "Pulsar::ProfileColumn::resize fits_modify_vector_len ("
       << scale_colname << "," << nchan*nprof << ") took " << clock.get_elapsed() << " sec" << endl;

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::resize", 
                     "error resizing " + scale_colname);

  if (verbose)
    cerr << "Pulsar::ProfileColumn::resize " << scale_colname 
	 << " resized to " << nchan*nprof << endl;

  // number of values to be written
  uint64_t nvalue = nbin*nchan*nprof;
  // assuming 16-bit word per value
  const uint64_t bytes_per_value = 2;
  
  const uint64_t nbyte = nvalue * bytes_per_value;
  nrow = 1;

#if 0 // try resizing a larger number of rows of some maximum size

  // experimentally determined limit, beyond which CFITSIO becomes inefficient
  const uint64_t maximum_bytes = 1 << 19;

  if (nbyte > maximum_bytes)
  {
    nrow = nbyte / maximum_bytes;
    if (nbyte % maximum_bytes)
      nrow ++;
    nvalue = maximum_bytes / bytes_per_value;
  }

#endif
  
  cerr << "Pulsar::ProfileColumn::resize calling fits_modify_vector_len ("
       << data_colname << "," << nvalue <<")" << endl;

  clock.start();
  fits_modify_vector_len (fptr, get_data_colnum(), nvalue, &status);
  clock.stop();
  cerr << "Pulsar::ProfileColumn::resize fits_modify_vector_len ("
       << data_colname << "," << nvalue << ") took "
       << clock.get_elapsed() << " sec" << endl;
  
  psrfits_update_tdim (fptr, get_data_colnum(), nbin, nchan, nprof);

  if (verbose)
    cerr << "Pulsar::ProfileColumn::resize " << data_colname 
	 << " resized to " << nbin*nchan*nprof << endl;
}

//! Unload the given vector of profiles
void Pulsar::ProfileColumn::unload (int row, 
				    const std::vector<const Profile*>& prof)
{
  if (!fptr)
    throw Error (InvalidState, "Pulsar::ProfileColumn::unload",
		 "fitsfile not set");

  if (verbose)
    cerr << "Pulsar::ProfileColumn::unload"
         << " data_colnum=" << data_colnum
         << " offset_colnum=" << offset_colnum 
         << " scale_colnum=" << scale_colnum << endl;

  const bool save_signed = true;

  // due to definition of offset, max_short should be the same when
  // using either signed or unsigned
  float max_short;

  if (save_signed)
    max_short = pow(2.0,15.0)-1.0;
  else
    max_short = pow(2.0,16.0)-1.0;
  
  //= pow(2.0,16.0)-1.0;

  unsigned bins_written = 0;

  vector<float> offsets (prof.size());
  vector<float> scales (prof.size());

  for (unsigned iprof=0; iprof < prof.size(); iprof++)
  {
    const unsigned nbin = prof[iprof]->get_nbin();
    const float* amps = prof[iprof]->get_amps();

    float min = 0.0;
    float max = 0.0;
    minmax (amps, amps+nbin, min, max);

    if (save_signed)
      offsets[iprof] = 0.5 * (max + min);
    else
      offsets[iprof] = min;

    if (verbose)
      cerr << "Pulsar::ProfileColumn::unload iprof=" << iprof
	   << " offset=" << offsets[iprof] << endl;
      
    // Test for dynamic range
    if (fabs(min - max) > (100.0 * FLT_MIN))
      scales[iprof] = (max - min) / max_short;
    else if (verbose)
    {
      scales[iprof] = 1.0;
      cerr << "Pulsar::ProfileColumn::unload WARNING no range in profile"
	   << endl;
    }
    
    if (verbose)
      cerr << "Pulsar::ProfileColumn::unload iprof=" << iprof
	   << " scale = " << scales[iprof] << endl;
  }

  // offset and scale are one-dimensional arrays
  vector<unsigned> dims (1, 1);

  if (verbose)
    cerr << "Pulsar::ProfileColumn::unload writing offsets" << endl;
  psrfits_write_col (fptr, offset_colnum, row, offsets, dims);

  if (verbose)
    cerr << "Pulsar::ProfileColumn::unload writing scales" << endl;
  psrfits_write_col (fptr, scale_colnum, row, scales, dims);

  for (unsigned iprof=0; iprof < prof.size(); iprof++)
  {
    const unsigned nbin = prof[iprof]->get_nbin();
    const float* amps = prof[iprof]->get_amps();

    // 16 bit representation of profile amplitudes
    vector<int16_t> compressed (nbin);
   
    // Apply the offset and scale factor 
    for (unsigned ibin = 0; ibin < nbin; ibin++)
      compressed[ibin] = int16_t ((amps[ibin]-offsets[iprof]) / scales[iprof]);

    // Write the data
    
    if (verbose)
      cerr << "Pulsar::ProfileColumn::unload writing data"
	" iprof=" << iprof << endl;

    int status = 0;
    
    fits_write_col (fptr, TSHORT, data_colnum, row, bins_written + 1, nbin, 
		    &(compressed[0]), &status);

    if (status != 0)
      throw FITSError (status, "Pulsar::ProfileColumn::unload",
		       "fits_write_col DATA");

    bins_written += nbin;

    if (verbose)
      cerr << "Pulsar::ProfileColumn::unload iprof=" << iprof << "written" 
	   << endl;
  }
}

//! Load the given vector of profiles
void Pulsar::ProfileColumn::load (int row, const std::vector<Profile*>& prof)
{
  if (!fptr)
    throw Error (InvalidState, "Pulsar::ProfileColumn::load",
		 "fitsfile not set");

  int typecode = 0;
  long repeat = 0;
  long width = 0;
  int status = 0;
  
  fits_get_coltype (fptr, get_data_colnum(),
		    &typecode, &repeat, &width, &status);  

  if (status != 0)
    throw FITSError (status, "Pulsar::ProfileColumn::load", 
		     "fits_get_coltype " + data_colname);
    
  if (typecode == TSHORT)
    load_amps<short> (row, prof);
  else if (typecode == TFLOAT)
    load_amps<float> (row, prof);
  else
    throw Error( InvalidState, "Pulsar::ProfileColumn::load",
		 "unhandled DATA typecode=%s", fits_datatype_str(typecode) );
}

template<typename T, typename C>
void Pulsar::ProfileColumn::load_amps (int row, C& prof) try 
{
  float nullfloat = 0.0;
  
  if (verbose)
    cerr << "Pulsar::ProfileColumn::load_amps<> reading offsets" << endl;
  
  vector<float> offsets;
  psrfits_read_col (fptr, offset_colname.c_str(), offsets, row, nullfloat);

  if (verbose)
    cerr << "Pulsar::ProfileColumn::load_amps<> reading scales" << endl;

  vector<float> scales;
  psrfits_read_col (fptr, scale_colname.c_str(), scales, row, nullfloat);

  if (offsets.size() != scales.size())
    throw Error( InvalidState, "Pulsar::ProfileColumn::load_amps<>",
		 "%s size=%d != %s size=%d",
		 offset_colname.c_str(), offsets.size(),
		 scale_colname.c_str(), scales.size() );

  //
  // offset and scale array size smay be either nprof*nchan or nchan
  //
  bool nprof_by_nchan = false;
  if (offsets.size() == nprof * nchan)
  {
    if (verbose)
      cerr << "Pulsar::ProfileColumn::load_amps<> ipol scaled" << endl;
    nprof_by_nchan = true;
  }
  else if (offsets.size() == nchan)
  {
    if (verbose)
      cerr << "Pulsar::ProfileColumn::load_amps<> nprof scaled" << endl;
    nprof_by_nchan = false;
  }
  else
    throw Error( InvalidState, "Pulsar::ProfileColumn::load_amps<>",
		 "%s size=%d != nchan=%d or nchan*nprof=%d",
		 offset_colname.c_str(), offsets.size(), nchan, nchan*nprof );

  // Load the data
  
  if (verbose)
    cerr << "Pulsar::ProfileColumn::load_amps<> reading data" << endl;
  
  T null = FITS_traits<T>::null();

  int initflag = 0;
  int status = 0;  
  int counter = 1;

  unsigned namp = nprof * nchan * nbin;
  vector<T> temparray (namp);

  fits_read_col (fptr, FITS_traits<T>::datatype(),
		 get_data_colnum(), row, counter, namp, 
		 &null, &(temparray[0]), &initflag, &status);

  if (status != 0)
    throw FITSError( status, "ProfileColumn::load_amps",
		     "Error reading subint data"
		     " nprof=%u nchan=%u nbin=%u \n\t"
		     "colnum=%d firstrow=%d firstelem=%d nelements=%d",
		     nprof, nchan, nbin,
		     data_colnum, row, counter, namp );
  
  unsigned index = 0;
  for (unsigned iprof = 0; iprof < nprof; iprof++)
  {
    for (unsigned ichan = 0; ichan < nchan; ichan++)
    {
      counter += nbin;

      float scale = scales[ichan];
      float offset = offsets[ichan];

      if (nprof_by_nchan)
      {
	scale = scales[iprof*nchan + ichan];
	offset = offsets[iprof*nchan + ichan];
      }

#ifdef _DEBUG
      cerr << " iprof=" << iprof << " ichan=" << ichan
	   << " scale=" << scale << " offset=" << offset << endl;
#endif

      if (scale == 0.0)
	scale = 1.0;

      if ( !isfinite(scale) || !isfinite(offset) )
      {
	warning << "Pulsar::ProfileColumn::load_amps"
	  " SCALE or OFFSET NaN in row=" << row << endl;
	scale = offset = 0.0;
      }

      prof[index]->resize (nbin);
      float* amps = prof[index]->get_amps();

      unsigned nans = 0;

      for (unsigned ibin = 0; ibin < nbin; ibin++)
      {
	amps[ibin] = temparray[index*nbin+ibin] * scale + offset;
	if (!isfinite(amps[ibin]))
	{
	  nans ++;
	  amps[ibin] = 0.0;
	}
      }

      index ++;

      if (nans)
	warning << "Pulsar::ProfileColumn::load_amps"
	  " data NaN in row=" << row << endl;

    }  
  }
}
catch (Error& error)
{
  throw error += "Pulsar::ProfileColumn::load_amps<>";
}
