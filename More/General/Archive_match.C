#include "Archive.h"

#include "string_utils.h"  // for stringprintf

/*!
  Maximum frequency difference in MHz, used by calibrator_match
*/
double Pulsar::Archive::match_max_frequency_difference = 0.1; // 100 kHz

bool Pulsar::Archive::match_opposite_sideband = false;

string Pulsar::Archive::match_indent = "\n    ";

// ///////////////////////////////////////////////////////////////////////
//
// Archive::mixable
//
/*!  This function defines the set of observing and processing
  parameters that should be equal (within certain limits) before two
  archives may be combined using append.  This method calls:
  <UL>
  <LI> standard_match
  <LI> processing_match
  <LI> calibrator_match
  </UL>
  \param archive pointer to Archive to be compared with this
  \retval reason if match fails, describes why in English
  \return true if archive matches this
*/
bool Pulsar::Archive::mixable (const Archive* archive, string& reason) const
{
  bool result = true;

  if (!standard_match (archive, reason))
    result = false;

  if (!processing_match (archive, reason))
    result = false;

  if (!calibrator_match (archive, reason))
    result = false;

  return result;
}

// ///////////////////////////////////////////////////////////////////////
//
// Archive::standard_match
//
/*!  This function defines the minimum set of observing parameters
  that should be equal before two archives are compared, as during
  the fitting of a profile to a standard template.  The following are
  tested for equality:
  <UL>
  <LI> get_state
  <LI> get_type
  <LI> get_source
  <LI> get_nbin
  </UL>
  \param archive pointer to Archive to be compared with this
  \retval reason if match fails, describes why in English
  \return true if archive matches this 
*/
bool Pulsar::Archive::standard_match (const Archive* archive,
				      string& reason) const
{
  bool result = true;

  if (get_state() != archive->get_state()) {
    reason += match_indent
      + stringprintf ("polarimetric state mismatch: %s != %s",
		      Signal::state_string(get_state()),
		      Signal::state_string(archive->get_state()));
    result = false;
  }
  
  if (get_type() != archive->get_type()) {
    reason += match_indent
      + stringprintf ("observation type mismatch: %s != %s",
		      Signal::source_string(get_type()),
		      Signal::source_string(archive->get_type()));
    result = false;
  }

  if (get_source() != archive->get_source()) {
    reason += match_indent
      + "source name mismatch: " + get_source() + " != " + archive->get_source();
    result = false;
  }

  if (get_nbin() != archive->get_nbin()) {
    reason += match_indent
      + stringprintf ("numbers of bins mismatch: %d != %d",
		      get_nbin(), archive->get_nbin());
    result = false;
  }

  return result;
}

// ///////////////////////////////////////////////////////////////////////
//
// Archive::processing_match
//
/*!  This function defines the minimum set of processing parameters
  that should be equal before data from two archives are combined,
  The following are tested for equality:
  <UL>
  <LI> get_state
  <LI> get_type
  <LI> get_source
  <LI> get_nbin
  </UL>
  \param archive pointer to Archive to be compared with this
  \retval reason if match fails, describes why in English
  \return true if archive matches this
*/
bool Pulsar::Archive::processing_match (const Archive* archive,
					string& reason) const
{
  bool result = true;

  //! Data has been flux calibrated
  if (get_flux_calibrated() != archive->get_flux_calibrated()) {
    reason += match_indent
      + stringprintf ("flux calibration mismatch");
    result = false;
  }

  //! Data has been corrected for feed angle errors
  if (get_feedangle_corrected() != archive->get_feedangle_corrected()) {
    reason += match_indent
      + stringprintf ("feed angle correction mismatch");
    result = false;
  }

  //! Data has been corrected for ionospheric faraday rotation
  if (get_iono_rm_corrected() != archive->get_iono_rm_corrected()) {
    reason += match_indent
      + stringprintf ("ionospheric RM correction mismatch");
    result = false;
  }

  //! Data has been corrected for ISM faraday rotation
  if (get_ism_rm_corrected() != archive->get_ism_rm_corrected()) {
    reason += match_indent
      + stringprintf ("ISM RM correction mismatch");
    result = false;
  }
  
  //! Data has been corrected for parallactic angle errors
  if (get_parallactic_corrected() != archive->get_parallactic_corrected()) {
    reason += match_indent
      + stringprintf ("parallactic angle correction mismatch");
    result = false;
  }
  
  //! Inter-channel dispersion delay has been removed
  if (get_dedispersed() != archive->get_dedispersed()) {
    reason += match_indent
      + stringprintf ("dedispersion mismatch");
    result = false;
  }

  return result;
}



// ///////////////////////////////////////////////////////////////////////
//
// Archive::calibrator_match
//
/*!  This function defines the minimum set of observing parameters
  that should be equal (within certain limits) before a calibrator may
  be applied to a pulsar observation.

  \param archive pointer to Archive to be compared with this
  \retval reason if match fails, describes why in English
  \return true if archive matches this
*/
bool Pulsar::Archive::calibrator_match (const Archive* archive,
					string& reason) const
{
  bool result = true;

  if (get_basis() != archive->get_basis()) {
    reason += match_indent
      + stringprintf ("feed type mismatch: %d != %d", 
		      get_basis(), archive->get_basis());
    result = false;
  }
  if (get_nchan() != archive->get_nchan()) {
    reason += match_indent
      + stringprintf ("numbers of channels mismatch: %d != %d",
		      get_nchan(), archive->get_nchan());
    result = false;
  }

  double cf1 = get_centre_frequency();
  double cf2 = archive->get_centre_frequency();
  double dfreq = fabs (cf2 - cf1);

  if (dfreq > match_max_frequency_difference) {
    reason += match_indent
      + stringprintf ("centre frequency mismatch: %lf and %lf", cf1, cf2);
    result = false;
  }

  double bw1 = get_bandwidth();
  double bw2 = archive->get_bandwidth();

  if ( ( bw1 != bw2 ) && !( match_opposite_sideband && (bw1 != -bw2) ) ) {
    reason += match_indent
      + stringprintf ("bandwidth mismatch: %lf and %lf", bw1, bw2);
    result = false;
  }
  
  return result;
}

