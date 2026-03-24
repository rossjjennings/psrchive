/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *
 *   Original version by Gregory Desvignes with updates and bugfixes by
 *   Stefan Oslowski and Maciej Serylak
 *
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/ManualPolnCalibrator.h"
#include "Pulsar/CalibratorTypes.h"

#include "strutil.h"

#include <fstream>

using namespace std;

Pulsar::ManualPolnCalibrator::ManualPolnCalibrator (const string& filename)
{
  type = new CalibratorTypes::ManualPoln;
  load (filename);
}

void Pulsar::ManualPolnCalibrator::load (const string& ascii_model_filename)
{
  std::ifstream input (ascii_model_filename.c_str());
  if (!input)
    throw Error (FailedSys, "Pulsar::ManualPolnCalibrator::load",
                 "ifstream (" + ascii_model_filename + ")");

  string line;
  Response single;

  while (input) try
  {
    getline (input, line);

    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    if (verbose > 2) cerr << "Pulsar::ManualPolnCalibrator::load '"<< line << "'" << endl;

    MJD epoch = single.load(line);
    add (epoch, single);
  }
  catch (Error& error)
  {
    cerr << "Pulsar::Database::load discarding entry:" << error << endl;
  }

  if (verbose > 2) cerr << "Pulsar::ManualPolnCalibrator::load " << response.size() << " epochs" <<endl;

  filenames.push_back(ascii_model_filename);
}

void Pulsar::ManualPolnCalibrator::add (const MJD& epoch, const Response& single)
{
  if (response.size() == 0 || response[current_response].get_epoch() != epoch)
  {
    if (verbose > 2) cerr << "Pulsar::ManualPolnCalibrator::add new epoch=" << epoch.printdays(10) << endl;
    response.resize( response.size() + 1 );
    current_response = response.size() - 1;
    response[current_response].set_epoch(epoch);
  }
  response[current_response].add(single);
}

void Pulsar::ManualPolnCalibrator::calibrate (Archive* arch)
{
  transform_work (arch, "calibrate", true);
}

void Pulsar::ManualPolnCalibrator::transform (Archive* arch)
{
  transform_work (arch, "transform", false);
}

void Pulsar::ManualPolnCalibrator::transform_work (Archive* arch, const std::string& name, bool invert) try
{
  if (verbose > 2)
    cerr << "Pulsar::ManualPolnCalibrator::" << name << endl;

  if (arch->get_npol() != 4)
    throw Error (InvalidState, "Pulsar::ManualPolnCalibrator::"+name, "Archive::npol != 4");

  for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
  {
    Integration* subint = arch->get_Integration(isub);
    auto& freq_response = match (subint->get_epoch());

    vector<Jones<float>> response (subint->get_nchan());

    for (unsigned ichan=0; ichan < arch->get_nchan(); ichan++)
    {
      double freq = subint->get_centre_frequency(ichan);
      auto& match = freq_response.match(freq);
      response[ichan] = match.get_response();
      if (invert)
        response[ichan] = inv(response[ichan]);
    }

    subint->expert()->transform (response);
  }
  arch->set_poln_calibrated (true);
}
catch (Error& error)
{
  throw error += "Pulsar::ManualPolnCalibrator::"+name;
}

MJD Pulsar::ManualPolnCalibrator::Response::load (const string& str)
{
  const string whitespace = WHITESPACE;
  string temp = str;
  std::vector<string> words = stringdecimate(temp, whitespace);

  if( words.size() < 10 )
  {
    throw Error(InvalidState,"Pulsar::ManualPolnCalibrator::Response::load",
                "could not parse 10 words from '"+temp+"'");
  }
  MJD ref_epoch (words[0]);
  frequency = atof(words[1].c_str());
  std::complex<float> j00, j01, j10, j11;
  j00 = std::complex<float> ( atof( words[2].c_str() ), atof( words[3].c_str() ) );
  j01 = std::complex<float> ( atof( words[4].c_str() ), atof( words[5].c_str() ) );
  j10 = std::complex<float> ( atof( words[6].c_str() ), atof( words[7].c_str() ) );
  j11 = std::complex<float> ( atof( words[8].c_str() ), atof( words[9].c_str() ) );
  Jones<float> tmp_jones = Jones<float>(j00, j01, j10, j11);
  set_response( tmp_jones );
  return ref_epoch;
}

const Pulsar::ManualPolnCalibrator::FrequencyResponse& 
Pulsar::ManualPolnCalibrator::match (const MJD& epoch) const
{
  if (response.size() == 0)
    throw Error (InvalidState, "Pulsar::ManualPolnCalibrator::match", "no responses loaded");

  unsigned best_response = 0;
  double best_diff = -1.0;

  for (unsigned ie=0; ie<response.size(); ie++)
  {
    double diff = fabs( (response[ie].get_epoch() - epoch).in_seconds() );

    if (best_diff < 0.0 || diff < best_diff)
    {
      best_response = ie;
      best_diff = diff;
    }
  }

  return response[best_response];
}

const Pulsar::ManualPolnCalibrator::Response& 
Pulsar::ManualPolnCalibrator::FrequencyResponse::match (double freq_MHz) const
{
  if (response.size() == 0)
    throw Error (InvalidState, "Pulsar::ManualPolnCalibrator::FrequencyResponse::match", 
                 "no responses loaded");

  unsigned best_response = 0;
  double best_diff = -1.0;

  // Frequency stored internally in Hz
  double freq = 1e6 * freq_MHz;

  for (unsigned ie=0; ie<response.size(); ie++)
  {
    double diff = fabs(response[ie].get_frequency() - freq);

    if (best_diff < 0.0 || diff < best_diff)
    {
      best_response = ie;
      best_diff = diff;
    }
  }

  return response[best_response];
}

//! Returns the best match, given the epoch and frequency
const Jones<float>& Pulsar::ManualPolnCalibrator::get_response (const MJD& epoch, double MHz) const
{
  return match(epoch).match(MHz).get_response();
}