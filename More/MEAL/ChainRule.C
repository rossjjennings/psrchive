/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ChainRule.h"

using namespace std;

void MEAL::covariance ( MEAL::Scalar* function, unsigned index, 
			vector<unsigned>& function_imap,
			vector< vector<double> >& covar)
{
  vector<double> gradient;
  function->evaluate( &gradient );
  unsigned nparam = function->get_nparam();

  for (unsigned i=0; i<covar.size(); i++)
  {
    if (i == index)
    {
      covar[i][i] = function->estimate().get_variance();
      continue;
    }

    if( covar[index][i] != 0.0 )
    {
      throw Error (InvalidState, "MEAL::covariance",
              "unexpected non-zero covariance between iparam=%u and jparam=%u", index, i);
    }

    double covariance = 0;
    for (unsigned iparam=0; iparam < nparam; iparam++)
      covariance += gradient[iparam] * covar[ function_imap[iparam] ][i];

    covar[index][i] = covar[i][index] = covariance;
  }
}
