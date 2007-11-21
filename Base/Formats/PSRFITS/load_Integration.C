/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/CalInfoExtension.h"

#include "Pulsar/Predictor.h"

#include "psrfitsio.h"

using namespace std;

// //////////////////////////////////////////////////////////////////

//! A function to read a single integration from a FITS file on disk.
/*! This function assumes that the Integration will have the global
  attributes of the file. */
Pulsar::Integration* 
Pulsar::FITSArchive::load_Integration (const char* filename, unsigned isubint)
try {

  if (!filename)
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "filename unspecified");

  Reference::To<Pulsar::Integration> integ = new_Integration();
  init_Integration (integ);

  int row = isubint + 1;
  
  int status = 0;  

  if (verbose > 2)
    cerr << "FITSArchive::load_Integration number " << isubint << endl;
  
  double nulldouble = 0.0;
  float nullfloat = 0.0;
  
  int initflag = 0;
  int colnum = 0;
  
  // Open the file

  fitsfile* fptr = 0;
  
  if (verbose > 2)
    cerr << "FITSArchive::load_Integration fits_open_file (" 
	 << filename << ")" << endl;
  
  fits_open_file(&fptr, filename, READONLY, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_open_file(%s)", filename);
  

  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_movnam_hdu SUBINT");


  // Load the convention for the epoch definition
  string epoch_def;
  string default_def = "STT_MJD";
  psrfits_read_key (fptr, "EPOCHS", &epoch_def,
		    default_def, verbose > 2);

  if (verbose > 2)
    cerr << "FITSArchive::load_Integration epochs are " << epoch_def << endl;

  // By default, correct epochs using the phase model
  bool correct_epoch_phase = true;

  // By default, correct epochs such that phase(epoch)=phase(start)
  bool phase_match_start_time = true;

  if (epoch_def == "VALID")
    correct_epoch_phase = phase_match_start_time = false;

  if (epoch_def == "MIDTIME")
    phase_match_start_time = false;

  if (get<Pulsar::IntegrationOrder>()) {

    colnum = 0;
    fits_get_colnum (fptr, CASEINSEN, "INDEXVAL", &colnum, &status);
    
    double value = 0.0;

    fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		   &value, &initflag, &status);
    
    if (status != 0)
      throw FITSError (status, "FITSArchive::load_Integration", 
		       "fits_read_col INDEXVAL");
    
    get<Pulsar::IntegrationOrder>()->set_Index(row-1,value);
  }
  
  // Get the reference epoch from the primary header
  const Pulsar::FITSHdrExtension* hdr_ext = get<Pulsar::FITSHdrExtension>();
  
  if (!hdr_ext) {
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "No FITSHdrExtension found");
  }
  
  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  double duration = 0.0;
  
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &duration, &initflag, &status);
  
  integ->set_duration (duration);

  // Set the start time of the integration
  
  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (fptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  double time = 0.0;
  fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &time, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_read_col OFFS_SUB");


  MJD epoch = hdr_ext->start_time + time;

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration reference_epoch=" 
	 << hdr_ext->start_time << "\n  offset=" << time << "s epoch=" << epoch
	 << endl;
  
  // Set a preliminary epoch to avoid problems loading the polyco
  integ->set_epoch (epoch);

  // Set the folding period to 0 until one of three possible methods succeeds
  integ->set_folding_period (0.0);

  /* **********************************************************************

     METHOD 1: folding period defined by a pulse phase model

     ********************************************************************** */

  if (hdr_model) {

    // Set the folding period, using the polyco from the file header
    // This was taken out of the condition clause below because period
    // wasn't set when TSUB was 0
    integ->set_folding_period (1.0 / hdr_model->frequency(epoch));

    if (integ->get_folding_period() <= 0.0)
      throw Error( InvalidState, "Pulsar::FITSArchive::load_Integration",
		   "header polyco/predictor corrupted; "
		   "period(epoch=%s)=%lf", epoch.printdays(5).c_str(),
		   integ->get_folding_period() );

    if (integ->get_folding_period() < 1.0e-3)
      warning << "Pulsar::FITSArchive::load_Integration folding_period=" 
	      << integ->get_folding_period() << " is less than 1ms" << endl;

    else if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_Integration folding_period = "
      	   << integ->get_folding_period () << endl;

    if (duration && correct_epoch_phase)
    {

      if (verbose > 2)
	cerr << "Pulsar::FITSArchive::load_Integration correcting epoch phase"
	     << endl;

      Phase reference_phs = 0.0;

      if (phase_match_start_time)
      {
	// Correct epoch such that its phase equals that of the start time
	if (verbose > 2)
	  cerr << "Pulsar::FITSArchive::load_Integration matching phase(start)"
	       << endl;

	reference_phs = hdr_model->phase(hdr_ext->start_time);
      }

      Phase off_phs = hdr_model->phase(epoch);
      Phase dphase  = off_phs - reference_phs;
      
      double dtime = dphase.fracturns() * integ->get_folding_period();
      epoch -= dtime;
      integ->set_epoch (epoch);

      if (verbose > 2)
      {
      	cerr << "Pulsar::FITSArchive::load_Integration row=" << row <<
	  "\n  PRED_PHS=" << predicted_phase;
	if (phase_match_start_time)
	  cerr << "\n  reference epoch=" << hdr_ext->start_time;
	cerr <<
	  "\n  reference phase=" << reference_phs <<
	  "\n      input phase=" << off_phs <<
	  "\n     phase offset=" << dphase << " = " << dtime << "s" 
	  "\n     subint epoch=" << epoch << 
	  "\n     subint phase=" << hdr_model->phase(epoch) << endl;
      }

    }

  }
  else
  {

    /* *******************************************************************

       METHOD 2: folding period defined by CAL_FREQ in primary header

       ******************************************************************* */

    CalInfoExtension* calinfo = get<CalInfoExtension>();
    if (calinfo && calinfo->cal_frequency > 0.0)
    {
      if (verbose > 2)
        cerr << "FITSArchive::load_Integration CAL_FREQ=" 
	     << calinfo->cal_frequency << endl;
      integ->set_folding_period( 1.0/calinfo->cal_frequency );
    }

    /* *******************************************************************

       METHOD 3: folding period defined by PERIOD column of SUBINT HDU

       ******************************************************************* */

    double period = 0.0;
    status = 0;
    fits_get_colnum (fptr, CASEINSEN, "PERIOD", &colnum, &status);
    fits_read_col (fptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
                   &period, &initflag, &status);

    if (status == 0 && period > 0.0)
    {
      if (verbose > 2)
	cerr << "FITSArchive::load_Integration PERIOD=" << period << endl;
      integ->set_folding_period (period);
    }

    if (integ->get_folding_period() == 0.0)
      throw FITSError (status, "FITSArchive::load_Integration",
                       "folding period unknown: no model, CAL_FREQ or PERIOD");
  }

  // Load other useful info

  load_Pointing(fptr,row,integ);

  // Set up the data vector, only Pulsar::Archive base class is friend

  resize_Integration (integ);

  // Load the channel centre frequencies
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration reading channel freqs" 
	 << endl;
  
  int counter = 1;
  vector < float >  chan_freqs(get_nchan());
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  fits_read_col (fptr, TFLOAT, colnum, row, counter, get_nchan(),
		 &nullfloat, &(chan_freqs[0]), &initflag, &status);
  
  // Set the profile channel centre frequencies
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration setting frequencies" 
	 << endl;

  bool all_ones = true;
  for (unsigned j = 0; j < get_nchan(); j++)
    if (chan_freqs[j] != 1)
      all_ones = false;
  
  double chanbw = get_bandwidth() / get_nchan();
  
  if ( all_ones ) {
    if (verbose > 2)
      cerr << "FITSArchive::load_Integration all frequencies unity - reseting"
	   << endl;
    for (unsigned j = 0; j < get_nchan(); j++) {
      integ->set_centre_frequency (j, get_centre_frequency()
				   -0.5*(get_bandwidth()+chanbw)+j*chanbw);
    }
  }
  else
    for (unsigned j = 0; j < get_nchan(); j++) {
      //integ->set_frequency(j, chan_freqs[j]);
      integ->set_centre_frequency(j, chan_freqs[j]);
    }
  
  // Load the profile weights

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration reading weights" 
	 << endl;
  
  counter = 1;
  vector < float >  weights(get_nchan());
  
  colnum = 0;
  fits_get_colnum (fptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  for(unsigned b = 0; b < get_nchan(); b++) {
    fits_read_col (fptr, TFLOAT, colnum, row, counter, 1, &nullfloat, 
		   &weights[b], &initflag, &status);
    counter ++;
  }
  
  // Set the profile weights
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration setting weights" 
	 << endl;
  
  for(unsigned j = 0; j < get_nchan(); j++)
    integ->set_weight(j, weights[j]);
  
  // Load the profile offsets
  
  if (!Profile::no_amps)
  {
    colnum = 0;
    fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
  
    int typecode = 0;
    long repeat = 0;
    long width = 0;
  
    fits_get_coltype (fptr, colnum, &typecode, &repeat, &width, &status);  

    if (status != 0)
      throw FITSError (status, "FITSArchive::load_Integration", 
		       "fits_get_coltype DATA");
    
    if (typecode == TSHORT)
      load_amps<short> (fptr, integ, isubint, colnum);
    else if (typecode == TFLOAT)
      load_amps<float> (fptr, integ, isubint, colnum);
    else
      throw Error( InvalidState, "FITSArchive::load_Integration",
		   "unhandled DATA typecode=%s", fits_datatype_str(typecode) );

  }
  else // Profile::no_amps
  {
    // as far as I can tell this is all the above does if we're not
    // interested in filling the profile amps -- redwards
    Signal::Component polmeas = Signal::None;
    for (unsigned a = 0; a < get_npol(); a++) {
      for (unsigned b = 0; b < get_nchan(); b++) {
	integ->get_Profile(a,b)->set_state(polmeas);
      }
    }
  }

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_Integration load complete" << endl;  
  
  // Finished with the file for now
  
  fits_close_file (fptr, &status);

  return integ.release();
}
catch (Error& error) {
  throw error += "Pulsar::FITSArchive::load_Integration";
}


template<typename T>
void Pulsar::FITSArchive::load_amps (fitsfile* fptr,
				     Integration* integ,
				     unsigned isubint,
				     int data_colnum)
try {

  unsigned nchan = get_nchan();
  unsigned npol = get_npol();
  unsigned nbin = get_nbin();

  float nullfloat = 0.0;
  int row = isubint + 1;
  
  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_amps<> reading offsets" << endl;
  
  vector<float> offsets;
  psrfits_read_col (fptr, "DAT_OFFS", offsets, row, nullfloat);

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_amps<> reading scales" << endl;

  vector<float> scales;
  psrfits_read_col (fptr, "DAT_SCL", scales, row, nullfloat);

  if (offsets.size() != scales.size())
    throw Error( InvalidState, "Pulsar::FITSArchive::load_amps<>",
		 "DAT_OFFS size=%d != DAT_SCL size=%d",
		 offsets.size(), scales.size() );

  //
  // the DAT_SCL and DAT_OFFS array size may be either npol*nchan or nchan
  //
  bool npol_by_nchan = false;
  if (offsets.size() == npol * nchan)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_amps<> ipol scaled" << endl;
    npol_by_nchan = true;
  }
  else if (offsets.size() == nchan)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_amps<> npol scaled" << endl;
    npol_by_nchan = false;
  }
  else
    throw Error( InvalidState, "Pulsar::FITSArchive::load_amps<>",
		 "DAT_OFFS/DAT_SCL size=%d != nchan=%d or nchan*npol=%d",
		 offsets.size(), nchan, nchan*npol );

  // Load the data
  
  vector<T> temparray (nbin);
  vector<float> fltarray (nbin);

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::load_amps<> reading data" << endl;
  
  T null = FITS_traits<T>::null();

  int initflag = 0;
  int status = 0;  
  int counter = 1;

  for (unsigned ipol = 0; ipol < npol; ipol++) {
    for (unsigned ichan = 0; ichan < nchan; ichan++) {
    
      Profile* p = integ->get_Profile(ipol,ichan);
      
      fits_read_col (fptr, FITS_traits<T>::datatype(),
		     data_colnum, row, counter, nbin, 
		     &null, &(temparray[0]), &initflag, &status);

      if (status != 0)
	throw FITSError( status, "FITSArchive::load_amps",
			 "Error reading subint data"
			 " ipol=%d/%d ichan=%d/%d\n\t"
			 "colnum=%d firstrow=%d firstelem=%d nelements=%d",
			 ipol, npol, ichan, nchan, 
			 data_colnum, row, counter, nbin );
      
      counter += nbin;

      float scale = scales[ichan];
      float offset = offsets[ichan];

      if (npol_by_nchan)
      {
	scale = scales[ipol*nchan + ichan];
	offset = offsets[ipol*nchan + ichan];
      }

#ifdef _DEBUG
      cerr << " ipol=" << ipol << " ichan=" << ichan
	   << " scale=" << scale << " offset=" << offset << endl;
#endif

      if (scale == 0.0)
	scale = 1.0;

      for(unsigned j = 0; j < nbin; j++) {
	fltarray[j] = temparray[j] * scale + offset;
	if (integ->get_state() == Signal::Coherence) {
	  if (scale_cross_products && (ipol == 2 || ipol == 3))
	    fltarray[j] *= 2;
	}
      }
      
      p->set_amps(fltarray);
      p->set_state(Signal::None);
      
    }  
  }
}
catch (Error& error) {
  throw error += "Pulsar::FITSArchive::load_amps<>";
}

