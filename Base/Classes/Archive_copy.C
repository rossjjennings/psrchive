#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

void Pulsar::Archive::copy (const Archive& archive){
  vector<unsigned> all_subints(archive.get_nsubint());
  for( unsigned i=0; i<all_subints.size(); i++)
    all_subints[i] = i;

  copy(archive,all_subints);
}

void Pulsar::Archive::copy (const Archive& archive,
			    const vector<unsigned>& selected)
{
  if (verbose)
    cerr << "Pulsar::Archive::copy" << endl;

  if (this == &archive)
    return;
  
  // copy only the selected subints
  resize (selected.size(), archive.get_npol(),
	  archive.get_nchan(), archive.get_nbin());
  
  for (unsigned isub=0; isub<selected.size(); isub++) {
    const Integration* subint = archive.get_Integration( selected[isub] );
    get_Integration(isub) -> copy (*(subint));
  }
  
  if (archive.ephemeris)
    ephemeris = new psrephem (*(archive.ephemeris));
  else
    ephemeris = 0;

  if (archive.model)
    model = new polyco (*(archive.model));
  else
    model = 0;

  // set virtual attributes

  set_nbin( archive.get_nbin() );
  set_nchan( archive.get_nchan() );
  set_npol( archive.get_npol() );
  set_telescope_code( archive.get_telescope_code() );
  set_basis( archive.get_basis() );
  set_state( archive.get_state() );
  set_type( archive.get_type() );
  set_source( archive.get_source() );
  set_receiver( archive.get_receiver() );
  set_backend( archive.get_backend() );
  set_coordinates( archive.get_coordinates() );
  set_bandwidth( archive.get_bandwidth() );
  set_centre_frequency( archive.get_centre_frequency() );
  set_dispersion_measure( archive.get_dispersion_measure() );
  
  set_feedangle_corrected( archive.get_feedangle_corrected() );
  set_iono_rm_corrected( archive.get_iono_rm_corrected() );
  set_ism_rm_corrected( archive.get_ism_rm_corrected() );
  set_parallactic_corrected( archive.get_parallactic_corrected() );
  set_dedispersed( archive.get_dedispersed() );
  set_poln_calibrated( archive.get_poln_calibrated() );
  set_flux_calibrated( archive.get_flux_calibrated() );

  // Using a Reference::To<Extension> ensures that the cloned
  // Extension will be deleted if the derived class chooses not to
  // manage it.

  if (verbose)
    cerr << "Pulsar::Archive::copy " << archive.get_nextension()
	 << " Extensions" << endl;

  extension.resize (0);

  for (unsigned iext=0; iext < archive.get_nextension(); iext++) {

    if (verbose)
      cerr << "Pulsar::Archive::copy clone " 
	   << archive.get_extension(iext)->get_name() << endl;

    Reference::To<Extension> ext = archive.get_extension(iext)->clone();
    add_extension (ext);

  }

}


