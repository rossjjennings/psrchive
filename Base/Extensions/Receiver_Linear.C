#include "Pulsar/Receiver_Linear.h"

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
  if (offset == 0.0)  {
    set_right_handed( true );
    orientation_Y_offset = 0;
  }
  else if (offset == M_PI || offset == -M_PI) {
    set_right_handed( false );
    orientation_Y_offset.setDegrees( -90 );
  }
  else
    throw Error (InvalidParam, "Pulsar::Receiver_Linear::set_Y_offset",
		 "invalid offset = %lf deg", offset.getDegrees());
}

Angle Pulsar::Receiver_Linear::get_Y_offset () const
{
  if ( get_right_handed() )
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
  if (offset == 0.0 || offset == 0.5*M_PI || offset == -0.5*M_PI)
    reference_source_phase = 2.0 * offset;
  else
    throw Error (InvalidParam, "Pulsar::Receiver_Linear::set_calibrator_offset",
                 "invalid offset = %lf deg", offset.getDegrees());
}

Angle Pulsar::Receiver_Linear::get_calibrator_offset () const
{ 
  return 0.5*reference_source_phase;
}
