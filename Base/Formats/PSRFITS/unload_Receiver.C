#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, const Receiver* ext)
{
  // status returned by FITSIO routines
  int status = 0;

  // no comment
  char* comment = 0;

  fits_update_key (fptr, TSTRING, "FRONTEND", 
  		   const_cast<char*>(ext->name.c_str()), comment, &status);
  
  auto_ptr<char> tempstr ( new char[FLEN_VALUE] );
  
  if (ext->basis == Signal::Linear)
    strcpy (tempstr.get(), "LIN");
  
  else if (ext->basis == Signal::Circular)
    strcpy (tempstr.get(), "CIRC");
  
  else
    strcpy (tempstr.get(), "    ");
  
  fits_update_key (fptr, TSTRING, "FD_POLN", tempstr.get(), comment, &status);
  
  float degrees;

  degrees = ext->X_offset.getDegrees();
  fits_update_key (fptr, TFLOAT, "XPOL_ANG", &degrees, comment, &status);

  degrees = ext->Y_offset.getDegrees() + 90.0;
  fits_update_key (fptr, TFLOAT, "YPOL_ANG", &degrees, comment, &status);

  degrees = ext->calibrator_offset.getDegrees() + 45.0;
  fits_update_key (fptr, TFLOAT, "CAL_ANG", &degrees, comment, &status);

  switch (ext->basis) {
  case Receiver::Feed:
    strcpy (tempstr.get(), "FA"); break;
  case Receiver::Celestial:
    strcpy (tempstr.get(), "CPA"); break;
  case Receiver::Galactic:
    strcpy (tempstr.get(), "GPA"); break;
  default:
    strcpy (tempstr.get(), "   "); break;
  }

  fits_update_key (fptr, TSTRING, "FD_MODE", tempstr.get(), comment, &status);
  
  degrees = ext->tracking_angle.getDegrees();
  fits_update_key (fptr, TFLOAT, "FA_REQ", &degrees, comment, &status);

  fits_update_key (fptr, TFLOAT, "ATTEN_A",
		   const_cast<float*>(&(ext->atten_a)), comment, &status);

  fits_update_key (fptr, TFLOAT, "ATTEN_B",
		   const_cast<float*>(&(ext->atten_b)), comment, &status);

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload Receiver");
}
