#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseVsTime::PhaseVsTime ()
{
  ichan = 0;
  ipol = 0;
}

TextInterface::Class* Pulsar::PhaseVsTime::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsTime::prepare (const Archive* data)
{
  unsigned rows = data->get_nsubint();
 
  const IntegrationOrder* order = data->get<IntegrationOrder>();

  if (order) {
    set_yrange( order->get_Index(0), order->get_Index(rows-1) );
    return;
  }

  double mjd0 = data->start_time().in_days();
  double mjd1 = data->end_time().in_days();

  float to_hours = 1.0 / 24.0;
  float to_minutes = to_hours / 60.0;
  float to_seconds = to_minutes / 60.0;

  time_string = "Time ";
  if (mjd1 - mjd0 > 1.0) {
    time_string += "(days)";
    set_yrange (mjd0, mjd1);
  }
  else if (mjd1 - mjd0 > to_hours) {
    time_string += "(hours)";
    set_yrange (mjd0*to_hours, mjd1*to_hours);
  }
  else if (mjd1 - mjd0 > to_minutes) {
    time_string += "(minutes)";
    set_yrange (mjd0*to_minutes, mjd1*to_minutes);
  }
  else {
    time_string += "(seconds)";
    set_yrange (mjd0*to_seconds, mjd1*to_seconds);
  }
}

std::string Pulsar::PhaseVsTime::get_ylabel (const Archive* data)
{
  const IntegrationOrder* order = data->get<IntegrationOrder>();
  if (!order)
    return time_string;

  std::string str = order->get_IndexState();
  if (!order->get_Unit().empty())
    str += " (" + order->get_Unit() + ")";
  return str;
}

unsigned Pulsar::PhaseVsTime::get_nrow (const Archive* data)
{
  return data->get_nsubint();
}

const Pulsar::Profile*
Pulsar::PhaseVsTime::get_Profile (const Archive* data, unsigned isubint)
{
  return Pulsar::get_Profile (data, isubint, ipol, ichan);
}
