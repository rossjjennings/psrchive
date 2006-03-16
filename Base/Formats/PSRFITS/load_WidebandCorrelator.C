#include "Pulsar/FITSArchive.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"
#include "psrfitsio.h"
#include "string_utils.h"

void Pulsar::FITSArchive::load_WidebandCorrelator (fitsfile* fptr)
{
  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator" << endl;
  
  Reference::To<WidebandCorrelator> ext = new WidebandCorrelator;

  {
    string temp;
    string dfault;

    if (verbose == 3)
      cerr << "FITSArchive::load_WidebandCorrelator instrument name" << endl;
    
    psrfits_read_key (fptr, "BACKEND", &temp, dfault, verbose == 3);
    ext->set_name(temp);
    
    if (verbose == 3)
      cerr << "FITSArchive::load_WidebandCorrelator instrument config" << endl;
    
    psrfits_read_key (fptr, "BECONFIG", &temp, dfault, verbose == 3);
    ext->configfile = temp;
  }

  {
    int temp;
    psrfits_read_key (fptr, "BE_PHASE", &temp, 0, verbose == 3);
    ext->set_argument( (Signal::Argument) temp );
    
    // psrfits_read_key (fptr, "BE_HAND",  &temp, 1, verbose == 3);
    // ext->set_hand( (Signal::Hand) temp );
  }

  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator reading cycle time" << endl;
  
  psrfits_read_key (fptr, "TCYCLE", &(ext->tcycle), 0.0, verbose == 3);
 
  if (verbose == 3)
    cerr << "FITSArchive::load_WidebandCorrelator reading NRCVR" << endl;
  
  psrfits_read_key (fptr, "NRCVR", &(ext->nrcvr), 0, verbose == 3);

  add_extension (ext);

  if (ext->get_name() != "WBCORR")
    return;

  // check for corrections

  FITSHdrExtension* hdr_ext = get<FITSHdrExtension>();
  if (!hdr_ext)
    throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
		 "no FITSHdrExtension extension");

  int major=-1, minor=-1;

  if (sscanf (hdr_ext->hdrver.c_str(), "%d.%d", &major, &minor) != 2)
    throw Error (InvalidParam, "FITSArchive::load_WidebandCorrelator",
		 "could not parse header version from " + hdr_ext->hdrver);
  
  if (major = 1 && minor < 10)  {
    
    /*
      Prior to header version 1.10, the WBCORR backend at Parkes did
      not properly set the reference_epoch.
    */
    
    correct_P236_reference_epoch = true;
    if (verbose > 1)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	"  correcting reference epoch of P236 data with version " 
	   << hdr_ext->hdrver << endl;
    
  }
  
  if (major = 1 && minor < 14 && minor > 9)  {
    
    /*
      Prior to header version 1.14, the WBCORR backend at Parkes
      produced cross products that were out by a scale factor of
      two. This little check applies the correction factor if it detects
      an archive that was affected by this instrumentation bug.
    */
    
    scale_cross_products = true;
    if (verbose == 3)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator "
	"doubling cross products of WBCORR data with version " 
	   << hdr_ext->hdrver << endl;
    
  }
  
  if (ext->get_argument() == (Signal::Argument) 0) {
    
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	"  undefined BE_PHASE; checking " << ext->configfile; // << endl;
    
    static char* conj_config = getenv ("WBCCONJCFG");
    static vector<string> conj_configs;
    if (conj_config)  {
      stringfload (&conj_configs, conj_config);
      conj_config = 0; // load it only once
      if (verbose==3 && conj_configs.size()) {
	cerr << "WBCORR conj configurations:" << endl;
	for (unsigned i=0; i < conj_configs.size(); i++)
	  cerr << conj_configs[i] << endl;
      }
    }
    
    if (find (conj_configs.begin(), conj_configs.end(), ext->configfile)
	!= conj_configs.end())  {
      
      ext->set_argument( Signal::Conjugate );
      
      if (verbose > 1)
	cerr << "Pulsar::FITSArchive::load_WidebandCorrelator\n"
	  "  correcting data with version=" << hdr_ext->hdrver
	     << " config=" << ext->configfile << endl;
      
    }
    
  }
  
}

