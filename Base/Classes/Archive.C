
#include "Archive.h"

float Pulsar::Archive::default_baseline_window = 0.1;

void Pulsar::Archive::init ()
{
  baseline_window = default_baseline_window;
}
