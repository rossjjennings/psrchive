#include "Pulsar/StokesSphericalTI.h"
#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/StokesPlotterTI.h"
#include "Pulsar/AnglePlotterTI.h"

Pulsar::StokesSphericalTI::StokesSphericalTI (StokesSpherical* instance)
{
  if (instance)
    set_instance (instance);

  add( &StokesSpherical::get_subint,
       &StokesSpherical::set_subint,
       "subint", "Sub-integration to plot" );

  add( &StokesSpherical::get_chan,
       &StokesSpherical::set_chan,
       "chan", "Frequency channel to plot" );

  import ( PhasePlotTI() );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "flux", StokesPlotterTI(), &StokesSpherical::get_flux );
  import ( "pa",  AnglePlotterTI(), &StokesSpherical::get_orientation );
  import ( "ell", AnglePlotterTI(), &StokesSpherical::get_ellipticity );
}
