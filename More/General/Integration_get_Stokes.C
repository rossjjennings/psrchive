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
  get_state() must return either Signal::Stokes or Signal::Coherence.  An
  exception is thrown otherwise.
*/
void Pulsar::Integration::get_Stokes ( Stokes& S, unsigned ichan, unsigned ibin ) const
{
  if (get_state() == Signal::Stokes) {
    for (unsigned ipol=0; ipol<4; ++ipol)
      S[ipol] = profiles[ipol][ichan]->get_amps()[ibin];
    return;
  }

  else if (get_state() == Signal::Coherence) {

    float PP   = profiles[0][ichan]->get_amps()[ibin];
    float QQ   = profiles[1][ichan]->get_amps()[ibin];
    float RePQ = profiles[2][ichan]->get_amps()[ibin];
    float ImPQ = profiles[3][ichan]->get_amps()[ibin];

    if (get_basis() == Signal::Circular) {
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
void Pulsar::Integration::get_Stokes (vector<Stokes>& S, unsigned iother,
				      Signal::Dimension abscissa) const
{
  unsigned ndim = 0;
  unsigned ndim_other = 0;

  if (!(get_state()==Signal::Stokes || get_state()==Signal::Coherence))
    throw Error (InvalidPolnState, "Integration::get_Stokes");

  if (abscissa == Signal::Frequency) {
    ndim = get_nchan();
    ndim_other = get_nbin();
  }
  else if (abscissa == Signal::Phase) {
    ndim = get_nbin();
    ndim_other = get_nchan();
  }
  else
    throw Error (InvalidParam, "Integration::get_Stokes", "invalid abscissa");

  if (iother<0 || iother>=ndim_other)
    throw Error (InvalidParam, "Integration::get_Stokes", "invalid dimension");

  S.resize(ndim);

  unsigned ibin=0, ichan=0;

  if (abscissa == Signal::Frequency)
    ibin = iother;    // all Stokes values come from the same bin
  else // dim == Phase
    ichan = iother;   // all Stokes values come from the same chan

  for (unsigned idim=0; idim<ndim; idim++) {

    if (abscissa == Signal::Frequency)
      ichan = idim;
    else
      ibin = idim;

    get_Stokes (S[idim], ichan, ibin);

  }
}

