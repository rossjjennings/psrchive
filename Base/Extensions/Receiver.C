#include "Pulsar/Receiver.h"

//! Default constructor
Pulsar::Receiver::Receiver () : Extension ("Receiver")
{
  mode = Feed;
  name = "unknown";

  calibrator_orientation.setDegrees( 45.0 );

  feed_offset_corrected = false;
  vertical_offset_corrected = false;

  atten_a = 0.0;
  atten_b = 0.0;
}

//! Copy constructor
Pulsar::Receiver::Receiver (const Receiver& ext)
  : Extension ("Receiver")
{
  operator = (ext);
}

//! Operator =
const Pulsar::Receiver&
Pulsar::Receiver::operator= (const Receiver& ext)
{
  mode = ext.mode;
  name = ext.name;

  X_offset = ext.X_offset;
  Y_offset = ext.Y_offset;

  calibrator_orientation = ext.calibrator_orientation;

  feed_offset_corrected = ext.feed_offset_corrected;
  vertical_offset_corrected = ext.vertical_offset_corrected;

  atten_a = ext.atten_a;
  atten_b = ext.atten_b;

  return *this;
}

//! Destructor
Pulsar::Receiver::~Receiver ()
{
}
