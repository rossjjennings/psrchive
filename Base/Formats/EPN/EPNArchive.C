#include "Pulsar/EPNArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include "epnio.h"

void Pulsar::EPNArchive::init ()
{
  // initialize the EPNArchive attributes
}

Pulsar::EPNArchive::EPNArchive()
{
  if (verbose)
    cerr << "EPNArchive construct" << endl;

  init ();
}

Pulsar::EPNArchive::~EPNArchive()
{
  // destroy any EPNArchive resources
}

Pulsar::EPNArchive::EPNArchive (const Archive& arch)
{
  if (verbose)
    cerr << "EPNArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::EPNArchive::EPNArchive (const EPNArchive& arch)
{
  if (verbose)
    cerr << "EPNArchive construct copy EPNArchive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::EPNArchive::EPNArchive (const Archive& arch, 
				const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "EPNArchive construct extract Archive" << endl;

  init ();
  Archive::copy (arch, subints);
}


void Pulsar::EPNArchive::copy (const Archive& archive, 
			       const vector<unsigned>& subints)
{
  if (verbose)
    cerr << "EPNArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose)
    cerr << "EPNArchive::copy dynamic cast call" << endl;
  
  const EPNArchive* like_me = dynamic_cast<const EPNArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose)
    cerr << "EPNArchive::copy another EPNArchive" << endl;

  // copy EPNArchive attributes
}

Pulsar::EPNArchive* Pulsar::EPNArchive::clone () const
{
  if (verbose)
    cerr << "EPNArchive::clone" << endl;
  return new EPNArchive (*this);
}

Pulsar::EPNArchive* 
Pulsar::EPNArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose)
    cerr << "EPNArchive::extract" << endl;
  return new EPNArchive (*this, subints);
}


char Pulsar::EPNArchive::get_telescope_code () const
{
  return '7';
}

void Pulsar::EPNArchive::set_telescope_code (char code)
{

}

Signal::Source Pulsar::EPNArchive::get_type () const
{
  return Signal::Pulsar;
}


void Pulsar::EPNArchive::set_type (Signal::Source type)
{

}

string fortran_string (const char* fortran)
{
  string result;
  while (*fortran != ' ') {
    result += *fortran;
    fortran ++;
  }

  return result;
}


string Pulsar::EPNArchive::get_source () const
{
  return fortran_string( line2.jname );
}

void Pulsar::EPNArchive::set_source (const string& source)
{
  strncpy (line2.jname, source.c_str(), 12);
}



/*! From line 3 of the header:

  c     rah         - I2     - Hours of right ascension (J2000)
  c     ram         - I2     - Mins. of right ascension (J2000)
  c     ras         - F6.3   - Secs. of right ascension (J2000)
  c     ded         - I3     - Degrees of declination   (J2000)
  c     dem         - I2     - Minutes of declination   (J2000)
  c     des         - F6.3   - Seconds of declination   (J2000)
*/
sky_coord Pulsar::EPNArchive::get_coordinates () const
{
  sky_coord coordinates;

  coordinates.ra().setHMS( line3.rah, line3.ram, line3.ras);
  coordinates.dec().setHMS( line3.ded, line3.dem, line3.des);

  return coordinates;
}

//! Set the coordinates of the source
void Pulsar::EPNArchive::set_coordinates (const sky_coord& coordinates)
{

}

unsigned Pulsar::EPNArchive::get_nbin () const
{
  if (line5.nbin < 0)
    return 0;

  return line5.nbin;
}

void Pulsar::EPNArchive::set_nbin (unsigned numbins)
{
  line5.nbin = numbins;
}

unsigned Pulsar::EPNArchive::get_nchan () const
{
  if (line5.nfreq < 0)
    return 0;

  return line5.nfreq;
}

void Pulsar::EPNArchive::set_nchan (unsigned numchan)
{
  line5.nfreq = numchan;
}


unsigned Pulsar::EPNArchive::get_npol () const
{
  if (line5.npol < 0)
    return 0;

  return line5.npol;
}

void Pulsar::EPNArchive::set_npol (unsigned numpol)
{
  line5.npol = numpol;
}

unsigned Pulsar::EPNArchive::get_nsubint () const
{
  if (line5.nint < 0)
    return 0;

  return line5.nint;
}

void Pulsar::EPNArchive::set_nsubint (unsigned nsubint)
{
  line5.nint = nsubint;
}

double Pulsar::EPNArchive::get_bandwidth () const
{
  return 0;
}

void Pulsar::EPNArchive::set_bandwidth (double bw)
{
  
}

double Pulsar::EPNArchive::get_centre_frequency () const
{
  return 0;
}

void Pulsar::EPNArchive::set_centre_frequency (double cf)
{
  
}


Signal::State Pulsar::EPNArchive::get_state () const
{
  return Signal::Stokes;
}

void Pulsar::EPNArchive::set_state (Signal::State state)
{

}

//! Get the scale of the profiles
Signal::Scale Pulsar::EPNArchive::get_scale () const
{
  return Signal::FluxDensity;
}

//! Set the scale of the profiles
void Pulsar::EPNArchive::set_scale (Signal::Scale scale)
{

}


//! Get the centre frequency of the observation
double Pulsar::EPNArchive::get_dispersion_measure () const
{
  return line2.dm;
}

//! Set the centre frequency of the observation
void Pulsar::EPNArchive::set_dispersion_measure (double dm)
{
  line2.dm = dm;
}

//! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
double Pulsar::EPNArchive::get_rotation_measure () const
{
  return line2.rm;
}

//! Set the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
void Pulsar::EPNArchive::set_rotation_measure (double rm)
{
  line2.rm = rm;
}


bool Pulsar::EPNArchive::get_poln_calibrated () const
{
  return false;
}

void Pulsar::EPNArchive::set_poln_calibrated (bool done)
{

}


bool Pulsar::EPNArchive::get_faraday_corrected () const
{
  return false;
}

void Pulsar::EPNArchive::set_faraday_corrected (bool done)
{

}


bool Pulsar::EPNArchive::get_dedispersed () const
{
  return false;
}

void Pulsar::EPNArchive::set_dedispersed (bool done)
{

}

//
//
//
Pulsar::Integration* 
Pulsar::EPNArchive::new_Integration (Integration* subint)
{
  if (verbose)
    cerr << "Pulsar::EPNArchive::new_Integration" << endl;

  BasicIntegration* integration;

  if (subint)
    integration = new BasicIntegration (*subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAllocation, "Pulsar::EPNArchive::new_Integration");
  
  return integration;
}


void Pulsar::EPNArchive::load_header (const char* filename)
{
  // load all BasicArchive and EPNArchive attributes from filename
  string fortran_string = filename;
  fortran_string += " ";

  char* name = const_cast<char*>(fortran_string.c_str());

  int readwri = -1;  // read
  int recno   = 0;   // 
  int padout  = 0;   // no padding

  if (verbose)
    cerr << "Pulsar::EPNArchive::load_header call rwepn " << name << endl;

  F772C(rwepn) (name, &readwri, &recno, &padout, strlen (name));

  line1 = F772C(epn1);
  line2 = F772C(epn2);
  line3 = F772C(epn3);
  line4 = F772C(epn4);
  line5 = F772C(epn5);

  if (verbose)
    cerr << "Pulsar::EPNArchive::load_header rwepn called" << endl;
}

Pulsar::Integration*
Pulsar::EPNArchive::load_Integration (const char* filename, unsigned subint)
{
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // load all BasicIntegration attributes and data from filename

  return integration;
}

void Pulsar::EPNArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and EPNArchive attributes as well as
  // BasicIntegration attributes and data to filename
}



string Pulsar::EPNArchive::Agent::get_description () 
{
  return "EPN Archive Version 1.0";
}

bool Pulsar::EPNArchive::Agent::advocate (const char* filename)
{
  FILE* fptr = fopen (filename, "r");
  if (!fptr)
    return false;

  float version;
  int scanned = fscanf (fptr, "EPN %f", &version);
  fclose (fptr);

  if (scanned != 1)
    return false;

  if (verbose)
    cerr << "Pulsar::EPNArchive::Agent::advocate EPN version " << version << endl;

  return true;
}

