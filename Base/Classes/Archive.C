
#include "Archive.h"

float Pulsar::Archive::default_baseline_window = 0.1;
bool  Pulsar::Archive::verbose = false;

void Pulsar::Archive::init ()
{
  baseline_window = default_baseline_window;
}

// Virtual method defns

Pulsar::Archive::Archive ()
{

}

Pulsar::Archive::~Archive ()
{

}

Pulsar::Archive* Pulsar::Archive::clone ()
{

}

void Pulsar::Archive::bscrunch (int nscrunch)
{

}

void Pulsar::Archive::fscrunch (int nscrunch=0, bool dedisp=true)
{

}

void Pulsar::Archive::tscrunch (int nscrunch=0, bool poly=true, bool wt=true)
{

}

void Pulsar::Archive::pscrunch()
{

}

void Pulsar::Archive::append (const Archive* a, bool check_ephemeris = true)
{

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

void Pulsar::Archive::set_baseline_window (float duty_cycle)
{

}

void Pulsar::Archive::snr_weight ()
{

}

void Pulsar::Archive::destroy ()
{

}

void Pulsar::Archive::resize (int nsubint, int nband=0, int npol=0, int nbin=0)
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

























