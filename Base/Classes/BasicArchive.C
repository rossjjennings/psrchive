#include "Pulsar/BasicArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Error.h"

void Pulsar::BasicArchive::ensure_linkage ()
{
}


Pulsar::BasicArchive::BasicArchive () 
{ 
  telescope_code = -1;

  basis = Signal::Linear;
  state = Signal::Intensity;

  type = Signal::Pulsar;

  source = "unknown";
  receiver = "unknown";
  backend = "unknown";
  
  nbin = 0;
  nchan = 1;
  npol = 1;
  nsubint = 0;

  bandwidth = 0.0;
  centre_frequency = 0.0;
  dispersion_measure = 0.0;

  flux_calibrated = false;
  poln_calibrated = false;
  feedangle_corrected = false;
  iono_rm_corrected = false;
  ism_rm_corrected = false;
  parallactic_corrected = false;
  dedispersed = false;
}

Pulsar::BasicArchive::BasicArchive (const BasicArchive& copy,
				    const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "Pulsar::BasicArchive:: copy constructor" << endl;

  Archive::copy (copy, subints);
}


const Pulsar::BasicArchive&
Pulsar::BasicArchive::operator = (const BasicArchive& copy)
{
  if (verbose)
    cerr << "Pulsar::BasicArchive::operator =" << endl;

  if (this == &copy)
    return *this;

  Archive::copy (copy);

  return *this;
}


Pulsar::BasicArchive::~BasicArchive () 
{ 
  if (verbose)
    cerr << "Pulsar::BasicArchive::destructor" << endl;
}

/*!  
  By over-riding this funciton, inherited types may re-define the type
  of Integration to which the elements of the subints vector point.
*/
Pulsar::Integration*
Pulsar::BasicArchive::new_Integration (Integration* subint)
{
  Integration* integration;

  if (subint)
    integration = new BasicIntegration (*subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAllocation, "BasicArchive::new_Integration");
  
  return integration;
}

//! Get the number of sub-integrations in the archive
unsigned Pulsar::BasicArchive::get_nsubint () const
{
  return nsubint;
}

//! Set the number of sub-integrations in the archive
void Pulsar::BasicArchive::set_nsubint (unsigned num_sub) 
{
  nsubint = num_sub;
}

//! Get the number of frequency polns used
unsigned Pulsar::BasicArchive::get_npol () const
{
  return npol;
}

//! Set the number of frequency polns used
void Pulsar::BasicArchive::set_npol (unsigned numpol)
{
  npol = numpol;
}

//! Get the number of frequency channels used
unsigned Pulsar::BasicArchive::get_nchan () const
{
  return nchan;
}

//! Set the number of frequency channels used
void Pulsar::BasicArchive::set_nchan (unsigned numchan)
{
  nchan = numchan;
}

//! Get the number of pulsar phase bins used
unsigned Pulsar::BasicArchive::get_nbin () const
{
  return nbin;
}

//! Set the number of pulsar phase bins used
void Pulsar::BasicArchive::set_nbin (unsigned numbins)
{
  nbin = numbins;
}

//! Get the tempo code of the telescope used
char Pulsar::BasicArchive::get_telescope_code () const
{
  return telescope_code;
}

//! Set the tempo code of the telescope used
void Pulsar::BasicArchive::set_telescope_code (char telcode)
{
  telescope_code = telcode;
}

//! Return the type of observation (psr, cal, etc.)
Signal::Source Pulsar::BasicArchive::get_type () const
{
  return type;
}

string Pulsar::BasicArchive::get_source () const
{
  return source;
}

string Pulsar::BasicArchive::get_receiver () const
{
  return receiver;
}

string Pulsar::BasicArchive::get_backend () const
{
  return backend;
}

//! Set the observation type (psr, cal etc.)
void Pulsar::BasicArchive::set_type (Signal::Source ob_type)
{
  type = ob_type;
}
    
void Pulsar::BasicArchive::set_source (const string& src)
{
  source = src;
}

void Pulsar::BasicArchive::set_receiver (const string& rec)
{
  receiver = rec;
}

void Pulsar::BasicArchive::set_backend (const string& bak)
{
  backend = bak;
}

//! Get the coordinates of the source
sky_coord Pulsar::BasicArchive::get_coordinates () const
{
  return coordinates;
}

//! Set the coordinates of the source
void Pulsar::BasicArchive::set_coordinates (const sky_coord& coords)
{
  coordinates = coords;
}

//! Return the bandwidth of the observation
double Pulsar::BasicArchive::get_bandwidth () const
{
  return bandwidth;
}
    
//! Set the bandwidth of the observation
void Pulsar::BasicArchive::set_bandwidth (double bw)
{
  bandwidth = bw;
}
    
//! Return the centre frequency of the observation
double Pulsar::BasicArchive::get_centre_frequency () const
{
  return centre_frequency;
}
    
//! Set the centre frequency of the observation
void Pulsar::BasicArchive::set_centre_frequency (double cf)
{
  centre_frequency = cf;
}
    
//! Return the type of feed used
Signal::Basis Pulsar::BasicArchive::get_basis () const
{
  return basis;
}

//! Set the type of feed used 
void Pulsar::BasicArchive::set_basis (Signal::Basis feed)
{
  basis = feed;
}
    
//! Return the polarisation state of the data
Signal::State Pulsar::BasicArchive::get_state () const
{
  return state;
}
    
//! Set the polarisation state of the data
void Pulsar::BasicArchive::set_state (Signal::State _state)
{
  state = _state;
}
    
//! Get the centre frequency of the observation
double Pulsar::BasicArchive::get_dispersion_measure () const
{
  return ephemeris.get_dm();
}

//! Set the centre frequency of the observation
void Pulsar::BasicArchive::set_dispersion_measure (double dm)
{
  ephemeris.set_dm (dm);
}

//! Data has been poln calibrated
bool Pulsar::BasicArchive::get_poln_calibrated () const
{
  return poln_calibrated;
}

//! Set the status of the poln calibrated flag
void Pulsar::BasicArchive::set_poln_calibrated (bool done)
{
  poln_calibrated = done;
}

//! Data has been flux calibrated
bool Pulsar::BasicArchive::get_flux_calibrated () const
{
  return flux_calibrated;
}

//! Set the status of the flux calibrated flag
void Pulsar::BasicArchive::set_flux_calibrated (bool done)
{
  flux_calibrated = done;
}


//! Return true when the data has been corrected for feed angle errors
bool Pulsar::BasicArchive::get_feedangle_corrected () const
{
  return feedangle_corrected;
}

//! Set true when the data has been corrected for feed angle errors
void Pulsar::BasicArchive::set_feedangle_corrected (bool done)
{
  feedangle_corrected = done;
}

//! Return true when the data has been corrected for ionospheric Faraday rotation
bool Pulsar::BasicArchive::get_iono_rm_corrected () const
{
  return iono_rm_corrected;
}
    
//! Set true when the data has been corrected for ionospheric Faraday rotation
void Pulsar::BasicArchive::set_iono_rm_corrected (bool done)
{
  iono_rm_corrected = done;
}
    
//! Return true when the data has been corrected for ISM Faraday rotation
bool Pulsar::BasicArchive::get_ism_rm_corrected () const
{
  return ism_rm_corrected;
}
    
//! Set true when the data has been corrected for ISM Faraday rotation
void Pulsar::BasicArchive::set_ism_rm_corrected (bool done)
{
  ism_rm_corrected = done;
}
    
//! Return true when the data has been corrected for parallactic angle errors
bool Pulsar::BasicArchive::get_parallactic_corrected () const
{
  return parallactic_corrected;
} 
    
//! Set true when the data has been corrected for parallactic angle errors
void Pulsar::BasicArchive::set_parallactic_corrected (bool done)
{
  parallactic_corrected = done;
}
    
//! Inter-channel dispersion delay has been removed
bool Pulsar::BasicArchive::get_dedispersed () const
{
  return dedispersed;
}

//! Set true when the inter-channel dispersion delay has been removed
void Pulsar::BasicArchive::set_dedispersed (bool done)
{
  dedispersed = done;
}
