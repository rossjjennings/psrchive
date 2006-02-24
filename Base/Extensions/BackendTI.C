#include "Pulsar/BackendTI.h"

Pulsar::BackendTI::BackendTI ()
{
  init ();
}

void Pulsar::BackendTI::init ()
{
  {
    Generator<Signal::Hand> gen;
    add( gen.described ("hand", "Hand of backend basis",
			&Backend::get_hand,
			&Backend::set_hand) );
  }
  {
    Generator<Signal::Argument> gen;
    add( gen.described ("arg", "Complex phase convention",
			&Backend::get_argument,
			&Backend::set_argument) );
  }
}

