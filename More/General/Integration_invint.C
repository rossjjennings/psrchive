#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
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
  upon completion, npol == 1 and state == Signal::Invariant.

  If invint_square is true, this function calculates
  \f$\det\rho=I^2-Q^2-U^2-V^2\f$, otherwise \f$\sqrt{\det\rho}\f$ is
  calcuated.

  \pre The profile baselines must have been removed (unchecked).

  \exception string thrown if Stokes 4-vector cannot be formed
*/
void Pulsar::Integration::invint ()
{
  // space to calculate the result
  vector<float> invariant (get_nbin());
  // Stokes 4-vector
  Stokes<float> stokes;

  for (unsigned ichan=0; ichan<get_nchan(); ++ichan) {

    for (unsigned ibin=0; ibin<get_nbin(); ++ibin) {
      // get the Stokes 4-vector
      stokes = get_Stokes (ichan, ibin);
      // calculate \det\rho
      invariant[ibin] = det (stokes);
    }

    // prepare to reset Stokes I to the Invariant Interval
    Profile* Sinv = get_Profile (0, ichan);

    // set the values
    Sinv->set_amps (invariant.begin());
    // remove the baseline
    *(Sinv) -= Sinv->mean( Sinv->find_min_phase() );

    if (invint_square)
      Sinv->set_state (Signal::DetRho);
    else {
      Sinv->set_state (Signal::Inv);
      Sinv->square_root();
    }
    
  } // for each channel

  resize (1);
  set_state (Signal::Invariant);
}
