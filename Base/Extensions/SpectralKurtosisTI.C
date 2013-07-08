/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SpectralKurtosis.h"

using Pulsar::SpectralKurtosis;
using std::string;

SpectralKurtosis::Interface::Interface ( SpectralKurtosis *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &SpectralKurtosis::get_M,
       &SpectralKurtosis::set_M,
       "m", "Integration factor (samples)" );

  add( &SpectralKurtosis::get_excision_threshold,
       &SpectralKurtosis::set_excision_threshold,
       "nsig", "Excision threshold (stddevs)" );

  VGenerator<float> fgenerator;
  add_value(fgenerator( "all", string("Unfiltered mean spectral kurtosis"),
			&SpectralKurtosis::get_unfiltered_mean,
			&SpectralKurtosis::get_nchan ));

  add_value(fgenerator( "filt", string("Filtered mean spectral kurtosis"),
			&SpectralKurtosis::get_filtered_mean,
			&SpectralKurtosis::get_nchan ));


}




