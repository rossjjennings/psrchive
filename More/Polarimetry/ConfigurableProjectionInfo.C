/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConfigurableProjectionInfo.h"
#include "debug.h"

#include <set>

using namespace std;

//! Constructor
Pulsar::ConfigurableProjection::Info::Info (const ConfigurableProjection* cal)
{
  if (!cal)
    return;

  projection = cal;
  
  unsigned nchan = cal->get_nchan ();

  set<unsigned> indeces;

  unsigned nparam = 0;

  DEBUG("ConfigurableProjection::Info nchan=" << nchan);

  const MEAL::Complex2* valid = 0;

  // find the indeces of all parameters with estimates
  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    if ( ! cal->get_transformation_valid (ichan) )
      continue;

    const ConfigurableProjection::Transformation* transformation = cal->get_transformation (ichan);
    const MEAL::Complex2* xform = transformation->get_transformation ();
    nparam = xform->get_nparam();

    if (!valid)
      valid = xform;

    for (unsigned iparam = 0; iparam < nparam; iparam++)
    {
      if (xform->get_variance(iparam) != 0.0)
        indeces.insert (iparam);
    }
  }

  parameters.insert (parameters.begin(), indeces.begin(), indeces.end());
  sort (parameters.begin(), parameters.end());

  together = 2;

  DEBUG("Pulsar::ConfigurableProjection::Info " << parameters.size() << "/" << nparam << " params have estimates");

  unsigned nclass = get_nclass();
  names.resize (nclass);

  nparam = parameters.size();
  unsigned iparam = 0;

  for (unsigned iclass = 0; iclass < nclass; iclass++)
  {
    string name;
    for (unsigned i=0; i<together && iparam<nparam; i++)
    {
      if (i > 0)
        name += " ";

      name += valid->get_param_name( parameters[iparam] );
      iparam ++;
    }

    names[iclass] = name;
  }
}

unsigned Pulsar::ConfigurableProjection::Info::get_nchan () const
{
  return projection->get_nchan();
}

/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::ConfigurableProjection::Info::get_nclass () const
{
  unsigned nclass = parameters.size() / together;
  if (parameters.size() % together)
    nclass ++;

  return nclass;
}

/*! The name of each parameter is unknown */
string Pulsar::ConfigurableProjection::Info::get_title () const
{
  return "Configurable Projection Parameters";
}

/*! The name of each parameter is unknown */
string Pulsar::ConfigurableProjection::Info::get_label (unsigned iclass) const
{
  return names[iclass];
}

/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::ConfigurableProjection::Info::get_nparam (unsigned iclass) const
{
  unsigned nclass = parameters.size() / together;
  unsigned remainder = parameters.size() % together;

  if (iclass >= nclass)
    return remainder;
  else
    return together;
}
      
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::ConfigurableProjection::Info::get_param (unsigned ichan, 
							 unsigned iclass,
							 unsigned iparam) const
{
  if (! projection->get_transformation_valid(ichan) )
  {
    DEBUG("Pulsar::ConfigurableProjection::Info::get_param invalid ichan=" << ichan);
    return 0;
  }

  unsigned offset = 0;
  for (unsigned jclass=1; jclass<=iclass; jclass++)
    offset += get_nparam (jclass-1);

  const ConfigurableProjection::Transformation* transformation = 0;
  transformation = projection->get_transformation (ichan);
  const MEAL::Complex2* xform = transformation->get_transformation ();

  return xform->get_Estimate( parameters[iparam+offset] );
}


//! Return the colour index
int Pulsar::ConfigurableProjection::Info::get_colour_index (unsigned iclass,
						            unsigned iparam) const
{
  unsigned colour_offset = 1;
  if (get_nparam (iclass) == 3)
    colour_offset = 2;

  return colour_offset + iparam;
}

//! Return the graph marker
int Pulsar::ConfigurableProjection::Info::get_graph_marker (unsigned iclass, 
						    unsigned iparam) const
{
  if (iparam == 0 || iparam > 3)
    return -1;

  iparam --;

  int nice_markers[3] = { 2, 5, 4 };

  return nice_markers[iparam];
}
