#include "Integration.h"
#include "Profile.h"
#include "Stokes.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::get_Stokes
//
/*!  
  \retval S Stokes 4-vector for the given band and bin
  \param iband the frequency band
  \param ibin the phase bin

  \pre The Integration must contain full polarimetric information.  That is,
  get_state() must return either Poln::Stokes or Poln::Coherence.  An
  exception is thrown otherwise.
*/
void Pulsar::Integration::get_Stokes ( Stokes& S, int iband, int ibin ) const
{
  if (state == Poln::Stokes) {
    for (int ipol=0; ipol<4; ++ipol)
      S[ipol] = profiles[ipol][iband]->get_amps()[ibin];
    return;
  }

  else if (state == Poln::Coherence) {

    float PP   = profiles[0][iband]->get_amps()[ibin];
    float QQ   = profiles[1][iband]->get_amps()[ibin];
    float RePQ = profiles[2][iband]->get_amps()[ibin];
    float ImPQ = profiles[3][iband]->get_amps()[ibin];

    bool circular = profiles[0][iband]->get_state() == Poln::RR;

    if (circular) {
      S.i = PP + QQ;
      S.v = PP - QQ;
      S.q = 2.0 * RePQ;
      S.u = 2.0 * ImPQ;
    }
    else {
      S.i = PP + QQ;
      S.q = PP - QQ;
      S.u = 2.0 * RePQ;
      S.v = 2.0 * ImPQ;
    }
    return;
  }

  throw string ("Pulsar::Integration::get_Stokes invalid state");
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::get_Stokes vector
//
/*!  
  The vector of Stokes parameters may be calculated with either pulse
  phase or observing frequency as the abscissa.

  \retval S a vector of Stokes parameters as a function of the given dimension
  \param iother the index in the other dimension from which to draw S
  \param abscissa the dimension of the vector abscissa

  By default, "get_Stokes (S, iband);" will return the polarimetric
  profile from band, iband.  To get the polarimetric spectra from the
  middle of the pulse profile, for example, call "get_Stokes (S,
  get_nbin()/2, Axis::Frequency);"

*/
void Pulsar::Integration::get_Stokes (vector<Stokes>& S, int iother,
				      Dimension::Axis abscissa) const
{
  int ndim = 0;
  int ndim_other = 0;

  if (!(state == Poln::Stokes || state == Poln::Coherence))
    throw string ("Pulsar::Integration::get_Stokes invalid state");

  if (abscissa == Dimension::Frequency) {
    ndim = get_nband();
    ndim_other = get_nbin();
  }
  else if (abscissa == Dimension::Phase) {
    ndim = get_nbin();
    ndim_other = get_nband();
  }
  else
    throw string ("Pulsar::Integration::get_Stokes invalid abscissa");

  if (iother<0 || iother>=ndim_other)
    throw string ("Pulsar::Integration::get_Stokes invalid dimension");

  S.resize(ndim);

  int ibin=0, iband=0;

  if (abscissa == Dimension::Frequency)
    ibin = iother;    // all Stokes values come from the same bin
  else // dim == Dimension::Phase
    iband = iother;   // all Stokes values come from the same band

  for (int idim=0; idim<ndim; idim++) {

    if (abscissa == Dimension::Frequency)
      iband = idim;
    else
      ibin = idim;

    get_Stokes (S[idim], iband, ibin);

  }
}

