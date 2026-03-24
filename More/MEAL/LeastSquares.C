/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/LeastSquares.h"

void MEAL::LeastSquares::set_convergence_chisq (float chisq)
{
  convergence_chisq = chisq;
}

void MEAL::LeastSquares::set_convergence_delta (float delta)
{
  convergence_delta = delta;
}

void MEAL::LeastSquares::set_maximum_reduced_chisq (float max)
{
  maximum_reduced = max;
}

