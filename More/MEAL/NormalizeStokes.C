#include "MEPL/NormalizeStokes.h"
#include "MEPL/ScalarValue.h"
#include "MEPL/ScalarMath.h"

void Model::NormalizeStokes::init ()
{
  unsigned ipol = 0;

  for (ipol=0; ipol<input.size(); ipol++)
    input[ipol] = *(new Model::ScalarValue);

  Model::ScalarMath invariant = sqrt( det(input) );

  for (ipol=0; ipol<output.size(); ipol++)
    output[ipol] = input[ipol] / invariant;
}

Model::NormalizeStokes::NormalizeStokes ()
{
  init ();
}

Model::NormalizeStokes::NormalizeStokes (const NormalizeStokes&)
{
  init ();
}

Model::NormalizeStokes::~NormalizeStokes ()
{

}

Model::NormalizeStokes&
Model::NormalizeStokes::operator = (const NormalizeStokes&)
{
  return *this;
}

void
Model::NormalizeStokes::normalize (Stokes<Estimate<float> >& stokes)
{
  Stokes<Estimate<double> > temp = stokes;
  normalize (temp);
  stokes = temp;
}

//! Set the scale to be used during conversion
void
Model::NormalizeStokes::normalize (Stokes<Estimate<double> >& stokes)
{
  Estimate<double> invariant = det(stokes);

  if ( invariant.val < stokes[0].var )
    throw Error (InvalidParam, "Model::NormalizeStokes::normalize",
		 "invariant=%lf < variance=%lf", invariant.val, stokes[0].var);

  unsigned ipol = 0;

  for (ipol=0; ipol<stokes.size(); ipol++)
    input[ipol].get_expression()->set_Estimate( 0, stokes[ipol] );

  Stokes< Estimate<double> > normalized;

  for (ipol=0; ipol<stokes.size(); ipol++)
    normalized[ipol] = output[ipol].get_Estimate();

  stokes = normalized;
}
