#include "Pulsar/Receiver.h"
#include "Calibration/Rotation.h"
#include "Pauli.h"

//! Default constructor
Pulsar::Receiver::Receiver () : Extension ("Receiver")
{
  tracking_mode = Feed;
  name = "unknown";
  basis = Signal::Linear;

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
  basis = ext.basis;

  X_offset = ext.X_offset;
  Y_offset = ext.Y_offset;

  calibrator_offset = ext.calibrator_offset;

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

//! Return the feed correction matrix
Jones<double> Pulsar::Receiver::get_correction () const
{
  Jones<double> xform = Jones<double>::identity();

  if ( feed_corrected )
    return xform;

  Pauli::basis.set_basis( get_basis() );

  if (get_Y_offset().getDegrees() == 180.0) {

    if (Archive::verbose == 3)
      cerr << "Pulsar::Receiver::get_correction 180 phase shift in Y" << endl;
    
    // rotate the basis by 180 degrees about the Stokes Q axis
    Calibration::Rotation rotation ( Pauli::basis.get_basis_vector(0) );
    rotation.set_phi ( 0.5 * M_PI );
    
    xform *= rotation.evaluate();
    
  }
  else if (get_Y_offset() != 0) {
    
    Error error (InvalidState, "Pulsar::Receiver::get_correction");
    error << "Cannot correct Y_offset=" << Y_offset.getDegrees() << " degrees";
    throw error;
      
  }
  
  if (get_X_offset() != 0) {
    
    if (Archive::verbose == 3)
      cerr << "Pulsar::Receiver::get_correction X axis offset" << endl;

    // rotate the basis about the Stokes V axis
    Calibration::Rotation rotation ( Pauli::basis.get_basis_vector(2) );
    rotation.set_phi ( -get_X_offset().getRadians() );
    
    xform *= rotation.evaluate();
    
  }

  return xform;

}

Stokes<double> Pulsar::Receiver::get_reference_source () const
{
  Jones<double> xform = get_correction ();

  if (get_calibrator_offset() != 0) {

    if (Archive::verbose == 3)
      cerr << "Pulsar::Receiver::get_reference_source calibrator offset="
	   << get_calibrator_offset() << endl;

    // rotate the basis about the Stokes V axis
    Calibration::Rotation rotation ( Pauli::basis.get_basis_vector(2) );
    rotation.set_phi ( get_calibrator_offset().getRadians() );

    xform *= rotation.evaluate();

  }

  Stokes<double> noise_diode (1,0,1,0);
  Stokes<double> output_diode = transform (noise_diode, xform);

  if (Archive::verbose == 3)
    cerr << "Pulsar::Receiver::get_reference_source noise diode="
	 << output_diode << endl;

  return output_diode;
}
