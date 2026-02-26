/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotScale.h"
#include "Pulsar/PlotLabel.h"
#include "pairutil.h"

//#define _DEBUG 1
#include "debug.h"

#include <math.h>

using namespace std;

std::pair<float,float> unset_range_norm (0.0, 1.0);
std::pair<float,float> unset_world (0.0, 0.0);
std::pair<unsigned,unsigned> unset_indeces (0,0);

Pulsar::PlotScale::PlotScale () :
  world (unset_world),
  range_norm (unset_range_norm),
  world_external (unset_world),
  index_range (unset_indeces)
{
}

void Pulsar::PlotScale::init (const Archive*)
{
}

//! Expand as necessary to include another PlotScale
void Pulsar::PlotScale::include (PlotScale* other)
{
  DEBUG("Pulsar::PlotScale::include this=" << this << " that=" << other);
	     
  if (frozen)
  {
    DEBUG("Pulsar::PlotScale::include frozen - ignoring");
    return;
  }

  if (!other->minmaxvalset)
  {
    DEBUG("Pulsar::PlotScale::include other not set - ignoring");
    return;
  }

  DEBUG("Pulsar::PlotScale::include calling update_minmax");
  update_minmax (other->minval, other->maxval);
}

void Pulsar::PlotScale::copy (PlotScale* other)
{
  if (frozen)
    return;

  *this = *other;
}

//! Set the minimum and maximum value in the data
void Pulsar::PlotScale::set_minmax (float min, float max)
{
  if (frozen)
  {
    DEBUG("Pulsar::PlotScale::set_minmax frozen - ignoring");
    return;
  }

  DEBUG("Pulsar::PlotScale::set_minmax min=" << min << " max=" << max);

  minval = min;
  maxval = max;
  minmaxvalset = true;
}

void Pulsar::PlotScale::update_minmax (float min, float max)
{
  if (frozen)
  {
    DEBUG("Pulsar::PlotScale::update_minmax frozen - ignoring");
    return;
  }

  DEBUG("Pulsar::PlotScale::update_minmax incoming min=" << min << " max=" << max);

  if (!minmaxvalset)
  {
    DEBUG("Pulsar::PlotScale::update_minmax currently unset - setting to incoming");
    minval = min;
    maxval = max;
  }
  else
  {
    DEBUG("Pulsar::PlotScale::update_minmax current min=" << minval << " max=" << maxval);
    minval = std::min (min, minval);
    maxval = std::max (max, maxval);
    DEBUG("Pulsar::PlotScale::update_minmax new min=" << minval << " max=" << maxval);
  }
 
  minmaxvalset = true;
}

//! Get the minimum and maximum value in the data
void Pulsar::PlotScale::get_minmax (float& min, float& max) const
{
  min = minval;
  max = maxval;
}

std::pair<float,float> Pulsar::PlotScale::get_minmax () const
{
  return std::pair<float,float> (minval, maxval);
}

void Pulsar::PlotScale::set_range_norm (const std::pair<float,float>& f)
{
  range_norm = f;
}

void Pulsar::PlotScale::set_world (const std::pair<float,float>& f)
{
  world = f;
}

void Pulsar::PlotScale::set_world_external (const std::pair<float,float>& f)
{
  world_external = f;
}

std::pair<float,float> Pulsar::PlotScale::get_world_external () const
{
  return world_external;
}

//! Set the index range to be plotted
void Pulsar::PlotScale::set_index_range (const std::pair<unsigned,unsigned>& i)
{
  index_range = i;
}

//! Get the index range to be plotted
std::pair<unsigned,unsigned> Pulsar::PlotScale::get_index_range () const
{
  return index_range;
}

std::pair<float,float>
Pulsar::PlotScale::viewport_to_world (const std::pair<float,float>& viewport)
{
  std::pair<float,float> result;
  result.first = viewport_to_world( viewport.first );
  result.second = viewport_to_world( viewport.second );
  return result;  
}

double Pulsar::PlotScale::viewport_to_world (double viewport)
{
  float min = 0, max = 0;
  get_range (min, max);

  return min + (max-min) * viewport;
}

void Pulsar::PlotScale::get_range (float& min, float& max) const
{
  if (world != unset_world)
  {
    min = world.first;
    max = world.second;
    return;
  }

  get_minmax (min, max);

  if (num_indeces > 1 && index_range != unset_indeces)
  {
    double span = max - min;
    max = min + span * (double(index_range.second)/double(num_indeces));
    min = min + span * (double(index_range.first)/double(num_indeces));

    DEBUG("Pulsar::PlotScale::get_range num_indeces=" << num_indeces
         << " span=" << span << " max=" << max << " min=" << min 
         << " imin=" << index_range.first << " imax=" << index_range.second);

    return;
  }
  
  if (range_norm != unset_range_norm)
    stretch (range_norm, min, max);

  if (buf_norm > 0)
  {
    float space = (max - min) * buf_norm;
    min -= space;
    max += space;
  }
}

//! Return min and max scaled according to zoom attributes
void Pulsar::PlotScale::get_range_external (float& min, float& max) const
{
  if (world_external == unset_world)
    get_range (min, max);
  else
  {
    min = world_external.first;
    max = world_external.second;
  }
}

void Pulsar::PlotScale::get_indeces (unsigned n,
				     unsigned& imin, unsigned& imax,
				     bool cyclic) const
{
  if (num_indeces == n && index_range != unset_indeces)
  {
    imin = index_range.first;
    imax = index_range.second;

    if (!cyclic)
    {
      if (imin > n)
	imin = n;

      if (imax > n)
	imax = n;

      if (imin > imax)
      std::swap (imin, imax);
    }
    return;
  }

  float min = 0.0;
  float max = 1.0;

  get_axis_indeces (n, min, max, cyclic);

  imin = unsigned( min );
  imax = unsigned( ceil(max) );

  if (!cyclic)
  {
    if (imin > n)
      imin = n;

    if (imax > n)
      imax = n;

    if (imin > imax)
      std::swap (imin, imax);
  }
}

void Pulsar::PlotScale::get_axis_indeces (unsigned n,
                                          float& imin, float& imax,
                                          bool cyclic) const 
{
  if (num_indeces == n && index_range != unset_indeces)
  {
    imin = index_range.first;
    imax = index_range.second;
    return;
  }

  float min = 0.0;
  float max = 1.0;

  if (world != unset_world)
  {
    float wmin = 0.0;
    float wmax = 1.0;

    get_minmax (wmin, wmax);
    float length = wmax - wmin;

    std::pair<float,float> zoom;

    zoom.first = (world.first - wmin) / length;
    zoom.second = (world.second - wmin) / length;

    stretch (zoom, min, max);
  }

  stretch (range_norm, min, max);

  if (cyclic)
  {
    double diff = max - min;

    // fold onto 0->1
    min -= floor (min);
    max -= floor (max);

    // and ensure that max > min
    if (max <= min)
      max += 1.0;

    // and ensure that the difference is preserved
    if (diff > 0 && (max - min) < diff)
      max += 1.0;
  }
  else
  {
    if (min < 0)
      min = 0;
    if (max < 0)
      max = 0;
  }

  imin = min * n;
  imax = max * n;
}

