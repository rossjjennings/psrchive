#include "MEAL/Polar.h"

#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarValue.h"

#include "Pauli.h"

using namespace std;;

#if defined(__ICC)
#include "icomplex.h"
#endif

complex<MEAL::ScalarMath> 
ComplexScalar (const complex< Estimate<double> >& z)
{
  return complex<MEAL::ScalarMath>
    ( *(new MEAL::ScalarValue( z.real() )),
      *(new MEAL::ScalarValue( z.imag() )) );
}

//! Polar decompose the Jones matrix
void MEAL::Polar::solve (Jones< Estimate<double> >& j)
{
  Jones<ScalarMath> jones
    ( ComplexScalar(j.j00), ComplexScalar(j.j01),
      ComplexScalar(j.j10), ComplexScalar(j.j11) );

  complex<ScalarMath> determinant (0.0,0.0);
  Quaternion<ScalarMath,Hermitian> boost;
  Quaternion<ScalarMath,Unitary>   rotation;

  polar( determinant, boost, rotation, jones);
      
  set_gain( determinant.real().get_Estimate() );

  for (unsigned i=0; i<3; i++) {
    set_boostGibbs( i, boost[i+1].get_Estimate() );
    set_rotationEuler( i, rotation[i+1].get_Estimate() );
  } 
}
