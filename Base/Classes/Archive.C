#include "Archive.h"
#include "Integration.h"

bool Pulsar::Archive::verbose = false;

void Pulsar::Archive::init ()
{

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

void Pulsar::Archive::dedisperse (double dm = 0.0, double frequency = 0.0)
{

}

void Pulsar::Archive::fold (int nfold)
{

}

void Pulsar::Archive::toas (const Archive& standard,
                       vector<Tempo::toa>& toas, int mode=0, bool wt=false)
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

void Pulsar::Archive::invint (bool square_root = true, // take sqrt(II-QQ-UU-VV)
                         float baseline_ph=-1)
{

}

void Pulsar::Archive::remove_baseline (int poln = 0, float phase = -1.0)
{

}

void Pulsar::Archive::rotate (double time)
{

}

void Pulsar::Archive::rotate (const Phase& shift)
{

}

void Pulsar::Archive::RM_correct (double rotation_measure = 0, double rm_iono = 0)
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

void Pulsar::Archive::destroy ()
{

}

void Pulsar::Archive::resize (int nsubint, int nchan=0, int npol=0, int nbin=0)
{

}

void Pulsar::Archive::iq_xy()
{

}

void Pulsar::Archive::xy_iq()
{

}

void Pulsar::Archive::iv_rl()
{

}

void Pulsar::Archive::rl_iv()
{

}

























