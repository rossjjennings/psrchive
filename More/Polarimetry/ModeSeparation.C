/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ModeSeparation.h"

#include "MEAL/OrthogonalModes.h"
#include "MEAL/JonesMueller.h"

#include "MEAL/ProductRule.h"
#include "MEAL/SumRule.h"
#include "MEAL/Cast.h"
#include "MEAL/Union.h"
#include "MEAL/Vectorize.h"

using namespace MEAL;

Complex2* product (Scalar* a, Complex2* A)
{
  ProductRule<Complex2>* product = new ProductRule<Complex2>;
  product->add_model( cast<Complex2>(a) );
  product->add_model( A );
  return product;
}

void Pulsar::ModeSeparation::init ()
{
  OrthogonalModes* modes = new OrthogonalModes;

  mode_A = modes->get_modeA();
  dof_A = new ScalarParameter;

  mode_B = modes->get_modeB();
  dof_B = new ScalarParameter;

  mode_C = new Coherency;
  dof_C = new ScalarParameter;

  SumRule<Complex2>* mean_sum = new SumRule<Complex2>;

  mean_sum->add_model( modes );
  mean_sum->add_model( mode_C );

  mean = mean_sum;

  SumRule<Real4>* cov_sum = new SumRule<Real4>;
  
  // these three lines are not correct
  cov_sum->add_model( new JonesMueller( product(dof_A,mode_A) ) );
  cov_sum->add_model( new JonesMueller( product(dof_B,mode_B) ) );
  cov_sum->add_model( new JonesMueller( product(dof_C,mode_C) ) );

  covariance = cov_sum;

  Union* join = new Union;
  join->push_back( vectorize(mean_sum) );
  join->push_back( vectorize(cov_sum) );

  space = join;
}
