#include <stdlib.h>
#include <iostream>

#include <complex>
#include <string>

template <class Type1, class Type2, class Element>
class MatrixTest {
  
 public:
  
  MatrixTest (unsigned int seed=13)
    { verbose = false; if (seed) srand(seed); }
  
  virtual void runtest (unsigned nloops);
  
  bool verbose;
  
};

template <class T>
void random (T& val)
{
  T r1 = T(rand()) - T(RAND_MAX)/2.0;

  int random = rand() + 1;

  T r2 = T(random);

  if (random%2)
    r2 = 1.0/r2;

  val = r1*r2;
}

template <class T>
void random (complex<T>& val)
{
  T re, im;

  random(re);
  random(im);

  val = complex<T> (re, im);
}

template <class T, class U>
T max (const T& a, const U& b)
{
  if (a>b)
    return a;
  else
    return b;
}

template <class Type1, class Type2, class Element>
void test_matrix (const Type1& t1_1, const Type1& t1_2, const Type2& t2_null,
		  const Element& p, bool verbose)
{
  Type1 temp1;
  Type1 temp2;

  Element one (1.0);

  //
  // test operator =
  //
  if (verbose)
  cerr << "\n*************** Testing operator =" << endl;

  if (verbose)
  cerr << "Type1 = Type1" << endl;
  temp1 = t1_1;

  if (verbose) cerr << "Type2 = Type1" << endl;
  Type2 type2_1 = temp1;

  if (verbose) cerr << "Type2 = Type2" << endl;
  Type2 type2_2 = type2_1;
 
  if (verbose) cerr << "Type1 = Type2" << endl;
  temp1 = type2_2;

  if (verbose) cerr << "Type1 == Type1" << endl;
  if (t1_1 != temp1)
    throw string ("test_matrix "
		 "Error casting from Type1 to Type2");


  if (verbose) cerr << "Type1 == Type2" << endl;
  if (t1_1 != type2_1)
    throw string ("test_matrix "
		 "Error casting from Type1 to Type2");

  //
  // test operator + and - (implicitly tests operator+= and -=)
  //
  if (verbose) cerr << "\n*************** Testing operator + and -" << endl;

  type2_2 = t1_2;

  if (verbose) cerr << "Type1 + Type2" << endl;
  temp1 = t1_1 + type2_2;

  type2_2 = temp1;

  if (verbose) cerr << "Type2 - Type1" << endl;
  type2_1 = type2_2 - t1_2;

  if (verbose) cerr << "Type1 == Type2" << endl;
  if (t1_1 != type2_1)
    throw string ("test_matrix "
		  "Error adding and subtracting Type1 and Type2");



  //
  // test operator * and / Element (implicitly tests operator*= and /=)
  //
  if (verbose)
    cerr << "\n*************** Testing operator * and / Element" << endl;
  Element dfactor;
  random (dfactor);

  type2_1 = t1_1;

  if (verbose) cerr << "Element * Type2" << endl;
  type2_2 = dfactor * type2_1;

  if (verbose) cerr << "Type2 / Element" << endl;
  type2_1 = type2_2 / dfactor;

  if (verbose) cerr << "Type1 == Type2" << endl;
  if (norm(type2_1 - t1_1)/norm(type2_1) > 1e-17) {
    cerr << "Error Type2=" << type2_1 
	 << " * / Element=" << dfactor
	 << " != Type1=" << t1_1 << endl;
    throw string ("test_matrix Error in Type2 * / Element");
  }

  //
  // test operator Type1 * Type1 (implicitly tests operator*=)
  //
  if (verbose) cerr << "\n*************** Testing operator * Type1" << endl;

  if (verbose) cerr << "Type1::identity=" << Type1::identity() << endl;

  if (verbose) cerr << "Type1 * Type1::identity" << endl;
  type2_1 = t1_1 * Type1::identity();

  if (verbose) cerr << "Type1 == Type2" << endl;
  if (t1_1 != type2_1)
    throw string ("test_matrix Error Type1 * Type1::identity");


  //
  // test complex conjugate
  //
  if (verbose) cerr << "\n*************** Testing conjugate" << endl;

  if (verbose) cerr << "conj (t1_1 * t1_2)" << endl;
  type2_1 = conj (t1_1 * t1_2);

  if (verbose) cerr << "conj (t1_1) * conj (t1_2)" << endl;
  type2_2 = conj (t1_1) * conj (t1_2);

  if (verbose) cerr << "Type2 == Type2" << endl;
  if (type2_2 != type2_1)
    throw string ("test_matrix Error Conjugate");
  //
  // test Hermitian
  //
  if (verbose) cerr << "\n*************** Testing Hermitian" << endl;

  type2_1 = t1_1;
  type2_2 = t1_2;

  if (verbose) cerr << "herm (Type2_1 * Type2_2)" << endl;
  temp1 = herm (type2_1 * type2_2);

  if (verbose) cerr << "herm (Type2_2) * herm (Type2_1)" << endl;
  temp2 = herm (type2_2) * herm (type2_1);

  if (verbose) cerr << "Type2 == Type2" << endl;

  if ( norm(temp2 - temp1)/norm(temp2) > 1e-17) {
    cerr << "Error Hermitian"
      "\n   M1=" << t1_1 <<
      "\n   M2=" << t1_2 <<
      "\n  herm(M1*M2)=" << temp1 <<
      "\n  herm(M2)*herm(M1)=" << temp2 << endl;

    throw string ("test_matrix Error Hermitian");
  }

  //
  // test det(Type1) function
  //
  if (verbose) cerr << "\n*************** Testing determinant" << endl;

  type2_1 = t1_1;

  if (verbose) cerr << "det(Type2)" << endl;
  Element det1 (det(type2_1));

  if (verbose) cerr << "Type2 /= sqrt(det)" << endl;
  type2_1 /= sqrt(det1);

  if (verbose) cerr << "det(Type2)" << endl;
  Element det2 (det(type2_1));

  if (norm(det2 - one) > 1e-10)
    throw string ("test_matrix "
		 "Error normalizing by square root of determinant"); 


  //
  // test norm and trace
  //
  if (verbose)
    cerr << "\n*************** Testing norm and trace" << endl;

  type2_2 = t1_1;

  if (verbose) cerr << "norm (Type2)" << endl;
  float variance1 = norm(type2_2);

  if (verbose) cerr << "herm (Type2)" << endl;
  type2_1 = type2_2*herm(type2_2);

  if (verbose) cerr << "trace (Type2)" << endl;
  Element variance2 (trace (type2_1));

  if ( variance1 != variance2 ) {
    cerr << "Q=" << type2_2 << endl;
    cerr << "herm(Q)=" << herm(type2_2) << endl;
    cerr << "Q*herm(Q)=" << type2_1 << endl;
    cerr << "norm(Q) = " << variance1 << endl;
    cerr << "trace (Q * herm(Q)) = " << variance2 << endl;

    throw string ("test_matrix "
		 "Error norm(Q) != trace (Q * herm(Q))");
  }

  //
  // test Type1::inv method
  //
  if (verbose) cerr << "\n*************** Testing inverse" << endl;

  if (verbose) cerr << "Type1::inv" << endl;
  type2_2 = inv(t1_1);

  if (verbose) cerr << "Type2 * Type2" << endl;
  type2_1 = Type2(t1_1) * type2_2;

  float tolerance = 1e-10 * max (norm(t1_1), norm(type2_2));

  if (verbose) cerr << "norm (Type2)" << endl;
  variance1 = norm (Type2::identity() - type2_1);

  if ( variance1 > tolerance ) {
    cerr << "Error multipying Type1 by its inverse"
      "\n   Q=" << t1_1 <<
      "\n   inv(Q)=" << type2_2 <<
      "\n   Q*inv(Q)=" << type2_1 << endl;
    
    throw string ("test_matrix "
		  "Error multipying Type1 by its inverse");
  }
}

template <class Type1, class Type2, class Element>
void MatrixTest<Type1, Type2, Element>::runtest (unsigned nloops)
{
  //
  // test constructors
  //
  if (verbose)
    cerr << "\n*************** Testing constructors" << endl;

  if (verbose)
    cerr << "Type1 null construct" << endl;
  Type1 temp;

  if (verbose)
    cerr << "Element array (static)" << endl;
  Element random_elements[4];
  for (int i=0; i<4; i++)
    random (random_elements[i]);

  if (verbose)
    cerr << "Type1 construct from 4 Elements" << endl;
  Type1 t1_1 (random_elements[0], random_elements[1],
		 random_elements[2], random_elements[3]);

  if (verbose)
    cerr << "Type1 construct from Type1" << endl;
  Type1 t1_2 (t1_1);

  if (verbose)
    cerr << "TEST: Type1 == Type1" << endl;
  if (t1_1 != t1_2)
    throw string ("MatrixTest::runtest "
		  "Error in constructing Type1 from Type1");

  if (verbose)
    cerr << "Type1 = Type1" << endl;
  temp = t1_1;

  if (verbose)
    cerr << "TEST: Type1 == Type1" << endl;
  if (t1_1 != temp)
    throw string ("MatrixTest::runtest "
		 "Error setting Type1 equal to Type1");

  for (unsigned iloop=0; iloop<nloops; iloop++) {

    for (int i=0; i<4; i++)
      random (random_elements[i]);

    Type1 t1_1 (random_elements[0], random_elements[1],
		random_elements[2], random_elements[3]);
    
    Type1 t1_2 (random_elements[0], random_elements[1],
		random_elements[2], random_elements[3]);

    Type2 t2_null;
    Element null;

    try {
      test_matrix (t1_1, t1_2, t2_null, null, verbose);
    }
    catch (string& error) {
      cerr << "MatrixTest::runtest error on loop "<< iloop <<"/"<< nloops 
	   << endl;
      throw error;
    }

  }
}
