#include "MEPL/Polar.h"
#include "MEPL/Gain.h"
#include "MEPL/Boost.h"
#include "MEPL/Rotation.h"

#include "Pauli.h"

#include <assert.h>

// #define _DEBUG 1

void Model::Polar::init ()
{

#ifdef _DEBUG
  cerr << "Model::Polar::init" << endl;
#endif

  // name = "Polar";

  // Note, these objects will be destroyed during Reference::To destructor
  gain  = new Model::Gain;
  add_model (gain);
  // gain->name = "Polar::Gain";

  boost = new Model::Boost;
  add_model (boost);
  // boost->name = "Polar::Boost";

  for (unsigned i=0; i<3; i++) {

    rotation[i] = new Model::Rotation(Vector<double, 3>::basis(i));
    // rotation[i]->name = "Polar::Rotation " + string(1, char('0' + i));

    add_model (rotation[i]);

  }

#ifdef _DEBUG
  cerr << "Model::Polar::init exit" << endl;
#endif

}
  
Model::Polar::Polar ()
{
  init ();
}

Model::Polar::~Polar ()
{
#ifdef _DEBUG
  cerr << "Model::Polar destructor" << endl;
#endif
}

Model::Polar::Polar (const Polar& polar)
{

#ifdef _DEBUG
  cerr << "Model::Polar copy constructor" << endl;
#endif

  init ();
  operator = (polar);

#ifdef _DEBUG
  cerr << "Model::Polar copy constructor exit" << endl;
#endif

}

//! Equality Operator
const Model::Polar& 
Model::Polar::operator = (const Polar& polar)
{
  if (&polar == this)
    return *this;

#ifdef _DEBUG
  cerr << "Model::Polar operator =" << endl;
#endif

  *gain = *(polar.gain);
  *boost = *(polar.boost);
  for (unsigned i=0; i<3; i++)
    *(rotation[i]) = *(polar.rotation[i]);

  return *this;
}

//! Return the name of the class
string Model::Polar::get_name () const
{
  return "Polar";
}


Estimate<double> Model::Polar::get_gain () const
{
  return gain->get_Estimate(0);
}


Estimate<double> Model::Polar::get_boostGibbs (unsigned i) const
{
  assert (i < 3);
  assert (boost->get_nparam() == 3);

  return boost->get_Estimate(i);
}

Estimate<double> Model::Polar::get_rotationEuler (unsigned i) const
{
  return rotation[i]->get_Estimate(0);
}

void Model::Polar::set_gain (const Estimate<double>& g)
{
  gain->set_Estimate (0, g);
}

void Model::Polar::set_boostGibbs (unsigned i, 
					 const Estimate<double>& b)
{
  boost->set_Estimate (i, b);
}
   
void Model::Polar::set_rotationEuler (unsigned i,
					    const Estimate<double>& phi_i)
{
  rotation[i]->set_Estimate (0, phi_i);
}


/*! Given the measured Stokes parameters of the linear calibrator
  noise diode, \f$ S_{\rm cal}=(I=1,Q=0,U=1,V=0)\f$, and the
  unpolarized system temperature, \f$ S_{\rm sys}=(T,0,0,0) \f$, this
  method solves for the instrumental gain, boost, and two of the three
  basis rotations that may be compared to the differential phase, and
  the orientation of the feed.

  \post source and sky will be modified to represent the un-boosted
  Stokes parameters.  */
void Model::Polar::solve (Quaternion<Estimate<double>, Hermitian>& source,
				Quaternion<Estimate<double>, Hermitian>& sky)
{
  // Assuming that the off pulse radiation is unpolarized, the boost
  // component of the system response is simply the square root of the
  // off-pulse Stokes (normalized by the sqrt of the determinant)
  Quaternion<Estimate<double>, Hermitian> boost = sqrt( sky );
  boost /= sqrt( det(boost) );

#if _DEBUG
  Quaternion<double,Hermitian> b;
  for (unsigned i=0; i<4; i++)
    b[i] = boost[i].val;
  Jones< double > j = convert (b);

  complex<double > d;
  Quaternion<double, Hermitian> hq;
  Quaternion<double, Unitary> uq;

  polar (d, hq, uq, j);

  cerr << "hq=" << hq << endl;
  cerr << "uq=" << uq << endl;
  cerr << "d=" << d << endl;
#endif

  for (unsigned i=0; i<3; i++)
    set_boostGibbs (i, boost[i+1]);

  // Undo the boost of the on-pulse coherencies
  // remember: herm(unboost) = unboost
  Jones< Estimate<double> > unboost = convert (inv (boost));
  source = real(convert(unboost * source * unboost));
  sky = real(convert(unboost * sky * unboost));

  Jones< Estimate<double> > jones_cal = convert (source);

  // The differential phase is given by the phase of j01, regardless of basis
  //  Estimate<double> phi = arg (jones_cal.j01);
  Estimate<double> phi = atan2(jones_cal.j01.imag(),jones_cal.j01.real());
  set_rotationEuler (0, 0.5 * phi);

  // This solution tells nothing about the rotation about the U-axis
  set_rotationEuler (1, 0.0);

  /* This works only if the feeds are linear.  Otherwise, differential phase
     and feed orientation are degenerate with each other. */
  Estimate<double> theta = atan2 ( source.s1, source.s2/cos(phi) );
  set_rotationEuler (2, 0.5 * theta);

  /* After correcting the cal for the boost, the total intensity represents
     the total gain of the system in units of calibrator flux */

cerr << "source.s0=" << source.s0 << endl;

  Estimate<double> G = source.s0;
  set_gain (sqrt(G));

}

