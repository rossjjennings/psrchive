#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"
#include "FITSError.h"

/*
  string name = FRONTEND
  Signal::Basis basis = FD_POLN;
  Angle X_offset = XPOL_ANG;
  Tracking mode = FD_MODE;
  Angle Y_offset = not yet;
  Angle calibrator_orientation = not yet;
  bool feed_offset_corrected = not yet;
  bool vertical_offset_corrected = HISTORY::PAR_CORR;
  float atten_a = ATTEN_A;
  float atten_b = ATTEN_B;
  
  FA_REQ ?
*/

void Pulsar::FITSArchive::load_Receiver (fitsfile* fptr)
{
  // status returned by FITSIO routines
  int status = 0;
  // no comment
  char* comment = 0;

  if (verbose)
    cerr << "FITSArchive::load_Receiver" << endl;
  
  Reference::To<Receiver> ext = new Receiver;

  auto_ptr<char> tempstr ( new char[FLEN_VALUE] );

  // Receiver name

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading receiver" << endl;

  fits_read_key (fptr, TSTRING, "FRONTEND", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FRONTEND").warning() << endl;
    status = 0;
    ext->name = "unknown";
  }
  else
    ext->name = tempstr.get();
  
  // Read the feed configuration

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading feed config" << endl;

  fits_read_key (fptr, TSTRING, "FD_POLN", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FD_POLN").warning() << endl;
    status = 0;
  }
  else {
    if (strncasecmp(tempstr.get(),"LIN",3) == 0)
      ext->basis = Signal::Linear;
    else if (strncasecmp(tempstr.get(),"CIRC",4) == 0)
      ext->basis = Signal::Circular;
    else
      throw Error (InvalidParam, "FITSArchive::load_Receiver",
	           "unknown FD_POLN: %s", tempstr.get());
  }
  
  // Read angle of X-probe wrt platform zero

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading XPOL_ANG" << endl;
  
  float degrees = 0.0;

  fits_read_key (fptr, TFLOAT, "XPOL_ANG", &degrees, comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key XPOL_ANG").warning() << endl;
    status = 0;
  }
  else
    ext->X_offset.setDegrees( degrees );

  // Feed track mode

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading feed track mode" << endl;

  ext->mode = Receiver::Feed;

  fits_read_key (fptr, TSTRING, "FD_MODE", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FD_MODE").warning() << endl;
    status = 0;
  }
  else {
    if (strncasecmp(tempstr.get(),"FA",2) == 0)
      ext->mode = Receiver::Feed;
    else if (strncasecmp(tempstr.get(),"CPA",3) == 0)
      ext->mode = Receiver::Celestial;
    else if (strncasecmp(tempstr.get(),"GPA",3) == 0)
      ext->mode = Receiver::Galactic;
    else if (verbose)
      cerr << "FITSArchive::load_Receiver unknown FD_MODE=" 
           << tempstr.get() << endl;
  }

#if 0

  // Read requested feed angle
  
  if (verbose)
    cerr << "FITSArchive::load_Receiver reading requested feed angle" << endl;
  
  fits_read_key (fptr, TFLOAT, "FA_REQ", &(ext->fa_req), comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FA_REQ").warning() << endl;
    status = 0;
  }

#endif

  // Read attenuator settings

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading attenuator settings" << endl;

  fits_read_key (fptr, TFLOAT, "ATTEN_A", &(ext->atten_a),
		 comment, &status);
  fits_read_key (fptr, TFLOAT, "ATTEN_B", &(ext->atten_b),
		 comment, &status);

  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key ATTEN_A,B").warning() << endl;
    status = 0;
  }

  if (status == 0) {
    add_extension (ext);
    return;
  }


}

