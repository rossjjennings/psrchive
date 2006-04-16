/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Receiver.h"

#include "Pulsar/Receiver_Native.h"
#include "Pulsar/Receiver_Field.h"
#include "Pulsar/Receiver_Linear.h"

#include "Pauli.h"

/*! If the current state is not of the specified StateType, a new state
  will be created, copied, and installed */
template<class StateType>
StateType* Pulsar::Receiver::get () const
{
  Receiver* thiz = const_cast<Receiver*>( this );
  StateType* st = dynamic_cast<StateType*>( thiz->state.get() );

  if (!st) {
    st = new StateType;
    st -> copy (state);
    thiz->state = st;
  }
  
  return st;  
}

//! Default constructor
Pulsar::Receiver::Receiver () : Extension ("Receiver")
{
  tracking_mode = Feed;
  name = "unknown";

  state = new Native;

  feed_corrected = false;
  platform_corrected = false;

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
  state = new Native;
  state->copy(ext.state);

  feed_corrected = ext.feed_corrected;
  platform_corrected = ext.platform_corrected;

  atten_a = ext.atten_a;
  atten_b = ext.atten_b;

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

void Pulsar::Receiver::set_basis (Signal::Basis basis)
{
  get<Native>()->set_basis (basis);
}

/*! If this method is called, then any previous changes due to
 set_X_offset, set_Y_offset, or set_field_orientation will be
 reset. */
void Pulsar::Receiver::set_orientation (const Angle& angle)
{
  get<Native>()->set_orientation (angle);
}

/*! If this method is called, then any changes due to set_Y_offset
  will be reset. */
void Pulsar::Receiver::set_hand (Signal::Hand hand)
{
  get<Native>()->set_hand (hand);
}

//! Set the phase of the reference source
void Pulsar::Receiver::set_reference_source_phase (const Angle& angle)
{
  get<Native>()->set_reference_source_phase (angle);
}

void Pulsar::Receiver::set_field_orientation (const Angle& angle)
{
  get<Field>()->set_field_orientation (angle);
}

Angle Pulsar::Receiver::get_field_orientation () const
{
  return get<Field>()->get_field_orientation ();
}

/*! If this method is called, then any previous changes due to
  set_orientation or set_field_orientation will be reset. */
void Pulsar::Receiver::set_X_offset (const Angle& offset)
{
  get<Linear>()->set_X_offset (offset);
}

Angle Pulsar::Receiver::get_X_offset () const
{
  return get<Linear>()->get_X_offset ();
}


/*
  \param offset either 0 or +/- 180 degrees
*/
void Pulsar::Receiver::set_Y_offset (const Angle& offset)
{ 
  get<Linear>()->set_Y_offset (offset);
}

Angle Pulsar::Receiver::get_Y_offset () const
{
  return get<Linear>()->get_Y_offset ();
}

/*! In the linear basis, the noise diode must illuminate both receptors
    equally.  Therefore, there are only two valid orientations:

    \param offset either 0 or +/- 90 degrees
*/
void Pulsar::Receiver::set_calibrator_offset (const Angle& offset)
{
  get<Linear>()->set_calibrator_offset (offset);
}

Angle Pulsar::Receiver::get_calibrator_offset () const
{ 
  return get<Linear>()->get_calibrator_offset ();
}

static std::string match_indent = "\n\t";

bool Pulsar::Receiver::match (const Receiver* receiver, string& reason) const
{
  if (!receiver) {
    reason += match_indent + "no Receiver";
    return false;
  }

  bool result = true;

  if (get_basis() != receiver->get_basis()) {
    result = false;
    reason += match_indent + "basis mismatch: "
      + Signal::Basis2string(get_basis()) + " != " 
      + Signal::Basis2string(receiver->get_basis());
  }

  return result;
}

//! Return the handedness correction matrix
Jones<double> Pulsar::Receiver::get_hand_transformation () const
{
  if ( feed_corrected || get_hand() == Signal::Right )
    return 1.0;

  if (Archive::verbose > 1)
    cerr << "Pulsar::Receiver::get_hand_transformation left-handed" << endl;

  /* Switch the two receptors.  Note that det(J)=1. */
  complex<double> i (0,1);
  return Jones<double> (0, i,
                        i, 0);
}

Stokes<double> Pulsar::Receiver::get_reference_source () const
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_reference_source phase="
	 << get_reference_source_phase().getDegrees() << " deg" << endl;

  /* The following line provides a basis-independent representation of a
   reference source that illuminates both receptors equally and in phase. */
  Quaternion<double,Hermitian> ideal (1,0,1,0);

  /* .. and the following is the reference source */
  double phi = get_reference_source_phase().getRadians();
  Quaternion<double,Hermitian> cal (1,0,cos(phi),sin(phi));

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_reference_source cal=" << cal << endl;

  return standard (cal);
}
