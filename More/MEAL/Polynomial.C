#include "MEPL/Polynomial.h"
#include "stringtok.h"
#include "tostring.h"

Model::Polynomial::Polynomial (unsigned ncoef)
  : UnivariateOptimizedScalar (ncoef)
{
  x_0 = 0;
}

//! Return the name of the class
string Model::Polynomial::get_name () const
{
  return "Polynomial";
}

//! Return the name of the specified parameter
string Model::Polynomial::get_param_name (unsigned index) const
{
  return "c_" + tostring (index);
}


void Model::Polynomial::parse (const string& line)
{
  string temp = line;

  // the key should be the name of the parameter to be set
  string key = stringtok (temp, " \t\n");
  string value = stringtok (temp, " \t\n");

  if (verbose)
    cerr << "Model::Polynomial::parse key='" << key << "'"
      " value='" << value << "'" << endl;

  if (key == "ncoef" || key == "order") {

    unsigned nparam;
    if ( sscanf (value.c_str(), "%d", &nparam) != 1 )
      throw Error (InvalidParam, "Model::Polynomial::parse",
                   "value='" + value + "' could not be parsed as an integer");

    if (key == "order")
      nparam ++;

    resize (nparam);
    return;

  }

  else if (key == "x_0" ) {

    if ( sscanf (value.c_str(), "%lf", &x_0) != 1 )
      throw Error (InvalidParam, "Model::Polynomial::parse",
                   "value='" + value + "' could not be parsed as a double");
    return;

  }
  
  Function::parse (line);

}

//! Prints the values of model parameters and fit flags to a string
void Model::Polynomial::print_parameters (string& text,
						const string& sep) const
{
  text += sep + "ncoef " + tostring (get_nparam());
  Function::print_parameters (text, sep);
}

//! Return the value (and gradient, if requested) of the function
void Model::Polynomial::calculate (double& result, vector<double>* grad)
{
  double x = abscissa - x_0;

  result = 0;

  if (grad)
    grad->resize (get_nparam());

  double xpower = 1.0;

  for (unsigned ig=0; ig<get_nparam(); ig++) {
    result += get_param(ig) * xpower;
    if (grad)
      (*grad)[ig] = xpower;
    xpower *= x;
  }

  if (verbose) {
    cerr << "Model::Polynomial::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "Model::Polynomial::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}
