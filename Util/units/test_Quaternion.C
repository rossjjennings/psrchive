#include <iostream>
#include "Quaternion.h"

int main () 
{
  //
  // test constructors
  //
  cerr << "\n*************** Testing constructors" << endl;

  cerr << "Quaternion<float> null construct" << endl;
  Quaternion<float> temp;

  cerr << "Quaternion<float> construct from float" << endl;
  Quaternion<float> j_f1 (1.2, 1, 2, 3.4);

  Quaternion<float> j_f2 (-3.4, 2.5, 1.2, -5.6);

  //
  // test operator =
  //
  cerr << "\n*************** Testing operator =" << endl;

  cerr << "Quaternion<float> = Quaternion<float>" << endl;
  temp = j_f1;
  
  cerr << "Quaternion<double> = Quaternion<float>" << endl;
  Quaternion<double> j_d1 = temp;

   cerr << "Quaternion<double> = Quaternion<double>" << endl;
  Quaternion<double> j_d2 = j_d1;
 
  cerr << "Quaternion<float> = Quaternion<double>" << endl;
  temp = j_d2;

  cerr << "Quaternion<float> == Quaternion<float>" << endl;
  if (j_f1 != temp) {
    cerr << "\nError in casting from Quaternion<float> to Quaternion<double>" << endl;
    return -1;
  }

  cerr << "Quaternion<float> == Quaternion<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError in casting from Quaternion<float> to Quaternion<double>" << endl;
    return -1;
  }

  //
  // test operator + and - (implicitly tests operator+= and -=)
  //
  cerr << "\n*************** Testing operator + and -" << endl;

  j_d2 = j_f2;

  cerr << "Quaternion<float> + Quaternion<double>" << endl;
  temp = j_f1 + j_d2;

  j_d2 = temp;

  cerr << "Quaternion<double> - Quaternion<float>" << endl;
  j_d1 = j_d2 - j_f2;

  cerr << "Quaternion<float> == Quaternion<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError adding and subtracting Quaternion<float> and Quaternion<double>\n" 
      "   Quaternion<double>=" << j_d1 << endl <<
      "   Quaternion<float>=" << j_f1 << endl;
    return -1;
  }

  //
  // test operator * and / double (implicitly tests operator*= and /=)
  //
  cerr << "\n*************** Testing operator * and / double" << endl;
  double dfactor = 63.2;

  j_d1 = j_f1;

  cerr << "double * Quaternion<double>" << endl;
  j_d2 = dfactor * j_d1;

  cerr << "Quaternion<double> / double" << endl;
  j_d1 = j_d2 / dfactor;

  cerr << "Quaternion<float> == Quaternion<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError in Quaternion<float|double> multiplication\n" 
      "   Quaternion<double>=" << j_d1 << endl <<
      "   Quaternion<float>=" << j_f1 << endl;
    return -1;
  }

  //
  // test operator Quaternion * Quaternion (implicitly tests operator*=)
  //
  cerr << "\n*************** Testing operator * Quaternion<float>" << endl;

  cerr << "Quaternion<float>::identity=" << Quaternion<float>::identity << endl;

  cerr << "Quaternion<float> * Quaternion<float>::identity" << endl;
  j_d1 = j_f1 * Quaternion<float>::identity;

  cerr << "Quaternion<float> == Quaternion<double>" << endl;
  if (j_f1 != j_d1) {
    cerr << "\nError Quaternion<float> * Quaternion<float>::identity\n" 
      "   Quaternion<double>=" << j_d1 << endl <<
      "   Quaternion<float>=" << j_f1 << endl <<
      "   Quaternion<float>::identity=" << Quaternion<float>::identity << endl;
    return -1;
  }


  Quaternion<float,Hermitian> h_f1 (1.2, 1, 2, 3.4);

  Quaternion<float,Hermitian> h_f2 (-3.4, 2.5, 1.2, -5.6);

  Quaternion<double,Hermitian> h_d1 = h_f1 * h_f2;



  return 0;
}
