#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Pointing.h"

#include "FITSError.h"

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
//! A function to read a single integration from a FITS file on disk.
/*! This function assumes that the Integration will have the global attributes
  of the file. */
Pulsar::Integration* 
Pulsar::FITSArchive::load_Integration (const char* filename, unsigned isubint)
try {

  if (!filename)
    throw Error (InvalidParam, "FITSArchive::load_Integration",
		 "filename unspecified");

  Pulsar::Integration* integ = new_Integration();

  int row = isubint + 1;
  
  int status = 0;  

  if (verbose == 3) {
    cerr << "FITSArchive::load_Integration number " << isubint << endl;
  }
  
  double nulldouble = 0.0;
  float nullfloat = 0.0;
  int16 nullshort = -1;
  
  int initflag = 0;
  int colnum = 0;
  
  // Open the file

  fitsfile* sfptr = 0;
  
  if (verbose == 3)
    cerr << "FITSArchive::load_Integration fits_open_file (" 
	 << filename << ")" << endl;
  
  fits_open_file(&sfptr, filename, READONLY, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_open_file(%s)", filename);
  

  // Move to the SUBINT Header Data Unit
  
  fits_movnam_hdu (sfptr, BINARY_TBL, "SUBINT", 0, &status);
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_movnam_hdu SUBINT");

  init_Integration ( integ );

  if (get<Pulsar::IntegrationOrder>()) {
    colnum = 0;
    fits_get_colnum (sfptr, CASEINSEN, "INDEXVAL", &colnum, &status);
    
    double value = 0.0;

    fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		   &value, &initflag, &status);
    
    if (status != 0)
      throw FITSError (status, "FITSArchive::load_Integration", 
		       "fits_read_col INDEXVAL");
    
    get<Pulsar::IntegrationOrder>()->set_Index(row-1,value);
  }
  
  // Set the duration of the integration
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "TSUBINT", &colnum, &status);
  
  double duration = 0.0;
  
  fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &duration, &initflag, &status);
  
  integ->set_duration (duration);
  
  // Set the start time of the integration
  
  initflag = 0;
  colnum = 0;
  
  fits_get_colnum (sfptr, CASEINSEN, "OFFS_SUB", &colnum, &status);
  
  double time = 0.0;
  MJD newmjd;
  
  fits_read_col (sfptr, TDOUBLE, colnum, row, 1, 1, &nulldouble,
		 &time, &initflag, &status);
  
  if (status != 0)
    throw FITSError (status, "FITSArchive::load_Integration", 
		     "fits_read_col OFFS_SUB");

  newmjd = reference_epoch + time;
  
  // Set the folding period

  if (model && duration) {

    integ->set_folding_period (model->period(newmjd));

    // Set the toa epoch, correcting for phase offset

    Phase stt_phs = model->phase(reference_epoch);
    Phase off_phs = model->phase(newmjd);

    Phase dphase = off_phs - stt_phs;
  
    newmjd -= dphase.fracturns() * integ->get_folding_period();
  
    integ->set_epoch (newmjd);
  
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_Integration set_epoch " 
	   << newmjd << endl;
  }


  // Load other useful info

  load_Pointing(sfptr,row,integ);

  // Set up the data vector, only Pulsar::Archive base class is friend

  resize_Integration (integ);

  // Load the channel centre frequencies
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration reading channel freqs" 
	 << endl;
  
  int counter = 1;
  vector < float >  chan_freqs(get_nchan());
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_FREQ", &colnum, &status);
  
  fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		 &(chan_freqs[0]), &initflag, &status);
  
  // Set the profile channel centre frequencies
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration setting channel freqs" 
	 << endl;

  bool all_ones = true;
  for (unsigned j = 0; j < get_nchan(); j++)
    if (chan_freqs[j] != 1)
      all_ones = false;
  
  double chanbw = get_bandwidth() / get_nchan();
  
  if ( all_ones ) {
    if (verbose == 3)
      cerr << "FITSArchive::load_Integration all frequencies unity - reseting" << endl;
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

  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration reading weights" 
	 << endl;
  
  counter = 1;
  vector < float >  weights(get_nchan());
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_WTS", &colnum, &status);
  
  for(unsigned b = 0; b < get_nchan(); b++) {
    fits_read_col (sfptr, TFLOAT, colnum, row, counter, 1, &nullfloat, 
		   &weights[b], &initflag, &status);
    counter ++;
  }
  
  // Set the profile weights
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration setting weights" 
	 << endl;
  
  for(unsigned j = 0; j < get_nchan(); j++)
    integ->set_weight(j, weights[j]);
  
  // Load the profile offsets
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration reading offsets" 
	 << endl;
  
  vector < vector < float > > offsets(get_npol(),vector<float>(get_nchan()));
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_OFFS", &colnum, &status);
  
  counter = 1;
  for (unsigned a = 0; a < get_npol(); a++) {
    fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		   &(offsets[a][0]), &initflag, &status);
    counter += nchan;
  }

  // Load the profile scale factors
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration reading scale factors" 
	 << endl;

  vector < vector < float > > scales(get_npol(),vector<float>(get_nchan()));

  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DAT_SCL", &colnum, &status);
  
  counter = 1;
  for (unsigned a = 0; a < get_npol(); a++) {
    fits_read_col (sfptr, TFLOAT, colnum, row, counter, get_nchan(), &nullfloat, 
		   &(scales[a][0]), &initflag, &status);
    counter += nchan;
  }

  // Load the data
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration reading profiles" 
	 << endl;
  
  counter = 1;
  Profile* p = 0;
  int16* temparray = new int16 [get_nbin()];
  float* fltarray = new float [get_nbin()];
  Signal::Component polmeas = Signal::None;
  
  colnum = 0;
  fits_get_colnum (sfptr, CASEINSEN, "DATA", &colnum, &status);
  
  int typecode = 0;
  long repeat = 0;
  long width = 0;
  
  fits_get_coltype (sfptr, colnum, &typecode, &repeat, &width, &status);  

  for (unsigned a = 0; a < get_npol(); a++) {
    for (unsigned b = 0; b < get_nchan(); b++) {
      
      p = integ->get_Profile(a,b);
      
      fits_read_col (sfptr, TSHORT, colnum, row, counter, get_nbin(), 
		     &nullshort, temparray, &initflag, &status);
      
      if (status != 0) {
	throw FITSError (status, "FITSArchive::load_Integration",
			 "Error reading subint data"
			 " ipol=%d/%d ichan=%d/%d counter=%d",
			 a, get_npol(), b, get_nchan(), counter);
      }
      
      counter += get_nbin();
      
      for(unsigned j = 0; j < get_nbin(); j++) {
	fltarray[j] = temparray[j] * scales[a][b] + offsets[a][b];
	if (scale_cross_products) {
	  if (integ->get_state() == Signal::Coherence) {
	    if (a == 2 || a == 3)
	      fltarray[j] *= 2;
	  }
	}
      }
      
      p->set_amps(fltarray);
      p->set_state(polmeas);

    }  
  }
  
  delete [] temparray; 
  delete [] fltarray;
  
  if (verbose == 3)
    cerr << "Pulsar::FITSArchive::load_Integration load complete" << endl;  
  
  // Finished with the file for now
  
  fits_close_file(sfptr,&status);

  return integ;
}
catch (Error& error) {
  throw error += "Pulsar::FITSArchive::load_Integration";
}
