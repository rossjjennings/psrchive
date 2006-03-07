#include "Pulsar/ReceiverTI.h"

Pulsar::ReceiverTI::ReceiverTI ()
{
  add( &Receiver::get_tracking_angle,
       &Receiver::set_tracking_angle,
       "ta", "Tracking angle of feed" );

  add( &Receiver::get_feed_corrected,
       &Receiver::set_feed_corrected,
       "fac", "Feed angle corrected" );

  add( &Receiver::get_platform_corrected,
       &Receiver::set_platform_corrected,
       "vac", "Vertical (parallactic) angle corrected" );

  add( &Receiver::get_basis,
       &Receiver::set_basis,
       "basis", "Basis of receptors" );

  add( &Receiver::get_hand,
       &Receiver::set_hand,
       "hand", "Hand of receptor basis" );
 
  add( &Receiver::get_reference_source_phase,
       &Receiver::set_reference_source_phase,
       "rph", "Reference source phase" );

  add( &Receiver::get_orientation,
       &Receiver::set_orientation,
       "oa", "Orientation angle of receptors in feed" );

  add( &Receiver::get_field_orientation,
       &Receiver::set_field_orientation,
       "ra", "Orientation angle of reference field vector" );

  add( &Receiver::get_X_offset,
       &Receiver::set_X_offset,
       "xo", "Offset of feed X-axis wrt platform zero" );

  add( &Receiver::get_Y_offset,
       &Receiver::set_Y_offset,
       "yo", "Offset of feed Y-axis wrt nominal value" );

  add( &Receiver::get_calibrator_offset,
       &Receiver::set_calibrator_offset,
       "co", "Offset of calibrator wrt nominal value" );

}

