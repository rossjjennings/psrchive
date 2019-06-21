/***************************************************************************
 *
 *   Copyright (C) 2005-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineWindow.h"

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"

#include "Pulsar/Config.h"
#include "Pulsar/Profile.h"

using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::BaselineWindow::BaselineWindow ()
{
  range_specified = false;

  find_max = false;
  find_mean = false;

  mean = 0;

  /* The default smoothing algorithm is not set in the constructor
     because the Smooth constructor uses a Pulsar::Config attribute
     that may not be initialized at the time of construction of this
     instance.
  */
}

Pulsar::BaselineWindow::BaselineWindow (const BaselineWindow& copy)
{
  find_max = copy.find_max;
  find_mean = copy.find_mean;
  mean = copy.mean;

  if (copy.smooth)
    smooth = copy.smooth->clone();

  range_specified = copy.range_specified;
  search_range = copy.search_range;
}

Pulsar::BaselineWindow* Pulsar::BaselineWindow::clone () const
{
  return new BaselineWindow (*this);
}

//! Set the smoothing function
void Pulsar::BaselineWindow::set_smooth (Smooth* function)
{
  smooth = function;
}

static Pulsar::Option<std::string> default_smooth
(
 "BaselineWindow::smooth", "mean",

 "Baseline window smoothing function",

 "The BaselineWindow algorithm defines the off-pulse baseline by the\n"
 "the minimum phase of a smoothed version of the profile.  The smoothing \n"
 "function can be either 'mean' or 'median'."
);

//! Get the smoothing function
Pulsar::Smooth* Pulsar::BaselineWindow::get_smooth () const
{
  if (!smooth)
  {
    BaselineWindow* non_const = const_cast<BaselineWindow*>(this);
    non_const->smooth = Smooth::factory (default_smooth);
  }

  return smooth;
}

//! Retrieve the PhaseWeight
void Pulsar::BaselineWindow::calculate (PhaseWeight* weight)
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::BaselineWindow::calculate",
		 "Profile not set");

  unsigned nbin = profile->get_nbin();

  double centre = find_phase (nbin, profile->get_amps());
  double width = get_smooth()->get_turns();

  if (width > centre)
    centre += 1.0;

  found_range.set_range( centre - 0.5*width, centre + 0.5*width );
  found_range.set_unit( Phase::Turns );
  found_range.set_nbin( nbin );

  std::pair<unsigned,unsigned> found_bins = found_range.get_bins();

#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::calculate centre=" << centre
       << " bins=" << bins << " ibin1=" << ibin1 << " ibin2=" << ibin2 << endl;
#endif

  weight->resize( nbin );
  weight->set_all( 0.0 );

  for (unsigned ibin=found_bins.first; ibin<found_bins.second; ibin++)
    (*weight)[ibin%nbin] = 1.0;
}

//! Set to find the minimum mean
void Pulsar::BaselineWindow::set_find_minimum (bool f)
{
  find_max = !f;
}
 
bool Pulsar::BaselineWindow::get_find_minimum () const
{
  return !find_max;
}

 
//! Set to find the maximum mean
void Pulsar::BaselineWindow::set_find_maximum (bool f)
{
  find_max = f;
}

bool Pulsar::BaselineWindow::get_find_maximum () const
{
  return find_max;
}

void Pulsar::BaselineWindow::set_find_mean (float _mean)
{
  find_mean = true;
  mean = _mean;
}

float Pulsar::BaselineWindow::get_find_mean () const
{
  return find_mean;
}

//! Set the start and end bins of the search
void Pulsar::BaselineWindow::set_range (int start, int end)
{
  cerr << "Pulsar::BaselineWindow::set_range"
    " start=" << start << " end=" << endl;
  search_range.set_unit (Phase::Bins);
  search_range.set_range (start, end);
  range_specified = true;
}

//! Set the range to be searched
void Pulsar::BaselineWindow::set_search_range (const Phase::Range& range)
{
  search_range = range;
  range_specified = true;
}

//! Get the range to be search
Phase::Range Pulsar::BaselineWindow::get_search_range () const
{
  return search_range;
}

//! Get the range found during execution
Phase::Range Pulsar::BaselineWindow::get_found_range () const
{
  if (range_specified)
    return found_range.as (search_range.get_unit());
  return found_range;
}

float Pulsar::BaselineWindow::find_phase (const std::vector<float>& amps)
{
  return find_phase (amps.size(), &amps[0]);
}

//! Return the phase at which minimum or maximum mean is found
float Pulsar::BaselineWindow::find_phase (unsigned nbin, const float* amps)
try {

  Profile temp (nbin);
  temp.set_amps (amps);

#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase" << endl;
#endif

  get_smooth()->transform( &temp );

  unsigned start = 0;
  unsigned stop = nbin;
  
  if (range_specified)
  {
    search_range.set_nbin (nbin);
    std::pair<int,int> bins = search_range.get_bins();
    nbinify (bins.first, bins.second, nbin);
    start = bins.first;
    stop = bins.second;
  }
  
#ifdef _DEBUG
  cerr << "Pulsar::BaselineWindow::find_phase init"
    " start=" << start << " stop=" << stop << endl;
#endif

  bool first = true;
  float found_val = 0;
  unsigned found_bin = 0;

  for (unsigned ibin=start; ibin < stop; ibin++)
  {
    if (!consider(ibin))
      continue;

    float value = temp.get_amps()[ibin%nbin];

    if ( find_mean )
    {
      double diff = fabs( value - mean );
      if ( first || diff < found_val )
      {
	found_val = diff;
	found_bin = ibin;
      }
      continue;
    }


    if ( first ||
	 (find_max && value>found_val) || (!find_max && value<found_val) )
    {
      found_val = value;
      found_bin = ibin;
    }

    first = false;

  }

  float phase = float(found_bin%nbin) / float(nbin);

#ifdef _DEBUG
  cerr << "phase=" << phase << endl;
#endif

  return phase;
}
catch (Error& error)
{
  throw error += "Pulsar::BaselineWindow::find_phase";
}



#include "interface_stream.h"

namespace Pulsar
{
  std::ostream& operator<< (std::ostream& ostr, Smooth* e)
  {
    return interface_insertion (ostr, e);
  }

  std::istream& operator>> (std::istream& istr, Smooth* &e)
  {
    return interface_extraction (istr, e);
  }
}

class Pulsar::BaselineWindow::Interface 
  : public TextInterface::To<BaselineWindow>
{
public:
  Interface (BaselineWindow* instance)
  {
    if (instance)
      set_instance (instance);

    add( &BaselineWindow::get_smooth,
	 &BaselineWindow::set_smooth,
	 &Smooth::get_interface,
	 "smooth", "smoothing algorithm" );

    add( &BaselineWindow::get_find_minimum,
         &BaselineWindow::set_find_minimum,
         "find_min", "find the minimum" );

    add( &BaselineWindow::get_find_mean,
         &BaselineWindow::set_find_mean,
         "find_mean", "find the specified mean" );

    add( &BaselineWindow::get_search_range,
	 &BaselineWindow::set_search_range,
	 "search", "range to be searched" );

    add( &BaselineWindow::get_found_range,
	 "found", "found range" );
  }

  std::string get_interface_name () const { return "minimum"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::BaselineWindow::get_interface ()
{
  return new Interface (this);
}
