#include "Integration.h"
#include "Profile.h"
#include "Stokes.h"

/////////////////////////////////////////////////////////////////////////////
//
/*! 
  When this flag is true, Pulsar::Integration::invint will calculate
  the square of the polarimetric invariant interval (equal to the deteminant
  of the coherency matrix). 
*/
bool Pulsar::Integration::invint_square = false;


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::invint
//
/*! 
  Forms the Stokes polarimetric invariant interval,
  \f$\det{P}=I^2-Q^2-U^2-V^2\f$, for every bin of each chan so that,
  upon completion, npol == 1 and state == Poln::Invariant.

  If invint_square is true, this function calculates
  \f$\det\rho=I^2-Q^2-U^2-V^2\f$, otherwise \f$\sqrt{\det\rho}\f$ is
  calcuated.

  \pre The profile baselines must have been removed (unchecked).

  \exception string thrown if Stokes 4-vector cannot be formed
*/
void Pulsar::Integration::invint ()
{
  // space to calculate the result
  vector<float> invariant (nbin);
  // Stokes 4-vector
  Stokes stokes;

  for (int ichan=0; ichan<nchan; ++ichan) {

    for (int ibin=0; ibin<nbin; ++ibin) {
      // get the Stokes 4-vector
      get_Stokes (stokes, ichan, ibin);
      // calculate \det\rho
      invariant[ibin] = stokes.invariant_Squared();
    }

    // prepare to reset Stokes I to the Invariant Interval
    Profile* Sinv = (*this)[Poln::Si][ichan];

    // set the values
    Sinv->set_amps (invariant.begin());
    // remove the baseline
    *(Sinv) += (-Sinv->mean (Sinv->find_min_phase()));

    if (invint_square)
      Sinv->set_state (Poln::DetRho);
    else {
      Sinv->set_state (Poln::SInv);
      Sinv->square_root();
    }
    
  } // for each channel

  resize (1, nchan, nbin);
  state = Poln::Invariant;
}
