#include "Pulsar/StokesSphericalTI.h"
#include "Pulsar/PhasePlotTI.h"
#include "Pulsar/StokesPlotTI.h"
#include "Pulsar/AnglePlotTI.h"

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

  import ( "flux", StokesPlotTI(), &StokesSpherical::get_flux );
  import ( "pa",  AnglePlotTI(), &StokesSpherical::get_orientation );
  import ( "ell", AnglePlotTI(), &StokesSpherical::get_ellipticity );
}
