#include "Pulsar/CalibratorStokesInfo.h"
#include "Pulsar/CalibratorStokes.h"

//! Constructor
Pulsar::CalibratorStokesInfo::CalibratorStokesInfo (const CalibratorStokes* cs)
{
  calibrator_stokes = cs;
  together = false;
}
    
//! Return the number of parameter classes
unsigned Pulsar::CalibratorStokesInfo::get_nclass () const
{
  if (together)
    return 1;
  else
    return 3;
}
    
//! Return the name of the specified class
const char* Pulsar::CalibratorStokesInfo::get_name (unsigned iclass) const
{
  if (together)
    return "Calibrator Stokes";

  static char label [16] = "\\fiC\\dn\\u/C\\d0";
  char* replace = strchr (label, 'n');

  *replace = '1' + iclass;

  return label;
}


//! Return the number of parameters in the specified class
unsigned Pulsar::CalibratorStokesInfo::get_nparam (unsigned iclass) const
{
  if (together)
    return 3;
  else
    return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::CalibratorStokesInfo::get_param (unsigned ichan, unsigned iclass,
					 unsigned iparam) const
{
  unsigned index = 0;

  if (together)
    index = iparam;
  else
    index = iclass;

  return calibrator_stokes->get_stokes(ichan)[iclass];

}

