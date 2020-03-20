/***************************************************************************
 *
 *   Copyright (C) 2014 by Gregory Desvignes
 *   Updates and bugfixes by Stefan Oslowski and Maciej Serylak
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ManualPolnCalibrator.h"
#include "Pulsar/CalibratorTypes.h"

#include "strutil.h"

#include <fstream>

using namespace std;

Pulsar::ManualPolnCalibrator::ManualPolnCalibrator (const string filename)
{
  type = new CalibratorTypes::ManualPoln;
  load (filename);
}

//! Copy constructor
Pulsar::ManualPolnCalibrator::ManualPolnCalibrator (const ManualPolnCalibrator& calibrator)
{
  built = false;
  observation_nchan = 0;
}

//! Destructor
Pulsar::ManualPolnCalibrator::~ManualPolnCalibrator ()
{
}

//! Set the number of integrations in the response array
void Pulsar::ManualPolnCalibrator::set_response_nsub (unsigned nsub)
{
  return;
}

//! Get the number of frequency channels in the response array
unsigned Pulsar::ManualPolnCalibrator::get_response_nsub () const
{
  //if (!built || response.size() == 0)
  //  return response.size ();
  return 0;
}

void Pulsar::ManualPolnCalibrator::load (const string& ascii_model_filename)
{
  std::ifstream input (ascii_model_filename.c_str());
  if (!input)
  {
    throw Error (FailedSys, "Pulsar::ManualPolnCalibrator::load",
                 "ifstream (" + ascii_model_filename + ")");
  }
  string line;
  //Entry entry;

  while (input) try
  {
    getline (input, line);

    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    if (verbose > 2) cerr << "Pulsar::ManualPolnCalibrator::load '"<< line << "'" << endl;

    entries.push_back( Entry(line) );
  }
  catch (Error& error)
  {
    cerr << "Pulsar::Database::load discarding entry:" << error << endl;
  }
  if (verbose > 2) cerr << "Pulsar::ManualPolnCalibrator::load " << entries.size() << " entries" <<endl;
  input.close();
}

// TODO GD !!
/* 
Jones<float> Pulsar::ManualPolnCalibrator::set_response(std::vector<string> words) 
{
  std::complex<float> j00, j01, j10, j11;
  j00 = std::complex<float> (words[2], words[3]);
  return Jones(j00, j01, j10, j11);
}
*/

void Pulsar::ManualPolnCalibrator::calibrate (Archive* arch) try
{
  if (verbose > 2)
  {
    cerr << "Pulsar::ManualPolnCalibrator::calibrate" << endl;
  }
  std::string site = arch->get_telescope();

  if (arch->get_npol() == 4)
  {
    if (arch->get_state() != Signal::Coherence )
    {
      throw Error (InvalidState, "Pulsar::ManualPolnCalibrator::calibrate",
                   "The archive needs to be in Coherence state");
    }
    // Response need to be a vector of vector
    vector< vector<Jones<float> > > response(arch->get_nsubint(), vector< Jones<float> > (arch->get_nchan()));
    //response.resize(arch->get_nsubint());
    for (unsigned isub=0; isub < arch->get_nsubint(); isub++)
    {
      //response[isub].resize(arch->get_nchan());
      for (unsigned ichan=0; ichan < arch->get_nchan(); ichan++) {
      }
      std::vector<Entry> best_match = match(arch->get_Integration(isub)->get_epoch());
      for (unsigned ichan=0; ichan < arch->get_nchan(); ichan++)
      {
          // The number of channels in the calibration file may not match the actual data.
          // M. Keith Dec-2019
          //
          // Frequency in .jones files is in Hz, so get our channel freq in Hz.
          const double freq = 1e6*arch->get_Integration(isub)->get_centre_frequency(ichan);

          // This could be done much cleaner with iterators, but doesn't seem to be the style for psrchive.
          // In any case, should not be too bad.
          Entry best_freq_match = best_match.at(0);
          double best_df=abs(best_freq_match.ref_frequency-freq);
          for (unsigned imatch = 0; imatch < best_match.size() ; ++imatch){
              const double df = abs(best_match.at(imatch).ref_frequency-freq);
              if (df < best_df){
                  best_df = df;
                  best_freq_match = best_match.at(imatch);
              }
          }
        //response[isub][ichan] = inv(best_match[ichan].get_response());
        //response[isub][ichan] = inv((best_match.at(ichan)).get_response());
        response[isub][ichan] = inv((best_freq_match).get_response());

	/*

        cout << "response[" << isub << "][" << ichan << "].j00 " << response[isub][ichan].j00 << \
                " response[" << isub << "][" << ichan << "].j01 " << response[isub][ichan].j01 << \
                " response[" << isub << "][" << ichan << "].j10 " << response[isub][ichan].j10 << \
                " response[" << isub << "][" << ichan << "].j11 " << response[isub][ichan].j11 << endl;

	*/
      }
      //cout << endl;
    }
    arch->transform (response);
    arch->set_poln_calibrated (true);
  }
  else throw Error (InvalidState, "Pulsar::ManualPolnCalibrator::calibrate",
                    "Only npol=4 is supported");
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::calibrate";
}

////////////////////////////////////////////////////////////////////////

void Pulsar::ManualPolnCalibrator::Entry::init ()
{
  ref_epoch = 0.0;
  ref_frequency = 0.0;
}

//! Destructor
Pulsar::ManualPolnCalibrator::Entry::~Entry ()
{
}

// load from ASCII string
void Pulsar::ManualPolnCalibrator::Entry::load (const string& str)
{
  const string whitespace = " \t\n";
  string temp = str;
  std::vector<string> words = stringdecimate(temp, whitespace);

  if( words.size() < 10 )
  {
    throw Error(InvalidState,"Pulsar::ManualPolnCalibrator::Entry::load",
                "could not parse 10 words from '"+temp+"'");
  }
  ref_epoch = MJD(words[0]);
  ref_frequency = atof(words[1].c_str());
  std::complex<float> j00, j01, j10, j11;
  j00 = std::complex<float> ( atof( words[2].c_str() ), atof( words[3].c_str() ) );
  j01 = std::complex<float> ( atof( words[4].c_str() ), atof( words[5].c_str() ) );
  j10 = std::complex<float> ( atof( words[6].c_str() ), atof( words[7].c_str() ) );
  j11 = std::complex<float> ( atof( words[8].c_str() ), atof( words[9].c_str() ) );
  Jones<float> tmp_jones = Jones<float>(j00, j01, j10, j11);
  set_response( tmp_jones );
}

//! Return true if the source name matches
std::vector<Pulsar::ManualPolnCalibrator::Entry> Pulsar::ManualPolnCalibrator::matches_epoch (const MJD& epoch) const
{
  Entry best_match;
  best_match.ref_epoch = 0.0;
  std::vector<Entry> best_entries;

  for (unsigned ie=0; ie<entries.size(); ie++)
  {
    if (best_match.ref_epoch == 0.0)
    {
      best_match = entries[ie];
      best_entries.push_back(entries[ie]);
    }
    else
    {
      double diff = fabs(cast_double(entries[ie].ref_epoch - epoch));
      double best_diff = fabs(cast_double(best_match.ref_epoch - epoch));
      if (diff < best_diff)
      {
        //cerr << "GD best "<< diff << " " << best_diff << " " << ie <<endl;
        //exit(0);
        // Reset the entries of best and add this one
        best_match = entries[ie];
        best_entries.clear();
        best_entries.push_back(entries[ie]);
      }
      if (diff == best_diff) {
        // Add this one
        //cout << "response" <<entries[ie].get_response() << endl;
        best_entries.push_back(entries[ie]);
      }
    }
  }
  //cout << "gd" << best_entries.size() << endl;
  return best_entries;
}

std::vector<Pulsar::ManualPolnCalibrator::Entry>
Pulsar::ManualPolnCalibrator::match (const MJD& epoch) const
{
  Entry best_match;
  std::vector<Entry> e_entries;
  e_entries = matches_epoch(epoch);

  /*
  for (unsigned ie=0; ie<e_entries.size(); ie++) {
    //if (entries[ie].matches_epoch(epoch)) {
      if  (best_match.ref_frequency == 0)
        best_match = e_entries[ie];
      else {
        double diff = fabs(e_entries[ie].ref_frequency - MHz);
        double best_diff = fabs(best_match.ref_frequency - MHz);
        if (diff < best_diff)
          best_match = e_entries[ie];
      }
    //}
  }
*/
  //return best_match;
  return e_entries;
}


//! Return the Entry that is closest in time and frequency
const Pulsar::ManualPolnCalibrator::Entry& 
Pulsar::ManualPolnCalibrator::match (const MJD& epoch, double freq_MHz) const
{
  if (entries.size() == 0)
    throw Error (InvalidState, "Pulsar::ManualPolnCalibrator::match",
                 "no entries loaded");

  const Entry* best_match = &(entries[0]);

  double frequency = freq_MHz * 1e6;

  for (unsigned ie=1; ie<entries.size(); ie++)
  {
    double diff = fabs(cast_double(entries[ie].ref_epoch - epoch));
    double best_diff = fabs(cast_double(best_match->ref_epoch - epoch));

    if (diff < best_diff)
      best_match = &(entries[ie]);

    else if (diff == best_diff)
    {
      diff = fabs(entries[ie].ref_frequency - frequency);
      best_diff = fabs(best_match->ref_frequency - frequency);
      if (diff < best_diff)
        best_match = &(entries[ie]);
    }
  }

  return *best_match;
}

