

#include "Pulsar/DynamicSNSpectrum.h"
#include <Pulsar/StandardSNR.h>
#include <Pulsar/FrequencyScale.h>
#include <float.h>
#include <Reference.h>
#include <cpgplot.h>




using Pulsar::DynamicSNSpectrum;



/**
 * Constructor
 *
 * DOES     - Initialize ranges, and set the y scale to be a frequency scale
 * RECEIVES - Nothing
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

DynamicSNSpectrum::DynamicSNSpectrum()
{
  isubint = -1;
  pol = -1;

  srange.first = -1;
  srange.second = -1;

  get_frame()->set_y_scale( new FrequencyScale );
}




/**
 * prepare
 *
 * DOES     - Nothing
 * RECEIVES - The Archive
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

void DynamicSNSpectrum::prepare( const Archive *data )
{}



/**
 * draw
 *
 * DOES     - Take the total, use that total profile as the standard to compare against, plot as an image
 * RECEIVES - The Archive
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Make subint ranges work
 **/

void DynamicSNSpectrum::draw( const Archive *data )
{
  // Take the total

  StandardSNR standard_snr;

  Reference::To<Archive> data_std = data->total();
  standard_snr.set_standard( data_std->get_Profile(0,0,0) );

  // Get the x and y scales to determine if we are zooming in x or y

  PlotScale *xs = get_frame()->get_x_scale();
  PlotScale *ys = get_frame()->get_y_scale();

  float xmin, xmax, ymin, ymax;

  xs->get_range( xmin, xmax );
  ys->get_range( ymin, ymax );

  if( isubint > -1 )
  {
    srange.first = srange.second = isubint;
  }
  else if( srange.first == -1 )
  {
    srange.first = 0;
    srange.second = data->get_nsubint() - 1;
  }

  float centre_freq = data->get_centre_frequency();
  float bw = data->get_bandwidth();

  // calculate the first and last channels

  int total_num_chans = data->get_nchan();

  if( bw < 0 )
  {
    bw = 0 - bw;
    float tmp = ymin;
    ymin = ymax;
    ymax = tmp;
  }

  float min_freq = centre_freq - bw / 2;
  float max_freq = centre_freq + bw / 2;
  float freq_range = max_freq - min_freq;

  int fchan = ( ymin - min_freq ) / freq_range * total_num_chans;
  int lchan = ( ymax - min_freq ) / freq_range * total_num_chans;

  if( lchan == total_num_chans )
    lchan -= 1;

  int num_chans_displayed = lchan - fchan + 1;

  if( pol < 0 )
    pol = 0;

  int num_subs = srange.second - srange.first + 1;

  float plot_array[ num_subs * data->get_nchan() ];

  float min_snr = FLT_MAX;
  float max_snr = -FLT_MAX;

  unsigned int plot_index = 0;

  for( int next_chan = fchan; next_chan <= lchan; next_chan ++ )
  {
    for( int next_sub = srange.first; next_sub <= srange.second; next_sub ++ )
    {
      Reference::To<const Profile> next_profile = data->get_Profile ( next_sub, pol, next_chan);

      float next_snr = standard_snr.get_snr( next_profile );

      plot_array[plot_index++] = next_snr;

      if( next_snr < min_snr )
        min_snr = next_snr;
      if( next_snr > max_snr )
        max_snr = next_snr;
    }
  }

  float dy = freq_range / total_num_chans;

  cpgswin( srange.first, srange.second+1, fchan, lchan+1 );

  get_frame()->get_x_scale()->set_minmax( srange.first, srange.second + 1 );

  float transform[6] = { srange.first -.5, 1, 0.0, fchan, 0.0, 1 };

  cpgimag( plot_array, num_subs, num_chans_displayed, 1, num_subs, 1, num_chans_displayed , min_snr, max_snr, transform );

}



/**
 * get_ylabel
 *
 * DOES     - Returns y label
 * RECEIVES - The Archive
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

std::string DynamicSNSpectrum::get_ylabel (const Archive*)
{
  return "Frequency (Mhz)";
}



/**
 * get_xlabel
 *
 * DOES     - Returns x label
 * RECEIVES - The Archive
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

std::string DynamicSNSpectrum::get_xlabel (const Archive*)
{
  return "Sub Integration";
}




