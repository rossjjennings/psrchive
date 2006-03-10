#include "Pulsar/StokesCylindricalTI.h"
#include "Pulsar/ProfilePlotterTI.h"
#include "Pulsar/StokesPlotterTI.h"
#include "Pulsar/AnglePlotterTI.h"

Pulsar::StokesCylindricalTI::StokesCylindricalTI (StokesCylindrical* instance)
{
  if (instance)
    set_instance (instance);

  add( &StokesCylindrical::get_subint,
       &StokesCylindrical::set_subint,
       "subint", "Sub-integration to plot" );

  add( &StokesCylindrical::get_chan,
       &StokesCylindrical::set_chan,
       "chan", "Frequency channel to plot" );

  import ( ProfilePlotterTI() );

  // when this is set, import will filter out attributes with identical names
  import_filter = true;

  import ( "flux", StokesPlotterTI(), &StokesCylindrical::get_flux );
  import ( "pa", AnglePlotterTI(), &StokesCylindrical::get_orientation );
}
