/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ParametersDM.h"
#include <math.h>

using namespace std;

void Pulsar::ParametersDM::set_parameters (const Pulsar::Parameters* params)
{
  parameters = params;
  dm_epoch = get_dm_epoch();
  dm = get_dm();
  get_dm_coeffs(dm_coeffs);

  string str = parameters->get_value("DM_SERIES");
  if (str == "POLY")
    is_taylor_series = false;
  else
    is_taylor_series = true;
}

MJD Pulsar::ParametersDM::get_dm_epoch()
{
  string str = parameters->get_value("DMEPOCH");

  if (!str.empty()) 
  {
    return MJD(fromstring<double>(str));
  }

  str = parameters->get_value("PEPOCH");

  if (!str.empty())
  {
    return MJD(fromstring<double>(str));
  }

  throw Error(InvalidState, "Pulsar::ParametersDM::get_dm_epoch",
      "cannot find DMEPOCH or PEPOCH in ephemeris");
}

double Pulsar::ParametersDM::get_dm()
{
  string str = parameters->get_value("DM");

  if (!str.empty())
  {
    return fromstring<double>(str);
  }

  throw Error(InvalidState, "Pulsar::ParametersDM::get_dm",
        "cannot find DM value in ephemeris");
}

void Pulsar::ParametersDM::get_dm_coeffs(vector<double>& dm_coeffs)
{
  dm_coeffs.clear();

  unsigned ikey = 1;

  while (ikey > 0)
  {
    string key = "DM" + tostring<unsigned>(ikey);
    string str = parameters->get_value(key);

    if (str.empty())
      break;

    dm_coeffs.push_back(fromstring<double>(str));
    ++ikey;
  }
}

double Pulsar::ParametersDM::get_dm (const MJD& epoch)
{
  // time offset in years
  const double delta_time = (epoch - dm_epoch).in_days() / 365.25;

  double new_dm = dm;
  unsigned power = 1;
  unsigned factorial = 1;

  for (unsigned i = 0; i < dm_coeffs.size(); ++i)
  {
    new_dm += dm_coeffs[i] * pow(delta_time, power) / factorial;
    ++power;

    if (is_taylor_series)
      factorial *= power;
  }

  return new_dm;
}
