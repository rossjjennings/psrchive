#include "Pulsar/Receiver.h"
#include "Pulsar/TimerArchive.h"
#include "Pulsar/Parkes.h"

void Pulsar::TimerArchive::unpack (Receiver* receiver)
{
  if (hdr.rcvr_id[0] == '\0' && verbose)
    cerr << "Pulsar::TimerArchive::unpack Receiver WARNING "
      "Name not specified." << endl;

  if (strlen (hdr.rcvr_id) >= RCVR_ID_STRLEN) {
    if (verbose)
      cerr << "Pulsar::TimerArchive::unpack Receiver WARNING "
	"Name corrupted." << endl;

    hdr.rcvr_id[0] = '\0';
  }

  receiver->set_name (hdr.rcvr_id);

  if (hdr.banda.polar == 0)
    receiver->set_basis( Signal::Circular );
  else
    receiver->set_basis( Signal::Linear );

  switch (hdr.feedmode) {
    
  case 0:
    receiver->set_tracking_mode (Receiver::Feed);
    break;
    
  case 1:
    receiver->set_tracking_mode (Receiver::Celestial);
    break;
    
  case 2:
    receiver->set_tracking_mode (Receiver::Galactic);
    break;
    
  default:
    throw Error (InvalidParam, "Pulsar::TimerArchive::pack Receiver",
		 "unrecognized feedmode=%d", hdr.feedmode);
    
  }

  Angle angle;

  angle.setDegrees (hdr.banda.feed_offset); 
  receiver->set_tracking_angle (angle);

  angle.setDegrees (hdr.extra.supplement.X_offset);
  receiver->set_X_offset (angle);

  angle.setDegrees (hdr.extra.supplement.Y_offset);
  receiver->set_Y_offset (angle);

  angle.setDegrees (hdr.extra.supplement.calibrator_offset);
  receiver->set_calibrator_offset (angle);

  receiver->set_feed_corrected (hdr.corrected & FEED_CORRECTED);
  receiver->set_platform_corrected (hdr.corrected & PARA_CORRECTED);
}

void Pulsar::TimerArchive::pack (const Receiver* receiver)
{
  if (verbose && receiver->get_name().length()+1 >= RCVR_ID_STRLEN)
    cerr << "Pulsar::TimerArchive::pack Receiver WARNING"
      " truncating receiver name " << receiver->get_name() <<
      " to " << RCVR_ID_STRLEN-1 << " characters" << endl;

  strncpy (hdr.rcvr_id, receiver->get_name().c_str(), RCVR_ID_STRLEN);

  switch (receiver->get_basis()) {
  case Signal::Linear:
    hdr.banda.polar = 1;
    break;
  case Signal::Circular:
    hdr.banda.polar = 0;
    break;
  default:
    hdr.banda.polar = -1;
    throw Error (InvalidParam, "Pulsar::TimerArchive::pack Receiver",
		 "unrecognized Basis=" 
		 + Signal::Basis2string( receiver->get_basis() ));
  }

  switch (receiver->get_tracking_mode()) {
  case Receiver::Feed:
    hdr.feedmode = 0;
    break;
  case Receiver::Celestial:
    hdr.feedmode = 1;
    break;
  case Receiver::Galactic:
    hdr.feedmode = 2;
    break;
  default:
    hdr.banda.polar = -1;
    throw Error (InvalidParam, "Pulsar::TimerArchive::pack Receiver",
		 "unrecognized tracking mode=" 
		 + receiver->get_tracking_mode_string());
  }

  hdr.banda.feed_offset = receiver->get_tracking_angle().getDegrees (); 

  hdr.extra.supplement.X_offset = receiver->get_X_offset().getDegrees ();

  hdr.extra.supplement.Y_offset = receiver->get_Y_offset().getDegrees ();

  hdr.extra.supplement.calibrator_offset 
    = receiver->get_calibrator_offset().getDegrees ();

  set_corrected (FEED_CORRECTED, receiver->get_feed_corrected());
  set_corrected (PARA_CORRECTED,  receiver->get_platform_corrected());

}
