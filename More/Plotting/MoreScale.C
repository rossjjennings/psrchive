/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MoreScale.h"
#include "Pulsar/MoreProfiles.h"
#include "Pulsar/Archive.h"
#include "pairutil.h"

#include <iostream>
using namespace std;

Pulsar::MoreScale::MoreScale ()
{
  reverse = false;
}

void Pulsar::MoreScale::init (const Archive* data)
{
  // PlotScale::num_indeces
  num_indeces = data->get_Profile(0,0,0)->get<MoreProfiles>()->get_size();
}

void Pulsar::MoreScale::get_indeces (const Archive* data, 
				     unsigned& min, unsigned& max) const
{
  PlotScale::get_indeces (num_indeces, min, max);
}

std::string Pulsar::MoreScale::get_label () const
{
  return "Auxiliary";
}

Pulsar::MoreScale::Interface::Interface (MoreScale* instance)

{
  if (instance)
    set_instance (instance);

  import( PlotScale::Interface() );

  add( &PlotScale::get_index_range,
       &PlotScale::set_index_range,
       "range", "Index range to plot" );

  add( &MoreScale::get_reverse,
       &MoreScale::set_reverse,
       "reverse", "Reverse order" );  
}
