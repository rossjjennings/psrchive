#include "Pulsar/SourceInfo.h"
#include "Pulsar/ReceptionCalibrator.h"

//! Constructor
Pulsar::SourceInfo::SourceInfo (const ReceptionCalibrator* c, unsigned i)
{
  calibrator = c;
  source_index = i;

  if (source_index >= calibrator->pulsar.size())
    throw Error (InvalidRange, "Pulsar::SourceInfo",
		 "index=%d >= pulsar size=%d", i, calibrator->pulsar.size());

  together = false;
}
    
//! Return the number of parameter classes
unsigned Pulsar::SourceInfo::get_nclass () const
{
  if (together)
    return 1;
  else
    return 4;
}

//! Return the name of the specified class
const char* Pulsar::SourceInfo::get_name (unsigned iclass) const
{
  static char label [64] = "\\fiS'\\b\\d\\fnk";
  static char* replace = strchr (label, 'k');

  if (!together)
    *replace = '0' + iclass;

  return label;
}


//! Return the number of parameters in the specified class
unsigned Pulsar::SourceInfo::get_nparam (unsigned iclass) const
{
  if (together)
    return 4;
  else
    return 1;
}

//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::SourceInfo::get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const
{
  if (!calibrator->pulsar[source_index].valid[ichan])
    return 0.0;

  unsigned index = 0;

  if (together)
    index = iparam;
  else
    index = iclass;

  return calibrator->pulsar[source_index].source[ichan]->get_Estimate(index);
}
