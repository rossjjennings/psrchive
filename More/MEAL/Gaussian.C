#include "Calibration/Gaussian.h"

Calibration::Gaussian::Gaussian ()
  : UnivariateOptimizedScalar (3)
{
  cyclic = false;
}

//! Return the name of the class
string Calibration::Gaussian::get_name () const
{
  return "Gaussian";
}

//! Return the name of the specified parameter
string Calibration::Gaussian::get_param_name (unsigned index) const
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
void Calibration::Gaussian::set_centre (double centre)
{
  set_param (0, centre);
}

//! Get the centre
double Calibration::Gaussian::get_centre () const
{
  return get_param (0);
}

//! Set the width
void Calibration::Gaussian::set_width (double width)
{
  set_param (1, width);
}

//! Get the width
double Calibration::Gaussian::get_width () const
{
  return get_param (1);
}

//! Set the height
void Calibration::Gaussian::set_height (double height)
{
  set_param (2, height);
}

//! Get the height
double Calibration::Gaussian::get_height () const
{
  return get_param (2);
}

//! Set the cyclic
void Calibration::Gaussian::set_cyclic (bool _cyclic)
{
  cyclic = _cyclic;
}

//! Get the cyclic
bool Calibration::Gaussian::get_cyclic () const
{
  return cyclic;
}

//! Return the value (and gradient, if requested) of the function
void Calibration::Gaussian::calculate (double& result, vector<double>* grad)
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
    cerr << "Calibration::Gaussian::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "Calibration::Gaussian::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}

