#include "Physical.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar:: dispersion_delay
//
/*! 
  If the frequency is lower than the reference frequency, then the delay
  is positive.
  \return dispersion delay in seconds
  \param dm the dispersion measure in \f$ {\rm pc\,cm}^{-3} \f$
  \param ref_freq the frequency (in MHz) to which the delay is referenced
  \param freq the frequency (in MHz) of the delayed band
*/
double Pulsar::dispersion_delay (double dm, double ref_freq, double freq)
{

  if (ref_freq == 0)
    throw Error (InvalidParam, "Pulsar::dispersion_delay", 
                 "invalid reference frequency");

  if (freq == 0)
    throw Error (InvalidParam, "Pulsar::dispersion_delay", 
                 "invalid frequency");

  return (dm/2.41e-4) * ( 1.0/(freq*freq) - 1.0/(ref_freq*ref_freq) );

}
