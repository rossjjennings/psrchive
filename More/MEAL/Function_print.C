#include "MEPL/Function.h"
#include "stringtok.h"
#include "tostring.h"

//! Prints the values of model parameters and fit flags to a string
void Model::Function::print (string& text) const
{
  text = get_name ();
  print_parameters (text, "\n ");
}


//! Prints the values of model parameters and fit flags to a string
void Model::Function::print_parameters (string& text, const string& s) const
{
  for (unsigned iparam=0; iparam < get_nparam(); iparam++) {
    text += s + get_param_name(iparam)
      + " " + tostring (get_param (iparam)) 
      + " " + tostring (get_infit (iparam));
    if ( get_infit (iparam) )
      text += " " + tostring (sqrt(get_variance(iparam)));
  }
}

