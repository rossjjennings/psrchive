#include "Pulsar/Receiver.h"
#include "MEAL/Rotation.h"
#include "Pauli.h"

//! Default constructor
Pulsar::Receiver::Receiver () : Extension ("Receiver")
{
  tracking_mode = Feed;
  name = "unknown";

  basis = Signal::Linear;
  right_handed = true;

  feed_corrected = false;
  platform_corrected = false;

  field_orientation = false;
  atten_a = 0.0;
  atten_b = 0.0;
}

//! Copy constructor
Pulsar::Receiver::Receiver (const Receiver& ext) : Extension ("Receiver")
{
  operator = (ext);
}

//! Operator =
const Pulsar::Receiver&
Pulsar::Receiver::operator= (const Receiver& ext)
{
  tracking_mode = ext.tracking_mode;
  tracking_angle = ext.tracking_angle;

  name = ext.name;
  basis = ext.basis;
  right_handed = ext.right_handed;

  orientation = ext.orientation;
  reference_source_phase = ext.reference_source_phase;

  feed_corrected = ext.feed_corrected;
  platform_corrected = ext.platform_corrected;

  atten_a = ext.atten_a;
  atten_b = ext.atten_b;

  orientation_Y_offset = ext.orientation_Y_offset;
  field_orientation = ext.field_orientation;

  return *this;
}

//! Destructor
Pulsar::Receiver::~Receiver ()
{
}

string Pulsar::Receiver::get_tracking_mode_string() const
{
  switch (tracking_mode) {
  case Feed:
    return "FA";
  case Celestial:
    return "CPA";
  case Galactic:
    return "GPA";
  }
  return "unknown";
}

/*! If this method is called, then any previous changes due to
 set_X_offset, set_Y_offset, or set_field_orientation will be
 reset. */
void Pulsar::Receiver::set_orientation (const Angle& angle)
{
  orientation = angle;
  orientation_Y_offset = 0.0;
  field_orientation = false;
}

const Angle Pulsar::Receiver::get_orientation () const
{
  Angle offset;
  if (field_orientation && basis == Signal::Linear)
    offset.setDegrees (-45);

  return orientation + orientation_Y_offset + offset;
}

//! Return true if the basis is right-handed
bool Pulsar::Receiver::get_right_handed () const
{
  return right_handed;
}

/*! If this method is called, then any changes due to set_Y_offset
  will be reset. */
void Pulsar::Receiver::set_right_handed (bool right)
{
  right_handed = right;
  orientation_Y_offset = 0.0;
}
 
//! Get the phase of the reference source
const Angle Pulsar::Receiver::get_reference_source_phase () const
{
  return reference_source_phase;
}

//! Set the phase of the reference source
void Pulsar::Receiver::set_reference_source_phase (const Angle& angle)
{
  reference_source_phase = angle;
}

/*! The orientation of the electric field vector that induces equal and
    in-phase responses in orthogonal receptors depends upon the basis.
    In the linear basis, it has an orientation of 45 degrees.  In the
    circular basis, 0 degrees.   Therefore, if this attribute is set,
    the interpretation of the orientation will become basis dependent. */
void Pulsar::Receiver::set_field_orientation (const Angle& angle)
{
  set_orientation (angle);
  field_orientation = true;
}

const Angle Pulsar::Receiver::get_field_orientation () const
{
  Angle offset = 0.0;
  if (basis == Signal::Linear)
    offset.setDegrees (45.0);

  return get_orientation() + offset;
}


/*! If this method is called, then any previous changes due to
  set_orientation or set_field_orientation will be reset. */
void Pulsar::Receiver::set_X_offset (const Angle& offset)
{
  orientation = offset;
  field_orientation = false;
}

const Angle Pulsar::Receiver::get_X_offset () const
{
  Angle offset = 0.0;
  if (!get_right_handed())
    offset.setDegrees (90.0);

  return get_orientation() + offset;
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
void Pulsar::Receiver::set_Y_offset (const Angle& offset)
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
    throw Error (InvalidParam, "Pulsar::Receiver::set_Y_offset",
		 "invalid offset = %lf deg", offset.getDegrees());
}

const Angle Pulsar::Receiver::get_Y_offset () const
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
void Pulsar::Receiver::set_calibrator_offset (const Angle& offset)
{
  if (offset == 0.0 || offset == 0.5*M_PI || offset == -0.5*M_PI)
    reference_source_phase = 2.0 * offset;
  else
    throw Error (InvalidParam, "Pulsar::Receiver::set_calibrator_offset",
                 "invalid offset = %lf deg", offset.getDegrees());
}

const Angle Pulsar::Receiver::get_calibrator_offset () const
{ 
  return 0.5*reference_source_phase;
}

bool Pulsar::Receiver::match (const Receiver* receiver, string& reason) const
{
  if (!receiver) {
    reason += Archive::match_indent + "no Receiver";
    return false;
  }

  bool result = true;

  if (basis != receiver->basis) {
    result = false;
    reason += Archive::match_indent + "basis mismatch: "
      + Signal::Basis2string(basis) + " != " 
      + Signal::Basis2string(receiver->basis);
  }

  return result;
}

//! Return the handedness correction matrix
Jones<double> Pulsar::Receiver::get_hand_transformation () const
{
  if ( feed_corrected || get_right_handed() )
    return 1.0;

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_transformation left-handed basis" << endl;

  /* rotate the basis by 180 degrees about the Stokes 2 axis
     in the linear basis: about the Stokes U axis
     in the circular basis: about the Stokes Q axis
     3-vector space is a subset of Stokes 4-vector space
     Jones rotations effect twice the angle in Poincare space */

  MEAL::Rotation rotation ( Vector<float, 3>::basis(1) );
  rotation.set_phi ( 0.5 * M_PI );

  return rotation.evaluate();

}

//! Return the feed correction matrix
Jones<double> Pulsar::Receiver::get_transformation () const
{
  Jones<double> xform = get_hand_transformation ();

  if ( feed_corrected || get_orientation() == 0 )
    return xform;

  Pauli::basis.set_basis( (Basis<double>::Type)get_basis() );

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_transformation orientation="
         << get_orientation().getDegrees() << " deg" << endl;

  // rotate the basis about the Stokes V axis
  MEAL::Rotation rotation ( Pauli::basis.get_basis_vector(2) );

  // the sign of this rotation may depend on handedness
  rotation.set_phi ( -get_orientation().getRadians() );
    
  xform *= rotation.evaluate();

  return xform;

}

Stokes<double> Pulsar::Receiver::get_reference_source () const
{
  Jones<double> xform = get_hand_transformation ();

  if (get_reference_source_phase() != 0) {

    if (Archive::verbose == 3)
      cerr << "Pulsar::Receiver::get_reference_source phase="
	   << get_reference_source_phase().getDegrees() << " deg" << endl;

    // rotate the basis about the Stokes 1 axis
    MEAL::Rotation rotation ( Vector<float,3>::basis(0) );

    rotation.set_phi ( 0.5*get_reference_source_phase().getRadians() );

    xform *= rotation.evaluate();

  }

  /* The conversion of a Stokes vector to a coherency matrix depends
   upon the basis.  The following line provides a basis-independent
   representation of a reference source that illuminates both
   receptors equally and in phase. */

  Quaternion<double,Hermitian> input (1,0,1,0);

  Stokes<double> output = coherency (xform * convert(input) * herm(xform));

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_reference_source output="
	 << output << endl;

  return output;
}

ostream& Pulsar::operator<< (ostream& ostr, const Pulsar::Receiver& recv)
{
  return ostr << endl <<
    "  orientation=" << recv.orientation.getDegrees() << "deg.\n"
    "  Y offset=" << recv.orientation_Y_offset.getDegrees() << "deg.\n"
    "  hand=" << recv.right_handed << endl <<
    "  field=" << recv.field_orientation << endl;
}
