#include "MEPL/Function.h"

bool Model::Function::verbose = false;
bool Model::Function::very_verbose = false;
bool Model::Function::check_zero = false;
bool Model::Function::check_variance = false;

Model::Function::Function ()
{
#ifdef _DEBUG
  cerr << "Model::Function default constructor" << endl;
#endif
  evaluation_changed = true;
}

Model::Function::Function (const Function& model)
{
#ifdef _DEBUG
  cerr << "Model::Function copy constructor" << endl;
#endif
  evaluation_changed = true;
}

Model::Function::~Function ()
{
#ifdef _DEBUG
  cerr << "Model::Function::~Function" << endl;
#endif
}

Model::Function& Model::Function::operator = (const Function& model)
{
  copy (&model);
  return *this;
}

void Model::Function::copy (const Function* model)
{
#ifdef _DEBUG
  cerr << "Model::Function::copy" << endl;
#endif

  if (this == model)
    return;

  unsigned nparam = get_nparam ();

  if (nparam != model->get_nparam())
    throw Error (InvalidParam, "Model::Function::operator =",
		 "nparam=%d != copy.nparam=%d", nparam, model->get_nparam());

  for (unsigned iparam=0; iparam<nparam; iparam++) {

    set_param( iparam, model->get_param(iparam) );
    set_variance( iparam, model->get_variance(iparam) );
    set_infit( iparam, model->get_infit(iparam) );

  }

  // name = model->name;

  set_evaluation_changed();
}


Estimate<double> Model::Function::get_Estimate (unsigned index) const
{
  return Estimate<double> ( get_param(index), get_variance(index) );
}

void Model::Function::set_Estimate (unsigned index,
				       const Estimate<double>& estimate)
{
  set_param( index, estimate.val );
  set_variance( index, estimate.var );
}

