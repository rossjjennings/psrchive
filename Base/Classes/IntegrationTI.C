#include "Pulsar/IntegrationTI.h"

Pulsar::IntegrationTI::IntegrationTI ()
{
  init ();
}

void Pulsar::IntegrationTI::init ()
{
  {
    Generator<unsigned> gen;

    add( gen.described ("nbin", "Number of frequency channels",
                        &Integration::get_nchan) );

    add( gen.described ("nbin", "Number of polarizations",
                        &Integration::get_npol) );

    add( gen.described ("nbin", "Number of pulse phase bins",
			&Integration::get_nbin) );
  }

  {
    Generator<MJD> gen;
    add( gen.described ("mjd", "Epoch (MJD)",
			&Integration::get_epoch,
			&Integration::set_epoch) );
  }

  {
    Generator<double> gen;
    add( gen.described ("duration", "Duration of integration (seconds)",
                        &Integration::get_duration,
                        &Integration::set_duration) );

    add( gen.described ("period", "Period at which data were folded (seconds)",
                        &Integration::get_folding_period,
                        &Integration::set_folding_period) );
  }

}

