#include <algorithm>
#include <math.h>

#include "Cartesian.h"

Cartesian::Cartesian (const AnglePair& spherical)
{
  double theta, phi;
  spherical.getRadians (&theta, &phi);

  double cosPhi = cos (phi);
  x = cos (theta) * cosPhi;
  y = sin (theta) * cosPhi;
  z = sin (phi);
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


// cross product
const Cartesian operator % (const Cartesian& c1, const Cartesian& c2)
{
  Cartesian result;
  result.x = c1.y * c2.z - c1.z * c2.y;
  result.y = c1.z * c2.x - c1.x * c2.z;
  result.z = c1.x * c2.y - c1.y * c2.x;
  return result;
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


void Cartesian::rotate (double& x1, double& x2, double radians)
{
  double cosr = cos (radians);
  double sinr = sin (radians);

  double temp = x1;
  x1 = cosr * temp - sinr * x2;
  x2 = sinr * temp + cosr * x2;
}

void Cartesian::x_rot (const Angle& phi)
{
  rotate (y, z, phi.getradians());
}

void Cartesian::y_rot (const Angle& phi)
{
  rotate (z, x, phi.getradians());
}

void Cartesian::z_rot (const Angle& phi)
{
  rotate (x, y, phi.getradians());
}

// rotates the point about an arbitrary vector
void Cartesian::rot (const Cartesian& v, const Angle& phi)
{
  double s = sin(phi.getradians());
  double c = cos(phi.getradians());
  double u = 1.0 - c;

  double R[3][3] =
  { {v.x*v.x*u + c    ,  v.y*v.x*u - v.z*s,  v.z*v.x*u + v.y*s},
    {v.x*v.y*u + v.z*s,  v.y*v.y*u + c    ,  v.z*v.y*u - v.x*s},
    {v.x*v.z*u - v.y*s,  v.y*v.z*u + v.x*s,  v.z*v.z*u + c} };

  double tx=x, ty=y, tz=z;
  x = R[0][0]*tx + R[0][1]*ty + R[0][2]*tz;
  y = R[1][0]*tx + R[1][1]*ty + R[1][2]*tz;
  z = R[2][0]*tx + R[2][1]*ty + R[2][2]*tz;
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
