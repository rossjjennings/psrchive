#include "Pulsar/TimerArchive.h"
#include "Pulsar/Telescope.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/BackendName.h"

void Pulsar::TimerArchive::unpack_extensions ()
{
  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions" << endl;

  Receiver* receiver = getadd<Receiver>();
  unpack (receiver);

  Telescope* telescope = getadd<Telescope>();
  telescope->set_coordinates (get_telescope_code());

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions set Backend" << endl;

  Backend* backend = get<Backend>();

  if (!backend) {
    BackendName* ben = getadd<BackendName>();
    ben->set_name (hdr.machine_id);
    backend = ben;
  }

  if (verbose == 3)
    cerr << "Pulsar::TimerArchive::unpack_extensions " 
	 << backend->get_extension_name() << " name="
	 << backend->get_name() << endl;

}

void Pulsar::TimerArchive::pack_extensions () const
{
  struct timer* header = const_cast<struct timer*>( &hdr );

  const Receiver* receiver = get<Receiver>();
  if (receiver)
    const_cast<TimerArchive*>(this)->pack (receiver);


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

