#include "Pulsar/ReceiverTI.h"

Pulsar::ReceiverTI::ReceiverTI ()
{
  init ();
}

void Pulsar::ReceiverTI::init ()
{
  {
    Generator<bool> gen;

    add( gen.described ("fac", "Feed angle corrected",
			&Receiver::get_feed_corrected,
			&Receiver::set_feed_corrected) );

    add( gen.described ("vac", "Vertical (parallactic) angle corrected",
			&Receiver::get_platform_corrected,
			&Receiver::set_platform_corrected) );
  }
}

