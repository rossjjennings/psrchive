#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

void F77_FUNC(iri2rm,IRI2RM) (double* Glong, double* Glati, 
			      float* year, int* mmdd, double* ut, 
			      float* az, float* el, float* RM);

int main ()
{
  double Glong = 150;
  double Glati = -33;
  float year = 2003;
  int mmdd = 220;
  double ut = 13.4;
  float az = -118;
  float el = 69;
  float RM = 0;
  
  F77_FUNC(iri2rm,IRI2RM) (&Glong, &Glati, 
			   &year, &mmdd, &ut, 
			   &az, &el, &RM);

  printf ("RM=%lf\n", RM);
  return 0;
}
