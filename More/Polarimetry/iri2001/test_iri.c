#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

void F77_FUNC(iri2rm,IRI2RM) (double* Glong, double* Glati, 
			      double* year, int* mmdd, double* ut, 
			      double* az, double* el, double* RM);

int main ()
{
  double Glong = 110;
  double Glati = -33;
  double year = 1995;
  int mmdd = 1213;
  double ut = 7.34;
  double az = 34;
  double el = 34;
  double RM = 0;
  
  F77_FUNC(iri2rm,IRI2RM) (&Glong, &Glati, 
			   &year, &mmdd, &ut, 
			   &az, &el, &RM);

  return 0;
}
