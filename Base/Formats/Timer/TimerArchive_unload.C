#include "TimerArchive.h"
#include "TimerIntegration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "timer++.h"
#include "mini++.h"
#include "coord.h"

/*****************************************************************************/
void Pulsar::TimerArchive::unload_file (const char* filename) const
{
  // TimerArchives do not support alternate ordering schemes
  bool has_alt_order = false;
  
  for (unsigned i = 0; i < extension.size(); i++) {
    if (dynamic_cast<Pulsar::IntegrationOrder*>(extension[i].get()))
      has_alt_order = true;
  }
  
  if (has_alt_order)
    throw Error(InvalidState, "Archive::add_extension",
		"The TimerArchive class does not support unloading of files with"
		" alternate IntegrationOrder extensions");
  
  FILE* fptr = fopen (filename, "w");
  if (!fptr)
    throw Error (FailedSys, "TimerArchive::unload", "fopen");

  if (verbose == 3) 
    cerr << "TimerArchive::unload opened '" << filename << "'" << endl;
  
  try {
    unload (fptr);
  }
  catch (Error& error) {
    fclose (fptr);
    throw error += "TimerArchive::unload(" + string(filename) + ")";
  }
  fclose (fptr);
}


void Pulsar::TimerArchive::unload (FILE* fptr) const
{
  pack_extensions ();

  hdr_unload (fptr);

  backend_unload (fptr);

  if (get_type() == Signal::Pulsar)
    psr_unload (fptr);

  subint_unload (fptr);
}


void Pulsar::TimerArchive::subint_unload (FILE* fptr) const
{
  if (verbose == 3) 
    cerr << "TimerArchive::unload"
      " nsubint=" << hdr.nsub_int <<
      " nchan="   << hdr.nsub_band <<
      " npol="    << hdr.banda.npol <<
      " nbin="    << hdr.nbin << endl;
 
  for (int isub=0; isub < hdr.nsub_int; isub++) { 

    if (verbose == 3)
      cerr << "TimerArchive::subint_unload " 
	   << isub+1 << "/" << hdr.nsub_int << endl;

    // by over-riding Archive::new_Integration, the subints array points to
    // instances of TimerIntegrations

    const TimerIntegration* subint;
    subint = dynamic_cast<const TimerIntegration*>(get_Integration(isub));
    if (!subint)
      throw Error (InvalidState, "TimerArchive::subint_unload",
		   "subints[%d] is not a TimerIntegration", isub);

    subint -> unload (fptr);

    if (verbose == 3)
      cerr << "TimerArchive::subint_unload " 
	   << isub+1 << "/" << hdr.nsub_int << " unloaded" << endl;

  }        

  if (verbose == 3) 
    cerr << "TimerArchive::subint_unload exit\n";
}


void Pulsar::TimerArchive::hdr_unload (FILE* fptr) const
{
  string text;

  if (!valid) {
    if (verbose == 3)
      cerr << "TimerArchive::hdr_unload correcting archive" << endl;
    const_cast<TimerArchive*>(this)->correct();
  }

  struct timer* header = const_cast<struct timer*> (&hdr);


  if (!model)
    header->nbytespoly = 0;

  else {

    if (verbose == 3) cerr << "TimerArchive::hdr_unload get polyco size" << endl;

    header->nbytespoly = model->unload (&text);
    if (hdr.nbytespoly < 0)
      throw Error (FailedCall, "TimerArchive::hdr_unload", "polyco::unload");
    
    if (verbose == 3) cerr << "TimerArchive::hdr_unload polyco size = " 
		      << hdr.nbytespoly << " bytes" << endl;

  }

  if (!ephemeris)
    header->nbytesephem = 0;

  else {

    if (verbose == 3) cerr << "TimerArchive::hdr_unload get psrephem size" << endl;
    
    header->nbytesephem = ephemeris->unload (&text);
    if (hdr.nbytesephem < 0)
      throw Error (FailedCall, "TimerArchive::hdr_unload", "psrephem::unload");
    
    if (verbose == 3) cerr << "TimerArchive::hdr_unload psrephem size = " 
		      << hdr.nbytesephem << " bytes" << endl;

  }

  if (get_nsubint() == 1)
    header->sub_int_time = integration_length();

  if (verbose == 3)
    cerr << "TimerArchive::hdr_unload writing timer header" << endl;

  if (Timer::unload (fptr, hdr) < 0)
    throw Error (FailedCall, "TimerArchive::hdr_unload", "Timer::unload");
}

void Pulsar::TimerArchive::backend_unload (FILE* fptr) const
{
  // nothing to do
}

void Pulsar::TimerArchive::psr_unload (FILE* fptr) const
{
  if (model && hdr.nbytespoly > 0) {
    if (verbose == 3)
      cerr << "TimerArchive::psr_unload "
	   << hdr.nbytespoly << " bytes in polyco" << endl;

    if (model->unload (fptr) != hdr.nbytespoly)
      throw Error (FailedCall, "TimerArchive::psr_unload",
		   "polyco::unload != %d bytes", hdr.nbytespoly);
  }

  if (ephemeris && hdr.nbytesephem > 0) {
    if (verbose == 3) 
      cerr << "TimerArchive::psr_unload "
	   << hdr.nbytesephem << " bytes in ephemeris" << endl;

    if (ephemeris->unload (fptr) != hdr.nbytesephem)
      throw Error (FailedCall, "TimerArchive::psr_unload",
		   "psrephem::unload != %d bytes", hdr.nbytesephem);
  }
}












