



#include "Pulsar/BandpassChannelWeightPlot.h"
#include <Pulsar/Archive.h>



using namespace Pulsar;
using namespace std;



BandpassChannelWeightPlot::BandpassChannelWeightPlot()
{
  manage( "band", &band );
  manage( "weights", &weights );
  
  band.get_frame()->set_viewport( 0, 1, .35, 1 );
  weights.get_frame()->set_viewport( 0, 1, 0, .3 );
  
  weights.get_frame()->get_label_above()->set_centre( "" );
  weights.get_frame()->get_label_below()->set_left( "" );
  weights.get_frame()->hide_axes();
}



BandpassChannelWeightPlot::~BandpassChannelWeightPlot()
{
}



TextInterface::Parser *BandpassChannelWeightPlot::get_interface()
{
  return new Interface( this );
}



void BandpassChannelWeightPlot::prepare( const Archive *data )
{
}



