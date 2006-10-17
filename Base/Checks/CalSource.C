/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalSource.h"
#include "Pulsar/Config.h"

#include "templates.h"
#include "strutil.h"

using namespace std;

static string get_default (string which)
{
  char* psrhome = getenv ("PSRHOME");

  if (psrhome)
    return psrhome + string ("/runtime/psrchive/fluxcal.") + which;
  else
    return "fluxcal." + which;
}

Pulsar::CalSource::CalSource ()
{
  stringfload (&fluxcal_on, 
	       Pulsar::config.get<string> ("fluxcal.on", get_default("on")));

  stringfload (&fluxcal_off,
	       Pulsar::config.get<string> ("fluxcal.off", get_default("off")));
}

void Pulsar::CalSource::apply (Archive* archive)
{
  if (archive->get_type() != Signal::PolnCal )
    return;

  if (found(archive->get_source(), fluxcal_on)) {
    archive->set_type ( Signal::FluxCalOn );
    if (Archive::verbose == 3)
      cerr << "Pulsar::CalSource::apply source " << archive->get_source()
	   << " is Signal::FluxCalOn" << endl;
    return;
  }

  if (found(archive->get_source(), fluxcal_off)) {
    archive->set_type ( Signal::FluxCalOff );
    if (Archive::verbose == 3)
      cerr << "Pulsar::CalSource::apply source " << archive->get_source()
	   << " is Signal::FluxCalOff" << endl;
    return;
  }

}
