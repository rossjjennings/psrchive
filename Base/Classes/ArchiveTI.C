#include "Pulsar/ArchiveTUI.h"
#include "Pulsar/Archive.h"
#include "string_utils.h"

//! Process a command
void Pulsar::ArchiveTUI::process (const std::string& command)
{
  string temp = command;

  string param = stringtok (&temp, "=\t\n");

  TextInterface::Attribute<Archive>* attribute = find (param);

  string value = stringtok (&temp, " \t\n");

  if (value.length())
    attribute->set_value (instance, value);
  else
    cout << param << "=" << attribute->get_value (instance);
}

void Pulsar::ArchiveTUI::init ()
{
  {
    Generator<unsigned> gen;

    add( gen.described ("nbin", "Number of pulse phase bins",
			&Archive::get_nbin) );

    add( gen.described ("nchan", "Number of frequency channels",
			&Archive::get_nchan) );

    add( gen.described ("npol", "Number of polarizations",
			&Archive::get_npol) );

    add( gen.described ("nsub", "Number of sub-integrations",
			&Archive::get_nsubint) );
  }

  {
    Generator<char> gen;

    add( gen.described ("site", "Telescope tempo code",
			&Archive::get_telescope_code,
			&Archive::set_telescope_code) );
  }

#if 0

  //! Get the state of the profile data
  virtual Signal::State get_state () const = 0;
  //! Set the state of the profile data
  virtual void set_state (Signal::State state) = 0;
  
  //! Get the scale in which flux density is measured
  virtual Signal::Scale get_scale () const = 0;
  //! Set the scale in which flux density is measured
  virtual void set_scale (Signal::Scale scale) = 0;
  
  //! Get the observation type (psr, cal)
  virtual Signal::Source get_type () const = 0;
  //! Set the observation type (psr, cal)
  virtual void set_type (Signal::Source type) = 0;

#endif

  {
    Generator<string> gen;

    add( gen.described ("name", "Name of the source",
			&Archive::get_source,
			&Archive::set_source) );
  }

#if 0
  //! Get the coordinates of the source
  virtual sky_coord get_coordinates () const = 0;
  //! Set the coordinates of the source
  virtual void set_coordinates (const sky_coord& coordinates) = 0;
#endif

  {
    Generator<double> gen;

    add( gen.described ("freq", "Centre frequency (MHz)",
			&Archive::get_centre_frequency,
			&Archive::set_centre_frequency) );

    add( gen.described ("bw", "Bandwidth (MHz)",
			&Archive::get_bandwidth,
			&Archive::set_bandwidth) );

    add( gen.described ("dm", "Dispersion measure (pc/cm^3)",
			&Archive::get_dispersion_measure,
			&Archive::set_dispersion_measure) );

    add( gen.described ("rm", "Rotation measure (rad/m^2)",
			&Archive::get_rotation_measure,
			&Archive::set_rotation_measure) );
  }

  {
    Generator<bool> gen;

    add( gen.described ("dmc", "Dispersion corrected (boolean)",
			&Archive::get_dedispersed,
			&Archive::set_dedispersed) );

    add( gen.described ("rmc", "Faraday Rotation corrected (boolean)",
			&Archive::get_faraday_corrected,
			&Archive::set_faraday_corrected) );

    add( gen.described ("polc", "Polarization calibrated (boolean)",
			&Archive::get_poln_calibrated,
			&Archive::set_poln_calibrated) );
  }
}

