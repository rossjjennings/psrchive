#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

void Pulsar::Archive::fappend (Pulsar::Archive* arch)
{
  if (arch->get_nsubint() != get_nsubint())
    throw Error (InvalidParam, "Pular::Archive::fappend nsubint mismatch");

  int new_nchan = get_nchan() + arch->get_nchan();
  
  try {
    
    for (unsigned i = 0; i < get_nsubint(); i++) {
      get_Integration(i)->fappend(arch->get_Integration(i));
    }

  }
  catch (Error& error) {
    cerr << error << endl;
  }

  set_nchan(new_nchan);
  set_bandwidth((arch->get_bandwidth())+get_bandwidth());
  set_centre_frequency(((arch->get_centre_frequency())+get_centre_frequency())/2.0);

}
