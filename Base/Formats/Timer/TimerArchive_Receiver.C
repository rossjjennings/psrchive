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
      cerr << "Pulsar::TimerArchive::unpack Receiver WARNING Receiver ";
      if (hdr.rcvr_id[0] == '\0')
	cerr << "not specified." << endl;
      else
	cerr << "not recognized." << endl;
    }

    if ( get_telescope_code() == '7' )
      Parkes::guess (receiver, this);

    else if (verbose)
      cerr << "Pulsar::TimerArchive::unpack Receiver"
	" WARNING No guess for telescope=" << hdr.telid << "." << endl;

  }

}

void Pulsar::TimerArchive::pack (const Receiver* receiver)
{
  switch (receiver->get_basis()) {
  case Signal::Linear:
    hdr.banda.polar = 1;
    break;
  case Signal::Circular:
    hdr.banda.polar = 0;
    break;
  default:
    hdr.banda.polar = -1;
    throw Error (InvalidParam, "Pulsar::TimerArchive::pack_extensions",
		 "unrecognized Basis=" 
		 + Signal::Basis2string( receiver->get_basis() ));
  }

  set_corrected (FEED_CORRECTED, receiver->get_feed_corrected());
  set_corrected (PARA_CORRECTED,  receiver->get_platform_corrected());

  if (receiver->get_name().length()+1 >= RCVR_ID_STRLEN) {
    if (verbose)
      cerr << "Pulsar::TimerArchive::pack Receiver WARNING"
	" truncating receiver name" << endl;
  }

  strncpy (hdr.rcvr_id, receiver->get_name().c_str(), RCVR_ID_STRLEN);
}
