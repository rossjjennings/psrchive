/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/CalSource.h"
#include "Pulsar/Config.h"

#include "templates.h"
#include "strutil.h"

using namespace std;

#ifdef _DEBUG
static int runme () { cerr << "CalSource: init" << endl; return 0; }
static int test = runme ();
#endif

static string get_default (string which)
{
  string result = Pulsar::Config::get_runtime() + "/fluxcal." + which;
#ifdef _DEBUG
  cerr << "CalSource::get_default=" << result << endl;
#endif
  return result;
}

Pulsar::CalSource::CalSource () 
{
  Configuration* config = Config::get_configuration();

  on_filename = config->get ("fluxcal::on",  get_default("on"));
#ifdef _DEBUG
  cerr << "Pulsar::CalSource on_filename=" << on_filename << endl;
#endif
  stringfload (&fluxcal_on, on_filename);

  off_filename = config->get ("fluxcal::off",  get_default("off"));
#ifdef _DEBUG
  cerr << "Pulsar::CalSource off_filename=" << off_filename << endl;
#endif
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
