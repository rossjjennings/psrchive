#include "TimerArchive.h"
#include "TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "timer++.h"
#include "mini++.h"
#include "coord.h"


/*****************************************************************************/
void Pulsar::TimerArchive::load_header (const char* filename)
{
  if (verbose)
    cerr << "TimerArchive::load (" << filename << ")" << endl;

  FILE* fptr = fopen (filename,"r");

  if (fptr == NULL)
    throw Error (FailedSys, "TimerArchive::load", "fopen");

  if (verbose) 
    cerr << "TimerArchive::load Opened '" << filename << endl;
  
  try {
    load (fptr);
  }
  catch (Error& error) {
    fclose (fptr);
    throw error += "TimerArchive::load(" + string(filename) + ")";
  }
  fclose (fptr);

  if (verbose)
    cerr << "TimerArchive::load (" << filename << ") exit" << endl;
}


void Pulsar::TimerArchive::load (FILE* fptr)
{
  if (verbose)
    cerr << "TimerArchive::load FILE*" << endl;

  hdr_load (fptr);

  backend_load (fptr);

  ephemeris = 0;
  model = 0;

  if (get_type() == Signal::Pulsar)
    psr_load (fptr);

  subint_load (fptr);

  valid = true;

  if (verbose)
    cerr << "TimerArchive::load FILE* exit" << endl;
}


void Pulsar::TimerArchive::subint_load (FILE* fptr)
{
  if (verbose) 
    cerr << "TimerArchive::subint_load"
      " nsubint=" << hdr.nsub_int <<
      " nchan="   << hdr.nsub_band <<
      " npol="    << hdr.banda.npol <<
      " nbin="    << hdr.nbin << endl;
 
  resize (hdr.nsub_int, hdr.banda.npol, hdr.nsub_band, hdr.nbin);

  // the coordinates of the telescope are used to correct the parallactic angle
  float latitude = 0, longitude = 0;
  telescope_coordinates (&latitude, &longitude);

  string machine = hdr.machine_id;
  float version  = hdr.version + hdr.minorversion;
  bool  circular = hdr.banda.polar == 0;
  bool  baseband = machine == "CPSR" || machine == "S2" || machine == "CPSR2";

  bool reverse_U = false;
  bool reverse_V = false;

  if( string(hdr.software).find("psrdisp",0) != string::npos ) {
    // feed_offset is set to -1 by default in older versions of psrdisp
    if (baseband && version < 15.1)
      hdr.banda.feed_offset = 0.0;

    if (baseband && version < 15.1 && hdr.banda.npol==4 && circular) {
      
      if (verbose)
	cerr << "TimerArchive::subint_load correct psrdisp circular" << endl;
      
      // an unjustified sign reversal of Stokes U in psrdisp was 
      // removed in version 14.2.  It turns out that it was necessary.
      if (version >= 14.2)
	reverse_U = true;
      
      // and that Stokes V need also be inverted
      reverse_V = true;
      
      // reversing the sign of Stokes U and V (for circular feeds)
      // is equivalent to swapping Left and Right inputs.  This has
      // been done in version 15.1
      hdr.version = 15.0;
      hdr.minorversion = 0.1;
    }

    // Before psrdisp v.17.1, all archives contained Stokes IQUV
    // With v.17.1 and onward, they may or may not.
    if (baseband && version<17.1 && hdr.banda.npol==4)  {
      set_state (Signal::Stokes);
      hdr.version = 17.0;
      hdr.minorversion = 0.1;
    }
  }

  // Before psrdisp v.18.1, lower sideband data did not have their sign
  // of Stokes V properly flipped.
  // HSK: According to Willem all archives (psrdisp and baseband/dsp; Stokes and coherency) need reverse_v = true
  if (baseband && version<18.1 && hdr.banda.npol==4 && hdr.banda.bw < 0.0)  {
    hdr.version = 18.0;
    hdr.minorversion = 0.1;
    reverse_V = true;
  }    
  
  for (unsigned isub=0; isub < get_nsubint(); isub++) { 
    if (verbose)
      cerr << "TimerArchive::subint_load " 
	   << isub+1 << "/" << get_nsubint() << endl;
    
    // by over-riding Archive::new_Integration, the subints array points to
    // instances of TimerIntegrations

    TimerIntegration* subint;
    subint = dynamic_cast<TimerIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "TimerArchive::subint_load",
		   "Integration[%d] is not a TimerIntegration", isub);

    // initialize some of the subint
    init_Integration (subint);
    subint -> load (fptr, hdr.wts_and_bpass, big_endian);

    if (verbose)
      cerr << "TimerArchive::subint_load " 
	   << isub+1 << "/" << get_nsubint() << " loaded" << endl;

        
    if (subint->mini.version < 1.1) {
      // update the mini header with the integration time of the sub_int
      // added by WvS 24 Sep, 1999
      if (verbose)
	fprintf (stderr, "TimerArchive::subint_load - updating mini header version\n");
      subint->mini.integration = hdr.sub_int_time;
      subint->mini.version = 1.1;
    }

    /* CPSR psrdisp MJD error fix.  Correct misinterpretation of
     * Revision 1.4  1999/08/11 10:28:35 of pspm_search_header.h
     * in psrdisp Version 10.0  -  fix added 2000 Mar 19 */
    if (hdr.mjd < 51401 && strcmp(hdr.machine_id,"CPSR")==0 
        && version == 10.0)  {
 
      if (verbose) 
	cerr << "TimerArchive::subint_load Correcting psrdisp v10.0 MJD error"
	     << endl;
      
      double seconds_per_file = 53.6870912;

      if (isub == 0)
	Timer::set_MJD (hdr, Timer::get_MJD (hdr) - seconds_per_file);

      MJD minimjd = Mini::get_MJD (subint->mini) - seconds_per_file;
      Mini::set_MJD (subint->mini, minimjd);
      hdr.minorversion = 1.0;  // problem fixed in psrdisp v.10.1
    }

    /* nint() correction  jss */
    /***** test mjd and correct if less than NOV 94     *******/
    /***** and not already corrected && Correlator data *******/
    if (hdr.mjd < 49650 && hdr.corrected != 1 && 
	((strncmp(hdr.machine_id,"A=Jo",4) ==0)||
	 (strncmp(hdr.machine_id,"FPTM",4)==0)) ) {
      /* nint() correction  jss */
      double difftime,tcorr;
      double freqerror = 32.0e6/4.0/(pow(2.0,32.0))/2.0/1024.0;
      /* end nint() correction  jss */
      if(verbose) 
	cerr << "TimerArchive::subint_load Correcting nint error" << endl;
      difftime = subint->mini.mjd + subint->mini.fracmjd 
	- hdr.mjd - hdr.fracmjd; 
      tcorr = difftime*freqerror*subint->mini.pfold;
      subint->mini.fracmjd += tcorr;
      if (subint->mini.fracmjd > 1.0) 
	{ subint->mini.mjd++; subint->mini.fracmjd -= 1.0;}
    }
    /* end nint() correction  jss */
    
    // fix ROS 1 second error if necessary
    // This bug labels the times 1 second earlier than they should be.
    // The error occurs in archives prior to julian date 200 in 1997 (MJD 50647)
    // that were also processed before the end of 1997 (at which point
    // ROS was upgraded to a new version without the bug)

    if ( strcmp(hdr.machine_id, "S2")==0 && 
	 subint->mini.mjd<50647 && !hdr.corrected)  {
      /* get the year data was processed */
      char comment[64];
      char * key;
      char * whitespace = " \n\t";
      int year;
      strcpy(comment, hdr.comment);
      key = strtok(comment, whitespace);
      for(int ntok = 0; ntok<6; ++ntok){
	if(key==NULL) break;
	key = strtok(NULL, whitespace);
      }
      if(key==NULL || sscanf(key, "%d", &year)!=1){
	break;
      }
      if (year<=1997) {
	cerr << "TimerArchive::subint_load performing S2 MJD correction" 
	     << endl;
	MJD old_mjd = Mini::get_MJD (subint->mini);
	Mini::set_MJD (subint->mini, old_mjd + 1.0);

	if (isub == 0) {
	  old_mjd = Timer::get_MJD (hdr);
	  Timer::set_MJD (hdr, old_mjd + 1.0);
	}
      }
    } // if S2 data


    if (baseband) {
      // Correct the LST for baseband systems
      double lst_in_hours = Mini::get_MJD (subint->mini).LST (longitude);
      subint->mini.lst_start = lst_in_hours / 24.0; // lst in days
 
      if (!get_parallactic_corrected()) {
        // Correct the direction and parallactic angles
        if (verbose)
          fprintf (stderr, "TimerArchive::subint_load correcting parallactic angle\n");
        az_zen_para (hdr.ra, hdr.dec,
                     lst_in_hours, latitude,
                     &subint->mini.tel_az,
                     &subint->mini.tel_zen,
                     &subint->mini.para_angle);
      }
    }


    if (reverse_U) {
      if (verbose)
	cerr << "TimerArchive::subint_load reversing sign of ipol=2" 
	     << endl;
      for (int ichan=0; ichan<hdr.nsub_band; ichan++)
	*(subint->profiles[2][ichan]) *= -1.0;
    }
    
    if (reverse_V) {
      if (verbose)
	cerr << "TimerArchive::subint_load reversing sign of ipol=3" 
	     << endl;
      for (int ichan=0; ichan<hdr.nsub_band; ichan++)
	*(subint->profiles[3][ichan]) *= -1.0;
    }

  } // end for each sub_int

  // Weights initialized to one in subint constructor, will always be 
  // unloaded as such - MCB
  hdr.wts_and_bpass = 1;

  if (verbose) 
    fprintf(stderr, "TimerArchive::subint_load Read in %d sub_ints\n",
	    get_nsubint());
  
  // profile.wt correction - MCB
  // profile weights from the S2 set to zero during software coherent
  // dedispersion.  Here we fill in the weights with the integration
  // time if they are all zero, but leave them alone if they are not.
  if (strcmp(hdr.machine_id, "S2")==0) {

    int weights = 0;

    for (unsigned i=0; i < get_nsubint(); i++)
      for (unsigned j=0; j < get_npol(); ++j)
	for (unsigned k=0; k < get_nchan(); ++k)
	  if (get_Profile(i,j,k)->get_weight() != 0) 
	    weights = 1;

    if (weights==0) {
      cerr << "TimerArchive::subint_load"
	"replacing profile weights by sub int time" << endl;
      
      for(unsigned i=0;i<get_nsubint();i++)
	for(unsigned j=0; j<get_npol(); ++j)
	  for(unsigned k=0; k<get_nchan(); ++k)
	    get_Profile(i,j,k)->set_weight(get_Integration(i)->get_duration());
    }
  }

  // Weight correction and calibration for FB archives
  // RNM Feb 17, 2000
  if(strcmp(hdr.machine_id, "FB")==0){
    int weights = 0; float nch_sub, tdmp;  double chbw;
    tdmp = hdr.sub_int_time / hdr.ndump_sub_int;
    chbw = fabs(hdr.banda.bw)/hdr.banda.nlag;

    for(unsigned i=0; i < get_nsubint(); i++) {    
      nch_sub = (float)hdr.banda.nlag / (float)get_nchan();
      for(unsigned j=0; j<get_npol(); ++j){
	for(unsigned k=0; k<get_nchan(); ++k){
	  if(verbose)printf("wt,nch_sub,tdmp: %f %f %f\n",
	       get_Profile(i,j,k)->get_weight(),nch_sub,tdmp);
	  if(get_Profile(i,j,k)->get_weight() > nch_sub*hdr.ndump_sub_int) weights = 1;
	}
      }
    }
    
    if(hdr.wts_and_bpass == 0 || weights == 0){
      if(verbose) 
	cerr << "Multiply FB wts by dump time:" << tdmp << endl;
      for(unsigned i=0;i<get_nsubint();i++)
	for(unsigned j=0; j<get_npol(); ++j)
	  for(unsigned k=0; k<get_nchan(); ++k) {
	    Profile* profile = get_Profile(i,j,k);
	    profile->set_weight( profile->get_weight() * tdmp );
	  }
    }
    if(hdr.calibrated == 0){
      if(verbose) 
	cerr << "Setting FB calibration" << endl;
      for(unsigned i=0;i<get_nsubint();i++)
	for(unsigned j=0; j<get_npol(); ++j)
	  for(unsigned k=0; k<get_nchan(); ++k){
	    // Scale to counts per sample and by sqrt of channel bandwidth 
	    // and sample time. Data were summed for original dump time and  
	    // this scaling is preserved by tscrunch and fscrunch.
	    *get_Profile(i,j,k) *= hdr.tsmp / tdmp * sqrt(chbw * hdr.tsmp);
	  }
      if(verbose) 
	cerr << "ndmp:" <<hdr.ndump_sub_int << " tsmp:" << hdr.tsmp 
	     << " tsub_int:" << hdr.sub_int_time
	     << " chbw: " << chbw << endl;
      hdr.calibrated = 1;
    }
  }
  
  // Set flag so weights always unloaded
  hdr.wts_and_bpass = 1;

  /* set a flag to indicate nint() correction has been done */
  /* don't worry about the s2 hdr.corrected - no data taken until 1996 */
  if (hdr.mjd < 49650 && hdr.corrected != 1) hdr.corrected =1;
  
  /* set a flag to indicate ROS 1 second error has been corrected */
  if (strcmp(hdr.machine_id, "S2")==0 && !hdr.corrected) hdr.corrected = 1;

  if (verbose) 
    cerr << "TimerArchive::subint_load - exiting" << endl;
}


void Pulsar::TimerArchive::hdr_load (FILE* fptr)
{
  if (verbose)
    cerr << "TimerArchive::hdr_load reading timer header" << endl;

  if (Timer::load (fptr, &hdr, big_endian) < 0)
    throw Error (FailedCall, "TimerArchive::hdr_load", "Timer::load");

  hdr.banda.npol = Timer::get_npol (hdr);
}

/*!
  Unless this method is overloaded by the appropriate sub-class, all
  backend-specific information will be lost when the file is loaded.
*/
void Pulsar::TimerArchive::backend_load (FILE* fptr)
{
  unsigned long backend_offset = Timer::backend_data_size(hdr);

  if (verbose)
    cerr << "TimerArchive::backend_load offset=" << backend_offset << endl;

  if (backend_offset == 0)
    return;

  // simply ignore the backend-specific information
  int ret = fseek (fptr, backend_offset, SEEK_CUR);
  if (ret != 0)
    throw Error (FailedSys, "TimerArchive::backend_load", "fseek");

  // all backend-specific information is lost
  strcpy (hdr.backend, "NONE");
  hdr.be_data_size = 0;
}

void Pulsar::TimerArchive::psr_load (FILE* fptr)
{
  if (hdr.nbytespoly > 0) {
    if (verbose)
      cerr << "TimerArchive::psr_load "
	   << hdr.nbytespoly << " bytes in polyco" << endl;

    model = new polyco;

    if (model->load (fptr, hdr.nbytespoly) <= 0)
      throw Error (FailedCall, "TimerArchive::psr_load", "polyco::load");

    if (verbose) {
      cerr << "TimerArchive::psr_load read in polyco:\n" << *model << endl;
      cerr << "TimerArchive::psr_load end of polyco" << endl;
    }
  }
  else {
    if (verbose)
      cerr << "TimerArchive::psr_load no polyco" << endl;
  }

  if (hdr.nbytesephem > 0) {
    if (verbose) 
      cerr << "TimerArchive::psr_load "
	   << hdr.nbytesephem << " bytes in ephemeris" << endl;

    ephemeris = new psrephem;

    if (ephemeris->load (fptr, hdr.nbytesephem) < 0)
      throw Error (FailedCall, "TimerArchive::psr_load", "psrephem::load");
    
    if (verbose) {
      cerr << "TimerArchive::psr_load read in psrephem:\n"<< *ephemeris 
	   << endl;
      cerr << "TimerArchive::psr_load end of psrephem" 
	   << endl;
    }
  }
  else {
    if (verbose)
      cerr << "TimerArchive::psr_load no ephemeris" << endl;
  }
}
