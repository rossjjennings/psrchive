#include "Calibration/IRIonosphere.h"
#include "MEAL/NoParameters.h"
#include "MEAL/ForwardResult.h"

using namespace std;

Calibration::IRIonosphere::IRIonosphere () 
{
  parameter_policy  = new MEAL::NoParameters;
  evaluation_policy = new MEAL::ForwardResult<Complex2> (this, &faraday);
}

//! Return the name of the class
string Calibration::IRIonosphere::get_name () const
{
  return "IRIonosphere";
}

//! Set the reference frequency in MHz
void Calibration::IRIonosphere::set_reference_frequency (double MHz)
{
  faraday.set_reference_frequency (MHz);
}

//! Get the reference frequency in MHz
double Calibration::IRIonosphere::get_reference_frequency () const
{
  return faraday.get_reference_frequency ();
}


//! Set the frequency in MHz
void Calibration::IRIonosphere::set_frequency (double MHz)
{
  faraday.set_frequency (MHz);
}

//! Get the frequency in MHz
double Calibration::IRIonosphere::get_frequency () const
{
  return faraday.get_frequency ();
}
