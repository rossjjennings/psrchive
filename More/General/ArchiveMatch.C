/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/ArchiveMatch.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Config.h"

#include "strutil.h"  // for stringprintf

/*!
  Maximum frequency difference in MHz
*/
double Pulsar::ArchiveMatch::max_frequency_difference 
= Pulsar::config.get<double>("max_frequency_difference", 0.1);

/*!
  Allow observations with opposite sidebands to match
*/
bool Pulsar::ArchiveMatch::opposite_sideband
= Pulsar::config.get<bool>("opposite_sideband", false);

/*!
  Added between match report strings in reason attribute
*/
std::string Pulsar::ArchiveMatch::separator = "\n\t";

Pulsar::ArchiveMatch::ArchiveMatch ()
{
  check_state = false;
  check_type = false;
  check_source = false;
  check_scale = false;
  check_faraday_corrected = false;
  check_dedispersed = false;
  check_nbin = false;
  check_nchan = false;
  check_npol = false;
  check_receiver = false;
  check_centre_frequency = false;
  check_bandwidth = false;
}

// ///////////////////////////////////////////////////////////////////////
//
// ArchiveMatch::check_mixable
//
/*!  This state defines the set of observing and processing
  parameters that should be equal (within certain limits) before two
  archives may be combined (for instance, using append).  This method
  calls:
  <UL>
  <LI> set_check_standard
  <LI> set_check_processing
  <LI> set_check_calibrator
  </UL>
*/
void Pulsar::ArchiveMatch::set_check_mixable (bool flag)
{
  set_check_standard (flag);
  set_check_processing (flag);
  set_check_calibrator (flag);
}

bool Pulsar::ArchiveMatch::get_check_mixable () const
{
  return
    get_check_standard() &&
    get_check_processing() &&
    get_check_calibrator();
}

// ///////////////////////////////////////////////////////////////////////
//
// ArchiveMatch::check_standard
//
/*!  This state defines the minimum set of observing parameters
  that should be equal before two archives are compared, as during
  the fitting of a profile to a standard template.  The following 
  flags are set:
  <UL>
  <LI> check_state
  <LI> check_type
  <LI> check_source
  <LI> check_nbin
  </UL>
*/
void Pulsar::ArchiveMatch::set_check_standard (bool flag)
{
  set_check_state (flag);
  set_check_type (flag);
  set_check_source (flag);
  set_check_nbin (flag);
}

bool Pulsar::ArchiveMatch::get_check_standard () const
{
  return
    get_check_state() && 
    get_check_type() &&
    get_check_source() &&
    get_check_nbin();
}

// ///////////////////////////////////////////////////////////////////////
//
// ArchiveMatch::check_processing
//
/*!  This state defines the minimum set of processing parameters
  that should be equal before data from two archives are combined.
  The following flags are set:
  <UL>
  <LI> check_scale
  <LI> check_faraday_corrected
  <LI> check_dedispersed
  <LI> check_nchan
  </UL>
*/
void Pulsar::ArchiveMatch::set_check_processing (bool flag)
{
  set_check_scale (flag);
  set_check_faraday_corrected (flag);
  set_check_dedispersed (flag);
  set_check_nchan (flag);
}

bool Pulsar::ArchiveMatch::get_check_processing () const
{
  return
    get_check_scale () &&
    get_check_faraday_corrected () &&
    get_check_dedispersed () &&
    get_check_nchan ();
}

// ///////////////////////////////////////////////////////////////////////
//
// ArchiveMatch::check_calibrator
//
/*!  This state defines the minimum set of observing parameters
  that should be equal (within certain limits) before a calibrator may
  be applied to a pulsar observation.
  The following flags are set
  <UL>
  <LI> check_receiver
  <LI> check_centre_frequency
  <LI> check_bandwidth
  </UL>

*/

void Pulsar::ArchiveMatch::set_check_calibrator (bool flag)
{
  set_check_receiver (flag);
  set_check_centre_frequency (flag);
  set_check_bandwidth (flag);
}

bool Pulsar::ArchiveMatch::get_check_calibrator () const
{
  return
    get_check_receiver () &&
    get_check_centre_frequency () &&
    get_check_bandwidth ();
}

//! Set to check the state attribute
void Pulsar::ArchiveMatch::set_check_state (bool flag)
{
  check_state = flag;
}

bool Pulsar::ArchiveMatch::get_check_state () const
{
  return check_state;
}

//! Set to check the type attribute
void Pulsar::ArchiveMatch::set_check_type (bool flag)
{
  check_type = flag;
}

bool Pulsar::ArchiveMatch::get_check_type () const
{
  return check_type;
}

//! Set to check the source attribute
void Pulsar::ArchiveMatch::set_check_source (bool flag)
{
  check_source = flag;
}

bool Pulsar::ArchiveMatch::get_check_source () const
{
  return check_source;
}

//! Set to check the scale attribute
void Pulsar::ArchiveMatch::set_check_scale (bool flag)
{
  check_scale = flag;
}

bool Pulsar::ArchiveMatch::get_check_scale () const
{
  return check_scale;
}

//! Set to check the faraday_corrected attribute
void Pulsar::ArchiveMatch::set_check_faraday_corrected (bool flag)
{
  check_faraday_corrected = flag;
}

bool Pulsar::ArchiveMatch::get_check_faraday_corrected () const
{
  return check_faraday_corrected;
}

//! Set to check the dedispersed attribute
void Pulsar::ArchiveMatch::set_check_dedispersed (bool flag)
{
  check_dedispersed = flag;
}

bool Pulsar::ArchiveMatch::get_check_dedispersed () const
{
  return check_dedispersed;
}

//! Set to check the nbin attribute
void Pulsar::ArchiveMatch::set_check_nbin (bool flag)
{
  check_nbin = flag;
}

bool Pulsar::ArchiveMatch::get_check_nbin () const
{
  return check_nbin;
}

//! Set to check the nchan attribute
void Pulsar::ArchiveMatch::set_check_nchan (bool flag)
{
  check_nchan = flag;
}

bool Pulsar::ArchiveMatch::get_check_nchan () const
{
  return check_nchan;
}

//! Set to check the npol attribute
void Pulsar::ArchiveMatch::set_check_npol (bool flag)
{
  check_npol = flag;
}

bool Pulsar::ArchiveMatch::get_check_npol () const
{
  return check_npol;
}

//! Set to check the receiver attribute
void Pulsar::ArchiveMatch::set_check_receiver (bool flag)
{
  check_receiver = flag;
}

bool Pulsar::ArchiveMatch::get_check_receiver () const
{
  return check_receiver;
}

//! Set to check the centre_frequency attribute
void Pulsar::ArchiveMatch::set_check_centre_frequency (bool flag)
{
  check_centre_frequency = flag;
}

bool Pulsar::ArchiveMatch::get_check_centre_frequency () const
{
  return check_centre_frequency;
}

//! Set to check the bandwidth attribute
void Pulsar::ArchiveMatch::set_check_bandwidth (bool flag)
{
  check_bandwidth = flag;
}

bool Pulsar::ArchiveMatch::get_check_bandwidth () const
{
  return check_bandwidth;
}

/*!  This method checks each of the archive attributes for which the
  check flag is set.  If a pair of checked attributes does not match,
  the result of this function is set to false and an appropriate
  message is added to the reason attribute.  If all checked attributes
  match, the result is true. */

bool Pulsar::ArchiveMatch::match (const Archive* a, const Archive* b)
{
  bool result = true;
  reason = "";

  // State of the signal
  if (check_state && a->get_state() != b->get_state()) {
    reason += separator
      + stringprintf ("signal state mismatch: %s != %s",
		      Signal::state_string(a->get_state()),
		      Signal::state_string(b->get_state()));
    result = false;
  }
  
  // Type of observation
  if (check_type && a->get_type() != b->get_type()) {
    reason += separator
      + stringprintf ("observation type mismatch: %s != %s",
		      Signal::source_string(a->get_type()),
		      Signal::source_string(b->get_type()));
    result = false;
  }

  // Name of observed source
  if (check_source && a->get_source() != b->get_source()) {
    reason += separator
      + "source name mismatch: " + a->get_source() + " != " + b->get_source();
    result = false;
  }

  // Data has been flux calibrated
  if (check_scale && a->get_scale() != b->get_scale()) {
    reason += separator
      + stringprintf ("scale mismatch");
    result = false;
  }

  // Data has been corrected for ISM faraday rotation
  if (check_faraday_corrected &&
      a->get_faraday_corrected() != b->get_faraday_corrected()) {
    reason += separator
      + stringprintf ("faraday rotation correction mismatch");
    result = false;
  }
  
  // Inter-channel dispersion delay has been removed
  if (check_dedispersed && a->get_dedispersed() != b->get_dedispersed()) {
    reason += separator
      + stringprintf ("dedispersion mismatch");
    result = false;
  }

  // number of phase bins
  if (check_nbin && a->get_nbin() != b->get_nbin()) {
    reason += separator
      + stringprintf ("number of bins mismatch: %d != %d",
		      a->get_nbin(), b->get_nbin());
    result = false;
  }

  // number of frequency channels
  if (check_nchan && a->get_nchan() != b->get_nchan()) {
    reason += separator
      + stringprintf ("numbers of channels mismatch: %d != %d",
		      a->get_nchan(), b->get_nchan());
    result = false;
  }

  // number of polarizations
  if (check_npol && a->get_npol() != b->get_npol()) {
    reason += separator
      + stringprintf ("numbers of channels mismatch: %d != %d",
		      a->get_npol(), b->get_npol());
    result = false;
  }

  if (check_receiver) {

    const Receiver* a_receiver = a->get<Receiver>();
    const Receiver* b_receiver = b->get<Receiver>();

    if ( a_receiver && !b_receiver ) {
      reason += separator + "receiver mismatch: have and haven't extension";
      result = false;
    }
    else if ( !a_receiver && b_receiver ) {
      reason += separator + "receiver mismatch: haven't and have extension";
      result = false;
    }
    else if (a_receiver && ! a_receiver->match (b_receiver, reason))
      result = false;

  }

  if (check_centre_frequency) {

    double cf1 = a->get_centre_frequency();
    double cf2 = b->get_centre_frequency();
    double dfreq = fabs (cf2 - cf1);

    if (dfreq > max_frequency_difference) {
      reason += separator
	+ stringprintf ("centre frequency mismatch: %lf and %lf", cf1, cf2);
      result = false;
    }

  }

  if (check_bandwidth) {

    double bw1 = a->get_bandwidth();
    double bw2 = b->get_bandwidth();
    
    if ( ( bw1 != bw2 ) && !(opposite_sideband && (bw1 == -bw2)) ) {
      reason += separator
	+ stringprintf ("bandwidth mismatch: %lf and %lf", bw1, bw2);
      result = false;
    }
    
  }

  return result;
}

