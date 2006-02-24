#include "Pulsar/FITSArchive.h"
#include "Pulsar/Receiver.h"
#include "FITSError.h"

void Pulsar::FITSArchive::unload (fitsfile* fptr, const Receiver* ext)
{
  // status returned by FITSIO routines
  int status = 0;

  // no comment
  char* comment = 0;

  // temporary string
  auto_ptr<char> tempstr ( new char[FLEN_VALUE] );

  strncpy (tempstr.get(), ext->get_name().c_str(), FLEN_VALUE);
  fits_update_key (fptr, TSTRING, "FRONTEND", tempstr.get(), comment, &status);
    
  if (ext->get_basis() == Signal::Linear)
    strcpy (tempstr.get(), "LIN");
  
  else if (ext->get_basis() == Signal::Circular)
    strcpy (tempstr.get(), "CIRC");
  
  else
    strcpy (tempstr.get(), "    ");
  
  fits_update_key (fptr, TSTRING, "FD_POLN", tempstr.get(), comment, &status);
  
  float temp;

  temp = ext->get_field_orientation().getDegrees();
  fits_update_key (fptr, TFLOAT, "FD_SANG", &temp, comment, &status);

  int hand = (int) ext->get_hand();
  fits_update_key (fptr, TINT, "FD_HAND", &hand, comment, &status);

  temp = ext->get_reference_source_phase().getDegrees();
  fits_update_key (fptr, TFLOAT, "FD_XYPH", &temp, comment, &status);

  switch (ext->get_tracking_mode()) {
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
  
  temp = ext->get_tracking_angle().getDegrees();
  fits_update_key (fptr, TFLOAT, "FA_REQ", &temp, comment, &status);

  temp = ext->get_atten_a();
  fits_update_key (fptr, TFLOAT, "ATTEN_A", &temp, comment, &status);

  temp = ext->get_atten_b();
  fits_update_key (fptr, TFLOAT, "ATTEN_B", &temp, comment, &status);

  if (status)
    throw FITSError (status, "Pulsar::FITSArchive::unload Receiver");
}
