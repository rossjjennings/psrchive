#include "Archive.h"
#include "Integration.h"
#include "Error.h"

#include "coord.h"

bool Pulsar::Archive::verbose = false;

void Pulsar::Archive::init ()
{
  if (verbose)
    cerr << "Pulsar::Archive::init" << endl;

  model_updated = false;
}

Pulsar::Archive::Archive () 
{ 
  if (verbose)
    cerr << "Pulsar::Archive::null constructor" << endl;

  init(); 
}

Pulsar::Archive::~Archive () 
{ 
  if (verbose)
    cerr << "Pulsar::Archive::destructor" << endl;

  for (unsigned isub=0; isub<subints.size(); isub++)
    delete subints[isub];
}

void Pulsar::Archive::resize (int nsubint, int npol, int nchan, int nbin)
{
  if (verbose)
    cerr << "Pulsar::Archive::resize nsub=" << nsubint << " npol=" << npol
	 << " nchan=" << nchan << " nbin=" << nbin << endl;

  int isub, nsub = (int) subints.size();

  if (verbose)
    cerr << "Pulsar::Archive::resize delete " 
	 << nsubint << "->" << nsub << " old subints" << endl;

  for (isub=nsubint; isub<nsub; isub++)
    delete subints[isub];

  subints.resize (nsubint);

  if (verbose)
    cerr << "Pulsar::Archive::resize create " 
	 << nsub << "->" << nsubint << " new subints" << endl;

  for (isub=nsub; isub<nsubint; isub++)
    subints[isub] = new_Integration ();

  if (verbose)
    cerr << "Pulsar::Archive::resize subints" << endl;

  for (isub=0; isub<nsubint; isub++)
    subints[isub] -> resize (npol, nchan, nbin);

  if (verbose)
    cerr << "Pulsar::Archive::resize calling book-keeping functions" << endl;

  set_nsubint (nsubint);
  set_npol (npol);
  set_nchan (nchan);
  set_nbin (nbin);

  if (verbose)
    cerr << "Pulsar::Archive::resize exit" << endl;
}

//! Return a pointer to the integration
Pulsar::Integration* Pulsar::Archive::get_Integration (unsigned subint)
{
  if (subint < subints.size())
    return subints[subint];

  throw Error (InvalidRange, "Archive::get_Integration",
	       "isubint=%u nsubint=%u", subint, subints.size());
}

//! Return a pointer to the profile
Pulsar::Profile* 
Pulsar::Archive::get_Profile (unsigned subint, int pol, int chan)
{
  return get_Integration (subint) -> get_Profile (pol, chan);
}

/*!  
  This method may be useful during load.  This function assumes
  that the Integration is totally uninitialized.  As the folding
  period is unknown until the mid_time of the integration is known,
  Integration::set_folding_period is not called by this method.  
*/
void Pulsar::Archive::init_Integration (Integration* subint)
{
  subint -> set_centre_frequency ( get_centre_frequency() );
  subint -> set_bandwidth ( get_bandwidth() );
  subint -> set_dispersion_measure ( get_dispersion_measure() );
  subint -> set_feed_type ( get_feed_type() );
  subint -> set_poln_state ( get_poln_state() );
}

/*!
  \param nscrunch the number of phase bins to add together
  */
void Pulsar::Archive::bscrunch (int nscrunch)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> bscrunch (nscrunch);

  set_nbin (subints[0]->get_nbin());
}

/*!
  \param nscrunch 
  \param weighted_cfreq
  */
void Pulsar::Archive::fscrunch (int nscrunch, bool weighted_cfreq)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> fscrunch (nscrunch);

  set_nchan (subints[0]->get_nchan());
}


void Pulsar::Archive::pscrunch()
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> pscrunch ();

  set_npol (subints[0]->get_npol());
}

void Pulsar::Archive::centre ()
{

}

void Pulsar::Archive::correct()
{

}

void Pulsar::Archive::dedisperse (double dm, double frequency)
{

}

void Pulsar::Archive::fold (int nfold)
{
  if (subints.size() == 0)
    return;

  for (unsigned isub=0; isub < subints.size(); isub++)
    subints[isub] -> fold (nfold);

  set_nbin (subints[0]->get_nbin());
}

void Pulsar::Archive::toas (const Archive& standard,
                       vector<Tempo::toa>& toas, int mode, bool wt)
{

}

void Pulsar::Archive::deparallactify()
{

}

void Pulsar::Archive::parallactify()
{

}

void Pulsar::Archive::ppqq()
{

}

void Pulsar::Archive::iquv()
{

}

void Pulsar::Archive::invint ()
{

}

void Pulsar::Archive::remove_baseline ()
{

}

void Pulsar::Archive::rotate (double time)
{

}

void Pulsar::Archive::rotate (const Phase& shift)
{

}

void Pulsar::Archive::RM_correct (double rotation_measure, double rm_iono)
{

}

void Pulsar::Archive::set_ephem (const psrephem& e)
{

}

void Pulsar::Archive::set_polyco (const polyco& p)
{

}

void Pulsar::Archive::snr_weight ()
{

}


MJD Pulsar::Archive::start_time() const
{
  if (subints.size() < 1)
    throw Error (InvalidState, "Archive::start_time", "no subints");

  return subints[0] -> get_start_time();
}

MJD Pulsar::Archive::end_time() const
{
  if (subints.size() < 1)
    throw Error (InvalidState, "Archive::end_time", "no subints");

  return subints[0] -> get_end_time();
}

/*!
  \retval lat latitude in degrees
  \retval lon longitude in degrees
  \retval ele elevation in metres
*/
void
Pulsar::Archive::telescope_coordinates 
(float* lat, float* lon, float* ele) const
{
  int ret = telescope_coords (get_tel_tempo_code(), lat, lon, ele);
  if (ret < 0)
    throw Error (FailedCall, "Archive::telescope_coordinates",
		 "tempo code=%c", get_tel_tempo_code ());
}
