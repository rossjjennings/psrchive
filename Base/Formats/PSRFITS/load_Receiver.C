#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"
#include "FITSError.h"

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
    ext->set_name ("unknown");
  }
  else
    ext->set_name (tempstr.get());
  
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
      ext->set_basis (Signal::Linear);
    else if (strncasecmp(tempstr.get(),"CIRC",4) == 0)
      ext->set_basis (Signal::Circular);
    else
      throw Error (InvalidParam, "FITSArchive::load_Receiver",
	           "unknown FD_POLN: %s", tempstr.get());
  }
  
  // Read angle of X-probe wrt platform zero

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading XPOL_ANG" << endl;
  
  float temp = 0.0;
  Angle angle;

  fits_read_key (fptr, TFLOAT, "XPOL_ANG", &temp, comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key XPOL_ANG").warning() << endl;
    status = 0;
    angle.setDegrees( 0 );
  }
  else
    angle.setDegrees( temp );

  ext->set_X_offset (angle);

  // Read angle of Y-probe wrt platform zero

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading YPOL_ANG" << endl;
  
  fits_read_key (fptr, TFLOAT, "YPOL_ANG", &temp, comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key YPOL_ANG").warning() << endl;
    status = 0;
    angle.setDegrees( 0 );
  }
  else
    angle.setDegrees( temp - 90.0 );

  ext->set_Y_offset (angle);

  // Read angle of linear noise diode wrt platform zero

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading CAL_ANG" << endl;
  
  fits_read_key (fptr, TFLOAT, "CAL_ANG", &temp, comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key CAL_ANG").warning() << endl;
    status = 0;
    angle.setDegrees( 0 );
  }
  else
    angle.setDegrees( temp - 45.0 );

  ext->set_calibrator_offset (angle);

  // Feed track mode

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading feed track mode" << endl;

  ext->set_tracking_mode (Receiver::Feed);

  fits_read_key (fptr, TSTRING, "FD_MODE", tempstr.get(), comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FD_MODE").warning() << endl;
    status = 0;
  }
  else {
    if (strncasecmp(tempstr.get(),"FA",2) == 0)
      ext->set_tracking_mode (Receiver::Feed);
    else if (strncasecmp(tempstr.get(),"CPA",3) == 0)
      ext->set_tracking_mode (Receiver::Celestial);
    else if (strncasecmp(tempstr.get(),"GPA",3) == 0)
      ext->set_tracking_mode (Receiver::Galactic);
    else if (verbose)
      cerr << "FITSArchive::load_Receiver unknown FD_MODE=" 
           << tempstr.get() << endl;
  }

  // Read requested feed angle
  
  if (verbose)
    cerr << "FITSArchive::load_Receiver reading requested feed angle" << endl;
  
  fits_read_key (fptr, TFLOAT, "FA_REQ", &temp, comment, &status);
  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key FA_REQ").warning() << endl;
    status = 0;
    angle.setDegrees( 0 );
  }
  else
    angle.setDegrees( temp );

  ext->set_tracking_angle (angle);

  // Read attenuator settings

  if (verbose)
    cerr << "FITSArchive::load_Receiver reading attenuator settings" << endl;

  fits_read_key (fptr, TFLOAT, "ATTEN_A", &temp, comment, &status);
  ext->set_atten_a (temp);

  fits_read_key (fptr, TFLOAT, "ATTEN_B", &temp, comment, &status);
  ext->set_atten_b (temp);

  if (status != 0) {
    if (verbose)
      cerr << FITSError (status, "FITSArchive::load_Receiver",
			 "fits_read_key ATTEN_A,B").warning() << endl;

    ext->set_atten_a (0);
    ext->set_atten_b (0);

    status = 0;
  }

  if (status == 0) {
    add_extension (ext);
    return;
  }


}

