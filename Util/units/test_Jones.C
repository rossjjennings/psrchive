#include <iostream>
#include "Jones.h"

int main () 
{
  //
  // test constructors
  //
  cerr << "\n*************** Testing constructors" << endl;

  cerr << "Jones<float> null construct" << endl;
  Jones<float> temp;

  cerr << "Jones<float> construct from complex<float>" << endl;
  Jones<float> j_f1 (complex<float>(1.2,-2.3), complex<float>(2.3,-3.4),
		     complex<float>(4.5,6.7), complex<float>(-8.9,9.0));

  cerr << "Jones<float> construct from float cast to complex<float>" << endl;
  Jones<float> j_f2 (1.2, 1,
		     2, 3.4);

  Jones<float> j_f3 (complex<float>(1.2), complex<float>(1),
		     complex<float>(2), complex<float>(3.4));

  if (j_f2 != j_f3) {
    cerr << "\nError in Jones<float> construction\n" 
      "   Jones<float>=" << j_f2 << endl <<
      "   Jones<float>=" << j_f3 << endl;
    return -1;
  }

  //
  // test operator =
  //
  cerr << "\n*************** Testing operator =" << endl;

  cerr << "Jones<float> = Jones<float>" << endl;
  temp = j_f1;
  
  cerr << "Jones<double> = Jones<float>" << endl;
  Jones<double> j_d1 = temp;

   cerr << "Jones<double> = Jones<double>" << endl;
  Jones<double> j_d2 = j_d1;
 
  cerr << "Jones<float> = Jones<double>" << endl;
  temp = j_d2;

  cerr << "Jones<float> == Jones<float>" << endl;
  if (j_f1 != temp) {
    cerr << "\nError in casting from Jones<float> to Jones<double>" << endl;
    return -1;
  }

  cerr << "Jones<float> == Jones<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError in casting from Jones<float> to Jones<double>" << endl;
    return -1;
  }

  //
  // test operator + and - (implicitly tests operator+= and -=)
  //
  cerr << "\n*************** Testing operator + and -" << endl;

  j_d2 = j_f2;

  cerr << "Jones<float> + Jones<double>" << endl;
  temp = j_f1 + j_d2;

  j_d2 = temp;

  cerr << "Jones<double> - Jones<float>" << endl;
  j_d1 = j_d2 - j_f2;

  cerr << "Jones<float> == Jones<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError adding and subtracting Jones<float> and Jones<double>\n" 
      "   Jones<double>=" << j_d1 << endl <<
      "   Jones<float>=" << j_f1 << endl;
    return -1;
  }

  //
  // test operator * and / complex<float> (implicitly tests operator*= and /=)
  //
  cerr << "\n*************** Testing operator * and / complex<float>" << endl;
  complex<float> factor (3.2, -8.7);

  j_d1 = j_f1;

  cerr << "complex<float> * Jones<double>" << endl;
  j_d2 = factor * j_d1;

  cerr << "Jones<double> / complex<float>" << endl;
  j_d1 = j_d2 / factor;

  cerr << "Jones<float> == Jones<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError in Jones<float> and Jones<double> multiplication\n" 
      "   Jones<double>=" << j_d1 << endl <<
      "   Jones<float>=" << j_f1 << endl;
    return -1;
  }

  //
  // test operator * and / double (implicitly tests operator*= and /=)
  //
  cerr << "\n*************** Testing operator * and / double" << endl;
  double dfactor = 63.2;

  j_d1 = j_f1;

  cerr << "double * Jones<double>" << endl;
  j_d2 = dfactor * j_d1;

  cerr << "Jones<double> / double" << endl;
  j_d1 = j_d2 / dfactor;

  cerr << "Jones<float> == Jones<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError in Jones<float> and Jones<double> multiplication\n" 
      "   Jones<double>=" << j_d1 << endl <<
      "   Jones<float>=" << j_f1 << endl;
    return -1;
  }

  //
  // test operator Jones * Jones (implicitly tests operator*=)
  //
  cerr << "\n*************** Testing operator * Jones<float>" << endl;

  cerr << "Jones<float>::identity=" << Jones<float>::identity << endl;

  cerr << "Jones<float> * Jones<float>::identity" << endl;
  j_d1 = j_f1 * Jones<float>::identity;

  cerr << "Jones<float> == Jones<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError Jones<float> * Jones<float>::identity\n" 
      "   Jones<double>=" << j_d1 << endl <<
      "   Jones<float>=" << j_f1 << endl <<
      "   Jones<float>::identity=" << Jones<float>::identity << endl;
    return -1;
  }

  //
  // test det(Jones) function
  //
  cerr << "\n*************** Testing determinant" << endl;

  j_d1 = j_f1;

  cerr << "det(Jones<double>)" << endl;
  complex<double> det1 = det(j_d1);

  cerr << "Jones<double> /= sqrt(det)" << endl;
  j_d1 /= sqrt(det1);

  cerr << "det(Jones<double>)" << endl;
  complex<float> det2 (det(j_d1));

  if (norm(det2 - 1.0) > 1e-10) {
    cerr << "\nError normalizing matrix by square root of its determinant\n" 
      "   det(Jones<double>/det)=" << det2 << " != 1" << endl;
    return -1;
  }

  //
  // test norm, trace and Jones::dag methods
  //
  cerr << "\n*************** Testing variance, trace, and Hermitian" << endl;

  cerr << "norm (Jones<double>)" << endl;
  float variance1 = norm(j_d2);

  cerr << "Jones<double>::dag" << endl;
  j_d1 = j_d2*j_d2.dag();

  cerr << "trace (Jones<double>)" << endl;
  complex<float> variance2 (trace (j_d1));

  if ( variance1 != variance2 ) {
    cerr << "\nError norm(J) != trace (J * J.dag())" << endl;
    return -1;
  }

  //
  // test Jones::inv method
  //
  cerr << "\n*************** Testing inverse" << endl;

  cerr << "Jones<float>::inv" << endl;
  j_d2 = j_f1.inv();

  cerr << "Jones<float> * Jones<double>" << endl;
  j_d1 = j_f1 * j_d2;

  cerr << "norm (Jones<double>)" << endl;
  variance1 = norm (Jones<double>::identity - j_d1);

  if ( variance1 > 1e-10 ) {
    cerr << "\nError multipying matrix by its inverse\n" 
      "   Jones<double>=" << j_d1 << endl;
    return -1;
  }

  return 0;
}
