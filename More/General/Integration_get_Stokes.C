#include "Integration.h"
#include "Profile.h"
#include "Error.h"

#include "Stokes.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::get_Stokes
//
/*!  
  \retval S Stokes 4-vector for the given chan and bin
  \param ichan the frequency chan
  \param ibin the phase bin

  \pre The Integration must contain full polarimetric information.  That is,
  get_poln_state() must return either Poln::Stokes or Poln::Coherence.  An
  exception is thrown otherwise.
*/
void Pulsar::Integration::get_Stokes ( Stokes& S, int ichan, int ibin ) const
{
  if (get_poln_state() == Poln::Stokes) {
    for (int ipol=0; ipol<4; ++ipol)
      S[ipol] = profiles[ipol][ichan]->get_amps()[ibin];
    return;
  }

  else if (get_poln_state() == Poln::Coherence) {

    float PP   = profiles[0][ichan]->get_amps()[ibin];
    float QQ   = profiles[1][ichan]->get_amps()[ibin];
    float RePQ = profiles[2][ichan]->get_amps()[ibin];
    float ImPQ = profiles[3][ichan]->get_amps()[ibin];

    if (get_feed_type() == Feed::Circular) {
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

  throw Error (InvalidPolnState, "Integration::get_Stokes");
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

  By default, "get_Stokes (S, ichan);" will return the polarimetric
  profile from chan, ichan.  To get the polarimetric spectra from the
  middle of the pulse profile, for example, call "get_Stokes (S,
  get_nbin()/2, Axis::Frequency);"

*/
void Pulsar::Integration::get_Stokes (vector<Stokes>& S, int iother,
				      Dimension::Axis abscissa) const
{
  int ndim = 0;
  int ndim_other = 0;

  if (!(get_poln_state()==Poln::Stokes || get_poln_state()==Poln::Coherence))
    throw Error (InvalidPolnState, "Integration::get_Stokes");

  if (abscissa == Dimension::Frequency) {
    ndim = get_nchan();
    ndim_other = get_nbin();
  }
  else if (abscissa == Dimension::Phase) {
    ndim = get_nbin();
    ndim_other = get_nchan();
  }
  else
    throw Error (InvalidParam, "Integration::get_Stokes", "invalid abscissa");

  if (iother<0 || iother>=ndim_other)
    throw Error (InvalidParam, "Integration::get_Stokes", "invalid dimension");

  S.resize(ndim);

  int ibin=0, ichan=0;

  if (abscissa == Dimension::Frequency)
    ibin = iother;    // all Stokes values come from the same bin
  else // dim == Dimension::Phase
    ichan = iother;   // all Stokes values come from the same chan

  for (int idim=0; idim<ndim; idim++) {

    if (abscissa == Dimension::Frequency)
      ichan = idim;
    else
      ibin = idim;

    get_Stokes (S[idim], ichan, ibin);

  }
}

