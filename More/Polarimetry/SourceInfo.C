/***************************************************************************
 *
 *   Copyright (C) 2004 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SourceInfo.h"
#include "Pulsar/SystemCalibrator.h"

#include <assert.h>

//! Constructor
Pulsar::SourceInfo::SourceInfo 
(const std::vector<Calibration::SourceEstimate>& estimate)
  : source (estimate)
{
  together = false;
  title = "Stokes Parameters";
}

void Pulsar::SourceInfo::set_together (bool flag)
{
  together = flag;
}

void Pulsar::SourceInfo::set_label (const std::string& lab)
{
  label = lab;
}

void Pulsar::SourceInfo::set_title (const std::string& t)
{
  title = t;
}

std::string Pulsar::SourceInfo::get_title () const
{
  return title;
}

unsigned Pulsar::SourceInfo::get_nchan () const
{
  return source.size();
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
std::string Pulsar::SourceInfo::get_name (unsigned iclass) const
{
  std::string label = "\\fiS'\\b\\d\\frk";

  if (!together)
  {
    std::string::size_type index = label.find('k');
    assert (index != std::string::npos);
    label[index] = '0' + iclass;
  }

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
  if (!source.size())
    return 0.0;

  if (!source[ichan].valid || !source[ichan].source)
    return 0.0;

  unsigned index = 0;

  if (together)
    index = iparam;
  else
    index = iclass;

  return source[ichan].source->get_Estimate(index);
}
