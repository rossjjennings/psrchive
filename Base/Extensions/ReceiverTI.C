#include "Pulsar/ReceiverTI.h"

Pulsar::ReceiverTI::ReceiverTI ()
{
  init ();
}

void Pulsar::ReceiverTI::init ()
{
  {
    Generator<Angle> gen;
    
    add( gen.described ("ta", "Tracking angle of feed",
			&Receiver::get_tracking_angle,
			&Receiver::set_tracking_angle) );
  }

  {
    Generator<bool> gen;

    add( gen.described ("fac", "Feed angle corrected",
			&Receiver::get_feed_corrected,
			&Receiver::set_feed_corrected) );

    add( gen.described ("vac", "Vertical (parallactic) angle corrected",
			&Receiver::get_platform_corrected,
			&Receiver::set_platform_corrected) );

  }

  {
    Generator<Signal::Basis> gen;
    add( gen.described ("basis", "Basis of receptors",
			&Receiver::get_basis,
			&Receiver::set_basis) );
  }

  {
    Generator<Signal::Hand> gen;
    add( gen.described ("hand", "Hand of receptor basis",
			&Receiver::get_hand,
			&Receiver::set_hand) );
  }

  {
    Generator<Angle> gen;

    add( gen.described ("rph", "Reference source phase",
			&Receiver::get_reference_source_phase,
			&Receiver::set_reference_source_phase) );

    add( gen.described ("oa", "Orientation angle of receptors in feed",
			&Receiver::get_orientation,
			&Receiver::set_orientation) );

    add( gen.described ("ra", "Orientation angle of reference field vector",
			&Receiver::get_field_orientation,
			&Receiver::set_field_orientation) );

    add( gen.described ("xo", "Offset of feed X-axis wrt platform zero",
			&Receiver::get_X_offset,
			&Receiver::set_X_offset) );

    add( gen.described ("yo", "Offset of feed Y-axis wrt nominal value",
			&Receiver::get_Y_offset,
			&Receiver::set_Y_offset) );

    add( gen.described ("co", "Offset of calibrator wrt nominal value",
			&Receiver::get_calibrator_offset,
			&Receiver::set_calibrator_offset) );
  }

}

