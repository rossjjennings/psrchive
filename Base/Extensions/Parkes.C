#include "Pulsar/Parkes.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"

//! Initialize the Receiver Extension with Parkes Multibeam attributes
void Pulsar::Parkes::Multibeam (Receiver* receiver)
{
  receiver->set_name ("MB");
  receiver->set_X_offset (0.0);
  receiver->set_Y_offset (M_PI);
  receiver->set_calibrator_offset (M_PI*0.5);
}

//! Initialize the Receiver Extension with Parkes H_OH attributes
void Pulsar::Parkes::H_OH (Receiver* receiver)
{
  receiver->set_name ("H-OH");
  receiver->set_X_offset (0.0);
  receiver->set_Y_offset (0.0);
  receiver->set_calibrator_offset (0.0);
}

//! Initialize the Receiver Extension with Parkes best guess
void Pulsar::Parkes::guess (Receiver* receiver, Archive* archive)
{
  if ( archive->get_telescope_code() != '7' )
    throw Error (InvalidParam, "Pulsar::Parkes::guess",
		 "telescope=%c != Parkes=7", archive->get_telescope_code());

  if ( receiver->get_basis() == Signal::Linear ) {

    if ( archive->get_centre_frequency() > 1230.0 && 
	 archive->get_centre_frequency() < 1530.0 ) {
      if (Archive::verbose > 1)
	cerr << "Pulsar::Parkes::guess Multibeam" << endl;
      Parkes::Multibeam (receiver);
    }
	
    else if ( archive->get_centre_frequency() > 1200.0 &&
	      archive->get_centre_frequency() < 1800.0 ) {
      if (Archive::verbose > 1)
	cerr << "Pulsar::Parkes::guess H-OH" << endl;
      Parkes::H_OH (receiver);
    }

    else {
      
      if (Archive::verbose > 0)
        cerr << "Pulsar::Parkes::guess nothing for " 
             << archive->get_centre_frequency() << " MHz" << endl;
      
    }

  }
  
  else {
    
    cerr << "Pulsar::Parkes::guess nothing for circular feeds" << endl;
    
  }

}
