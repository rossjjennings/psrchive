#include "Pulsar/PowerSpectraTI.h"

Pulsar::PowerSpectraTI::PowerSpectraTI (PowerSpectra* instance)
{
  if (instance)
    set_instance (instance);

  add( &PowerSpectra::get_subint,
       &PowerSpectra::set_subint,
       "subint", "Sub-integration to plot" );

  add( &PowerSpectra::get_chan,
       &PowerSpectra::set_chan,
       "chan", "Frequency channel to plot" );

  add( &PowerSpectra::get_pol,
       &PowerSpectra::set_pol,
       "pol", "Polarization to plot" );
}
