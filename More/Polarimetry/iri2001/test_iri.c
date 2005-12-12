#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

void F77_FUNC(iri2rm,IRI2RM) (double* Glong, double* Glati, 
			      double* year, int* mmdd, double* ut, 
			      double* az, double* el, double* RM);

int main ()
{
  double Glong;
  double Glati;
  double year;
  int mmdd;
  double ut;
  double az;
  double el;
  double RM;
  
  F77_FUNC(iri2rm,IRI2RM) (&Glong, &Glati, 
			   &year, &mmdd, &ut, 
			   &az, &el, &RM);

  return 0;
}
