/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/MultivariatePolicy.h"
#include "MEAL/Axis.h"
#include "MEAL/Function.h"

#include <iostream>
#include <functional>

using namespace std;
using namespace std::placeholders;

MEAL::MultivariatePolicy::MultivariatePolicy (unsigned ndim, Function* context)
 : ArgumentPolicy (context)
{
  abscissa.resize( ndim, 0.0 );
}

//! Destructor
MEAL::MultivariatePolicy::~MultivariatePolicy ()
{
}

//! Copy constructor
MEAL::MultivariatePolicy::MultivariatePolicy (const MultivariatePolicy& u)
 : ArgumentPolicy (0)
{
  abscissa = u.abscissa;
}

//! Assignment operator
const MEAL::MultivariatePolicy&
MEAL::MultivariatePolicy::operator = (const MultivariatePolicy& u)
{
  if (this != &u)
    for (unsigned i=0; i < abscissa.size(); i++)
      set_abscissa( i, u.get_abscissa(i) );

  return *this;
}

//! Clone construtor
MEAL::MultivariatePolicy*
MEAL::MultivariatePolicy::clone (Function* context) const
{
  unsigned ndim = abscissa.size();
  MultivariatePolicy* retval = new MultivariatePolicy (ndim, context);
  *retval = *this;
  return retval;
}

//! Set the abscissa value
void MEAL::MultivariatePolicy::set_abscissa (unsigned idim, double value)
{
  if (abscissa.at(idim) == value)
    return;

  if (Function::verbose)
    cerr << "MEAL::MultivariatePolicy::set_abscissa " 
         << idim << " " << value << endl;

  abscissa[idim] = value;
  get_context()->set_evaluation_changed ();
}

//! Get the abscissa value
double MEAL::MultivariatePolicy::get_abscissa (unsigned idim) const
{
  return abscissa.at (idim);
}

class BindDimension : public Reference::Able
{
  Reference::To<MEAL::MultivariatePolicy, false> context;
  unsigned idim;

  public:
    BindDimension (unsigned _idim, MEAL::MultivariatePolicy* policy)
    { idim = _idim;  context = policy; }

    void set_abscissa (double val) { context->set_abscissa (idim, val); }
};

void MEAL::MultivariatePolicy::set_argument (unsigned dim, Argument* argument)
{
  if (Function::verbose)
    std::cerr << "MEAL::MultivariatePolicy::set_argument" << std::endl;

  if (dim != 0)
    return;

  Axis<double>* axis = dynamic_cast< Axis<double>* > (argument);
  if (!axis)
    return;

  BindDimension* bound = new BindDimension (dim, this);

  axis->signal.connect( bound, &BindDimension::set_abscissa );
}

