/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/RVM.h"

using namespace MEAL;

RVM::RVM ()
{
  reference_position_angle = new ScalarParameter;
  reference_position_angle->set_value_name ("PA_0");

  magnetic_meridian = new ScalarParameter;
  magnetic_meridian->set_value_name ("phi_0");

  // the argument to this function is pulse phase, phi
  longitude = new ScalarArgument; 
  copy_univariate_policy (longitude);
}

void RVM::set_atan_Psi (const ScalarMath& sin_Psi, const ScalarMath& cos_Psi)
{
  ScalarMath result = atan2(sin_Psi,cos_Psi) + *reference_position_angle;

  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);

#if 0
  
  ScalarMath N = cos(result);
  ScalarMath E = sin(result);
  
#else

  ScalarMath cos_Psi0 = cos( *reference_position_angle );
  ScalarMath sin_Psi0 = sin( *reference_position_angle );

  /*
    WvS - 30 August 2021
    
    N and E are now normalized in ComplexRVM::set_rvm
  */
  
  ScalarMath N = (cos_Psi * cos_Psi0 - sin_Psi * sin_Psi0);
  ScalarMath E = (sin_Psi * cos_Psi0 + cos_Psi * sin_Psi0);

#endif

  north = N.get_expression ();
  east = E.get_expression ();
}
