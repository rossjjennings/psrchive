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
  return Pulsar::Config::get_runtime() + "/fluxcal." + which;
}

Pulsar::CalSource::CalSource ()
{
  on_filename = config.get<string> ("fluxcal.on", get_default("on"));
  stringfload (&fluxcal_on, on_filename);

  off_filename = config.get<string> ("fluxcal.off", get_default("off"));
  stringfload (&fluxcal_off, off_filename);
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
