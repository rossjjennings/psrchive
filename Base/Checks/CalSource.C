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
  Option<string,false> cfgon
    ( &on_filename,
      "fluxcal::on", get_default("on"),

      "Standard candle on-source aliases filename",

      "The name of the file from which commonly used source name alises for\n"
      "on-source flux calibrator observation are read."
    );

#ifdef _DEBUG
  cerr << "Pulsar::CalSource on_filename=" << on_filename << endl;
#endif

  stringfload (&fluxcal_on, on_filename);

  Option<string,false> cfgoff
    ( &off_filename,
      "fluxcal::off", get_default("off"),

      "Standard candle off-source aliases filename",

      "The name of the file from which commonly used source name alises for\n"
      "off-source flux calibrator observation are read."
    );

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
