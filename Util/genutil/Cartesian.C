#include <math.h>

#include "cartesian.h"
#include "angle.h"

Cartesian::Cartesian ()
{
  x = y = z = 0.0;
}

Cartesian::Cartesian (double ux, double uy, double uz)
{
  x=ux; y=uy; z=uz;
}

Cartesian::Cartesian (const AnglePair& spherical)
{
  double theta, phi;
  spherical.getRadians (&theta, &phi);

  double cosPhi = cos (phi);
  x = cos (theta) * cosPhi;
  y = sin (theta) * cosPhi;
  z = sin (phi);
}

Cartesian::Cartesian (const Cartesian& in_cart)
{
  *this = in_cart;
}

Cartesian& Cartesian::operator = (const Cartesian& in_cart)
{
  if (this != &in_cart) {
    x = in_cart.x;
    y = in_cart.y;
    x = in_cart.z;
  }
  return *this;
}

Cartesian& Cartesian::operator += (const Cartesian& cart)
{
  x += cart.x;
  y += cart.y;
  z += cart.z;
  return *this;
}

Cartesian& Cartesian::operator -= (const Cartesian& cart)
{
  x -= cart.x;
  y -= cart.y;
  z -= cart.z;
  return *this;
}

Cartesian& Cartesian::operator *= (double linear)
{
  x *= linear;
  y *= linear;
  z *= linear;
  return *this;
}

Cartesian& Cartesian::operator /= (double linear)
{
  this->operator*= (1.0/linear);
  return *this;
}

const Cartesian operator + (const Cartesian& cart1, const Cartesian& cart2)
{
  Cartesian result (cart1);
  result += cart2;
  return result;
}

const Cartesian operator - (const Cartesian& cart1, const Cartesian& cart2)
{
  Cartesian result (cart1);
  result -= cart2;
  return result;
}

const Cartesian operator * (const Cartesian& cart, double linear)
{
  Cartesian result (cart);
  result *= linear;
  return result;
}  

const Cartesian operator / (const Cartesian& cart, double linear)
{
  Cartesian result (cart);
  result /= linear;
  return result;
}  

// cross product
const Cartesian operator % (const Cartesian &, const Cartesian &)
{
  throw ("Cartesian operator % not implemented\n");
}

// scalar product
double operator * (const Cartesian& cart1, const Cartesian& cart2)
{
  return (cart1.x*cart2.x + cart1.y*cart2.y + cart1.z*cart2.z);
}

int operator == (const Cartesian& cart1, const Cartesian& cart2)
{
  return ( (cart1.x == cart2.x) &&
	   (cart1.y == cart2.y) &&
	   (cart1.z == cart2.z) );
}

int operator != (const Cartesian& cart1, const Cartesian& cart2)
{
  return ( (cart1.x != cart2.x) ||
	   (cart1.y != cart2.y) ||
	   (cart1.z != cart2.z) );
}

double Cartesian::modSquared () const
{
  return (x*x + y*y + z*z);
}

double Cartesian::mod () const
{
  return sqrt (x*x + y*y + z*z);
}

Angle Cartesian::angularSeparation (const Cartesian& c1, const Cartesian& c2)
{
  return Angle (acos( (c1 * c2) / (c1.mod() * c2.mod()) ));
}
