#include "Pulsar/Receiver_Linear.h"

//! Default constructor
Pulsar::Receiver_Linear::Receiver_Linear ()
{
  y_offset = calibrator_offset = false;
}

//! Get the basis of the feed receptors
Signal::Basis Pulsar::Receiver_Linear::get_basis () const
{
  return Signal::Linear;
}
    
//! Get the orientation of the basis about the line of sight
Angle Pulsar::Receiver_Linear::get_orientation () const
{
  Angle offset;
  if (y_offset)
    offset.setDegrees (-90);

  return x_offset + offset;
}
    
//! Return true if the basis is right-handed
bool Pulsar::Receiver_Linear::get_right_handed () const
{
  return !y_offset;
}
    
//! Get the phase of the reference source
Angle Pulsar::Receiver_Linear::get_reference_source_phase () const
{
  if (y_offset ^ calibrator_offset)
    return M_PI;
  else
    return 0;   
}
    
//! Copy the state from another
void Pulsar::Receiver_Linear::copy (const State* state)
{
  if (state->get_basis() != Signal::Linear)
    throw Error (InvalidParam, "Pulsar::Receiver_Linear::copy",
		 "input State::get_basis != Signal::Linear");

  set_calibrator_offset( 0.5 * state->get_reference_source_phase() );
  y_offset = !state->get_right_handed();

  Angle offset;

  if ( y_offset ) {
    offset.setDegrees (90);
    calibrator_offset = !calibrator_offset;
  }

  x_offset = state->get_orientation() + offset;
}

//! Get the offset of the feed X axis with respect to the platform zero
Angle Pulsar::Receiver_Linear::get_X_offset () const
{
  return x_offset;
}

//! Set the offset of the feed X axis with respect to the platform zero
void Pulsar::Receiver_Linear::set_X_offset (const Angle& offset)
{
  x_offset = offset;
}
    
/*! In the right-handed basis, the Y axis points in the direction of
  East.  However it is also common to encounter systems in which the Y
  axis is offset by 180 degrees, pointing West.  This is equivalent to
  switching the sign of the Y probe, which amounts to a 180 degree
  rotation of the Stokes vector about the Q axis.  It is also equivalent
  to the product of:
  <OL>
  <LI> switching the X and Y probes (a 180 degree rotation about the U axis,
  as done when the right_handed attribute is false)
  <LI> a -90 degree rotation about the line of sight (a 180 degree
  rotation about the V axis, as done when the orientation_Y_offset = -90
  </OL>
  which is how the transformation is represented by this class.

  \param offset either 0 or +/- 180 degrees

*/
void Pulsar::Receiver_Linear::set_Y_offset (const Angle& offset)
{ 
  if (offset == 0.0)
    y_offset = false;
  else if (offset.getDegrees() == 180 || offset.getDegrees() == -180)
    y_offset = true;
  else
    throw Error (InvalidParam,
		 "Pulsar::Receiver_Linear::set_Y_offset",
		 "invalid offset = %lf deg", offset.getDegrees());
}

Angle Pulsar::Receiver_Linear::get_Y_offset () const
{
  if (!y_offset)
    return 0.0;
  else
    return M_PI;
}

/*! In the linear basis, the noise diode must illuminate both receptors
    equally.  Therefore, there are only two valid orientations:

    \param offset either 0 or +/- 90 degrees
*/
void Pulsar::Receiver_Linear::set_calibrator_offset (const Angle& offset)
{
  if (offset == 0.0)
    calibrator_offset = false;
  else if (offset.getDegrees() == 90 || offset.getDegrees() == -90)
    calibrator_offset = true;
  else
    throw Error (InvalidParam,
		 "Pulsar::Receiver_Linear::set_calibrator_offset",
                 "invalid offset = %lf deg", offset.getDegrees());
}

Angle Pulsar::Receiver_Linear::get_calibrator_offset () const
{ 
  if (!calibrator_offset)
    return 0.0;
  else
    return 0.5*M_PI;
}
