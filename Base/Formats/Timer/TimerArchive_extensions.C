#include "Pulsar/TimerArchive.h"
#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/BackendName.h"

void Pulsar::TimerArchive::unpack_extensions ()
{
  Receiver* receiver = getadd<Receiver>();
  unpack (receiver);

  Telescope* telescope = getadd<Telescope>();
  telescope->set_coordinates (get_telescope_code());

  Backend* backend = get<Backend>();

  if (!backend) {
    BackendName* ben = getadd<BackendName>();
    ben->set_name (hdr.machine_id);
    backend = ben;
  }

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions using " 
	 << backend->get_extension_name() << endl;

}

void Pulsar::TimerArchive::pack_extensions () const
{
  struct timer* header = const_cast<struct timer*>( &hdr );

  const Receiver* receiver = get<Receiver>();

  if (receiver) {

    switch (receiver->get_basis()) {
      case Signal::Linear:
        header->banda.polar = 1;
        break;
      case Signal::Circular:
        header->banda.polar = 0;
        break;
      default:
        header->banda.polar = -1;
        throw Error (InvalidParam, "Pulsar::TimerArchive::pack_extensions",
                     "unrecognized Basis=" 
                     + Signal::Basis2string( receiver->get_basis() ));
    }

    if (receiver->get_feed_corrected())
      header->corrected |= FEED_CORRECTED;
    else
      header->corrected &= ~FEED_CORRECTED;

    if (receiver->get_platform_corrected())
      header->corrected |= PARA_CORRECTED;
    else
      header->corrected &= ~PARA_CORRECTED;

  }

  // nothing done with Telescope Extension for now

  const Backend* backend = get<Backend>();

  if (backend) {

    string name = backend->get_name();
    
    if( name.length()+1 > MACHINE_ID_STRLEN )
      throw Error (InvalidParam, "Pulsar::TimerArchive::pack_extensions",
		   "length of backend '%s'=%d > MACHINE_ID_STRLEN=%d",
		   name.c_str(), name.length()+1, MACHINE_ID_STRLEN);

    strcpy (header->machine_id, name.c_str());

  }

}

