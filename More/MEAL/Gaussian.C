#include "MEPL/Gaussian.h"

Model::Gaussian::Gaussian ()
  : UnivariateOptimizedScalar (3)
{
  cyclic = false;
}

//! Return the name of the class
string Model::Gaussian::get_name () const
{
  return "Gaussian";
}

//! Return the name of the specified parameter
string Model::Gaussian::get_param_name (unsigned index) const
{
  switch (index) {
  case 0:
    return "centre";
  case 1:
    return "width";
  case 2:
    return "height";
  default:
    return "ERROR";
  }
}

//! Set the centre
void Model::Gaussian::set_centre (double centre)
{
  set_param (0, centre);
}

//! Get the centre
double Model::Gaussian::get_centre () const
{
  return get_param (0);
}

//! Set the width
void Model::Gaussian::set_width (double width)
{
  set_param (1, width);
}

//! Get the width
double Model::Gaussian::get_width () const
{
  return get_param (1);
}

//! Set the height
void Model::Gaussian::set_height (double height)
{
  set_param (2, height);
}

//! Get the height
double Model::Gaussian::get_height () const
{
  return get_param (2);
}

//! Set the cyclic
void Model::Gaussian::set_cyclic (bool _cyclic)
{
  cyclic = _cyclic;
}

//! Get the cyclic
bool Model::Gaussian::get_cyclic () const
{
  return cyclic;
}

//! Return the value (and gradient, if requested) of the function
void Model::Gaussian::calculate (double& result, vector<double>* grad)
{
  double centre   = get_centre ();
  double width    = get_width ();
  double height   = get_height ();
  double abscissa = get_abscissa ();
  bool   cyclic   = get_cyclic ();

  double current_centre = centre;

  result = 0;

  if (grad) {
    grad->resize (3);
    for (unsigned ig=0; ig<3; ig++)
      (*grad)[ig] = 0;
  }

  int iter = 1;

  if (cyclic) {
    current_centre -= 1.0;
    iter = 3;
  }

  for (int i=0; i<iter; i++) {

    //cerr << " cyclic:" << cyclic << " i:" << i 
    // << " c:" << current_centre << " x:" << x << " arg:" << arg << endl;

    double dist = abscissa - current_centre;
    double arg = dist / width;

    double ex = exp (-arg*arg);
    double fac = height * ex * 2.0 * arg / width;
    
    result += height * ex;

    if (grad) {

      // dy/dcentre
      (*grad)[0] += fac;

      // dy/dwidth
      (*grad)[1] += fac * arg;
      
      // dy/dheight
      (*grad)[2] += ex;
    
    }

    current_centre += 1.0;
    
  }

  if (verbose) {
    cerr << "Model::Gaussian::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "Model::Gaussian::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

