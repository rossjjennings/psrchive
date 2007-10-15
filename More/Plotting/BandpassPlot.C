



#include "Pulsar/BandpassPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <Pulsar/Integration.h>
#include <vector>
#include <cpgplot.h>
#include <float.h>
#include <utility>



using namespace Pulsar;
using namespace std;



BandpassPlot::BandpassPlot()
{}



BandpassPlot::~BandpassPlot()
{}



void BandpassPlot::prepare( const Archive *data )
{
  Reference::To<Archive> copy = data->clone();

  int num_chan = copy->get_nchan();

  means.resize( num_chan );

  float phase = 0.0;
  Pulsar::Profile* profile = 0;

  means_minmax.first = FLT_MAX;
  means_minmax.second = FLT_MIN;
  
  for (unsigned i = 0; i < copy->get_nchan(); i++)
  {
    profile = copy->get_Integration(0)->get_Profile(0, i);
    phase = profile->find_min_phase();
    means[i] = profile->mean(phase);

    if( means[i] < means_minmax.first )
      means_minmax.first = means[i];
    if( means[i] > means_minmax.second )
      means_minmax.second = means[i];
  }
  
  get_frame()->get_y_scale()->set_minmax( means_minmax.first, means_minmax.second );
  get_frame()->get_y_axis()->set_label( "Amplitude (Arbitrary)" );
}



void BandpassPlot::draw( const Archive *data )
{
  int num_chan = data->get_nchan();

  float xs[num_chan];
  float ys[num_chan];

  float freq = data->get_centre_frequency();
  float bw = data->get_bandwidth();
  float next_x = freq - bw / 2;
  float step_x = bw / num_chan;
  
  for( int i = 0; i < num_chan; i ++ )
  {
    xs[i] = next_x;
    ys[i] = means[i];
    next_x += step_x;
  }

  int old_ci;
  cpgqci( &old_ci );
  cpgsci( 2 );
  cpgline( num_chan, xs, ys );
  cpgsci( old_ci );
}



TextInterface::Parser *BandpassPlot::get_interface()
{
  return new Interface( this );
}


