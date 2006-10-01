/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Receiver.h"
#include "MEAL/Rotation1.h"
#include "Pauli.h"

using namespace std;

//! Return the feed correction matrix
Jones<double> Pulsar::Receiver::get_transformation () const
{
  Jones<double> xform = get_hand_transformation ();

  if ( feed_corrected || get_orientation() == 0 )
    return xform;

  Pauli::basis.set_basis( get_basis() );

  if (Archive::verbose > 1)
    cerr << "Pulsar::Receiver::get_transformation basis="
         << Signal::basis_string(get_basis()) << " orientation="
         << get_orientation().getDegrees() << " deg" << endl;

  // rotate the basis about the Stokes V axis
  MEAL::Rotation1 rotation ( Pauli::basis.get_basis_vector(2) );

  // the sign of this rotation may depend on handedness
  rotation.set_phi ( get_orientation().getRadians() );

  xform *= rotation.evaluate();

  return xform;

}
