#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <values.h>

#include "sky_coord.h"
#include "coord.h"
#include "f772c.h"

// redwards 10Aug99 function to compute angular separation of a pair
// of spherical coordinates

#define SLA_dsep SLA_FUNC(sla_dsep,slaDsep)
extern "C" double SLA_dsep(double *, double *, double *, double*);

Angle oldAngularSeparation(const AnglePair& a1, const AnglePair& a2)
{
  Angle a;
  double a1_1, a1_2, a2_1, a2_2;


  a1_1=a1.angle1.getradians();
  a1_2=a1.angle2.getradians();
  a2_1=a2.angle1.getradians();
  a2_2=a2.angle2.getradians();

  a.setradians(SLA_dsep(&a1_1, &a1_2, &a2_1, &a2_2));

  return a;
}

int main ()
{
  cerr << "If this process appears to hang, then fix Angle::wrap" << endl;
  Angle big (MAXFLOAT*0.5);
  cerr << "big=" << big << endl;
  cerr << "Angle::wrap test passed" << endl;

  sky_coord coordinates;

  char coordstr [80];

  strcpy (coordstr, "04:37:15.747849-47:15:08.23371");

  coordinates.setHMSDMS (coordstr);
  AnglePair galactic = coordinates.getGalactic();
  double l = galactic.angle1.getradians();
  double b = coordinates.angle2.getradians();

  printf ("COORDSTR: %s\n", coordstr);
  printf ("(RA DEC) (%s)  l:%g   b:%g\n", coordinates.getHMSDMS().c_str(),
	  l*(180/M_PI), b*(180/M_PI));

  double step = 0.3;
  double start = - 2.2*M_PI;
  double end = 2.2*M_PI;

  AnglePair coord1;
  AnglePair coord2;

  Angle sepnew;
  Angle sepold;

  cerr << "Testing the angular separation routine.  This may take a while..." 
       << endl;

  unsigned error_count = 0;
  unsigned error_limit = 10;
  double tolerance = 1e-10;

  double fraction_complete = 0.0;

  for (double theta1 = start; theta1 < end; theta1 += step) {
    fraction_complete = (theta1-start) / (end - start);
      fprintf (stderr, "%5.2lf%%   complete\r",
	       100.0 * fraction_complete);
    for (double phi1 = start; phi1 < end; phi1 += step) {
      for (double theta2 = start; theta2 < end; theta2 += step)
	for (double phi2 = start; phi2 < end; phi2 += step) {
	  coord1.setRadians (theta1, phi1);
	  coord2.setRadians (theta2, phi2);

	  sepnew = coord1.angularSeparation (coord2);
	  sepold = oldAngularSeparation (coord1,coord2);

	  if (fabs( sepnew.getradians() - sepold.getradians() ) > tolerance ) {
	    //if (sepnew != sepold) {
	    cerr << "Different Answers::"
		 << "  a1:" << coord1 
		 << "  a2:" << coord2 << endl
		 << "  old:" << sepold
		 << "  new:" << sepnew << endl;
	    error_count ++;
	    if (error_count > error_limit)
	      return -1;
	  }
	}
    }
  }

  if (!error_count)
    cerr << "Test completed successfully." << endl;
  return 0;
}
