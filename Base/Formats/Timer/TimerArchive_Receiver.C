#include "Pulsar/Receiver.h"
#include "Pulsar/TimerArchive.h"
#include "Pulsar/Parkes.h"

void Pulsar::TimerArchive::unpack (Receiver* receiver)
{
  switch (hdr.banda.polar)  {
  case 0:
    receiver->set_basis( Signal::Circular ); break;
  case 1:
  default:
    receiver->set_basis( Signal::Linear ); break;
  }

  receiver->set_feed_corrected (hdr.corrected & FEED_CORRECTED);
  receiver->set_platform_corrected (hdr.corrected & PARA_CORRECTED);

  if ( !strcmp (hdr.rcvr_id, "MB") )
    Parkes::Multibeam (receiver);

  else if ( !strcmp (hdr.rcvr_id, "H-OH") )
    Parkes::H_OH (receiver);

  else {

    if (verbose) {
      cerr << "Pulsar::TimerArchive::unpack WARNING Receiver ";
      if (hdr.rcvr_id[0] == '\0')
	cerr << "not specified." << endl;
      else
	cerr << "not recognized." << endl;
    }

    if ( get_telescope_code() == '7' )
      Parkes::guess (receiver, this);

    else if (verbose)
      cerr << "  No guess for telescope=" << hdr.telid << endl;

  }

}
