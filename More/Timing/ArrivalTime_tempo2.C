/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/Archive.h"

#include "Pulsar/ShiftEstimator.h"
#include "Pulsar/ObsExtension.h"
#include "Pulsar/Backend.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/WidebandCorrelator.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/ProcHistory.h"

using namespace Pulsar;
using namespace std;


std::string Pulsar::ArrivalTime::get_tempo2_aux_txt (const Tempo::toa& toa)
{
  string args;

  args = observation->get_filename();

  // check for special "IPTA" flag.  We will still allow for processing
  // any additional flags even though some of these will be duplicates of
  // the IPTA stuff.
  if (format_flags.find("IPTA")!=string::npos)
    args += get_ipta_aux_txt(toa);

  if (format_flags.find("i")!=string::npos)
  {
    const Backend* backend = observation->get<Backend>();
    if (backend)
      args += " -i " + backend->get_name();
  }

  if (format_flags.find("r")!=string::npos)
  {
    const Receiver* receiver = observation->get<Receiver>();
    if (receiver)
      args += " -r " + receiver->get_name();
  }

  if (format_flags.find("c")!=string::npos)
    args += " -c " + tostring(observation->get_nchan());

  if (format_flags.find("s")!=string::npos)
    args += " -s " + tostring(observation->get_nsubint());

  for (unsigned i = 0; i < attributes.size(); i++)
  {
    string value = get_value (attributes[i], toa);
    args += " -" + attributes[i] + " " + value;
  }

  if (format_flags.find("o")!=string::npos) /* Include observer info. */
  {
    const ObsExtension* ext = 0;
    ext = observation->get<ObsExtension>();
    if (!ext) {
      args += " -o N/A";
    }
    else {
      args += " -o " + ext->observer;
    }
  }

  // Any extra user-specified text (not checked for conformance with proper
  // flag formatting):
  if (extra_text.length())
    args += " " + extra_text;

  return args;
}

std::string Pulsar::ArrivalTime::get_ipta_aux_txt (const Tempo::toa& toa)
{
  string args;

  // -fe frontend
  const Receiver *r = observation->get<Receiver>();
  if (r) 
    args += " -fe " + r->get_name();

  // -be backend
  const Backend *b = observation->get<Backend>();
  if (b)
    args += " -be " + b->get_name();

  // -f frontend_backend
  if (r && b) 
    args += " -f " + r->get_name() + "_"  + b->get_name();
  
  // -B band descriptor : don't think there is a standard..

  // -bw bandwidth(MHz)
  // Doesn't really account for possible different zapping applied to each chan
  args += " -bw " + tostring(
      fabs(observation->get_bandwidth()) / (double)observation->get_nchan(), 4);

  const Integration *subint = observation->get_Integration(toa.get_subint());

  args += " -tobs " + tostring(subint->get_duration(), 5);

  // -pta PTA : Could choose based on telescope...
  
  // -proc processing version id : Can't get from the file

  // -tmplt template profile name
  if (standard)
    args += " -tmplt " + standard->get_filename();
  else
    args += " -model " + shift_estimator->get_name();
      
  // -gof reduced chi2 of template fit
  if (toa.get_reduced_chisq() != 0)
    args += " -gof " + tostring(toa.get_reduced_chisq(),3);

  // -nbin number of bins in profile
  args += " -nbin " + tostring(observation->get_nbin());

  // -nch number of pre-dedispersion channels in BW
  const ProcHistory *h = observation->get<ProcHistory>();
  if (h)
    args += " -nch " + tostring(
        h->get_row(0)->get_nchan() / observation->get_nchan());


  return args;

}
