#include <algorithm>
#include <math.h>

#include "Cartesian.h"
#include "angle.h"

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
    z = in_cart.z;
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

const Cartesian pdiv (const Cartesian& cart1, const Cartesian& cart2)
{
  Cartesian result (cart1);
  result.x /= cart2.x;
  result.y /= cart2.y;
  result.z /= cart2.z;
  return result;
}

const Cartesian pmult (const Cartesian& cart1, const Cartesian& cart2)
{
  Cartesian result (cart1);
  result.x *= cart2.x;
  result.y *= cart2.y;
  result.z *= cart2.z;
  return result;
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

Cartesian min (const Cartesian& cart1, const Cartesian& cart2)
{
  // cerr << "Cartesian min" << endl;
  return Cartesian ( min(cart1.x, cart2.x),
		     min(cart1.y, cart2.y),
		     min(cart1.z, cart2.z) );
}

Cartesian max (const Cartesian& cart1, const Cartesian& cart2)
{
  // cerr << "Cartesian max" << endl;
  return Cartesian ( max(cart1.x, cart2.x),
		     max(cart1.y, cart2.y),
		     max(cart1.z, cart2.z) );
}

double& Cartesian::operator [] (char dimension)
{
  switch (dimension) {
  case 'x': return x;
  case 'y': return y;
  case 'z': return z;
  default:
    throw;
  }
}

/*
const double& Cartesian::operator [] (int dimension) const
{
  switch (char(dimension)) {
  case 'x': return x;
  case 'y': return y;
  case 'z': return z;
  default:
    throw;
  }
}
*/

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

ostream& operator << (ostream& ostr, const Cartesian& coord) {
  return ostr << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")";
}

// to create a window on the pgplot screen from bottom left to top right
void diagonalize (Cartesian& bottom_left, Cartesian& upper_right)
{
  if (bottom_left.x > upper_right.x)
    swap (upper_right.x, bottom_left.x);
  if (bottom_left.y > upper_right.y)
    swap (upper_right.y, bottom_left.y);
  if (bottom_left.z > upper_right.z)
    swap (upper_right.z, bottom_left.z);
}
