/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/PolnCalibratorExtensionTI.h"
using namespace std;

#include <assert.h>

//! Default constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension ()
  : CalibratorExtension ("PolnCalibratorExtension")
{
  init ();
}

void Pulsar::PolnCalibratorExtension::init ()
{
  type = Calibrator::SingleAxis;
  nparam = 3;
  has_covariance = false;
}

//! Copy constructor
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension
(const PolnCalibratorExtension& copy)
  : CalibratorExtension (copy)
{
  operator = (copy);
}

//! Operator =
const Pulsar::PolnCalibratorExtension&
Pulsar::PolnCalibratorExtension::operator= 
(const PolnCalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  if (Archive::verbose == 3)
    cerr << "Pulsar::PolnCalibratorExtension::operator=" << endl;

  type = copy.get_type();
  epoch = copy.get_epoch();
  nparam = copy.get_nparam();
  has_covariance = copy.get_has_covariance();

  unsigned nchan = copy.get_nchan();
  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    response[ichan] = copy.response[ichan];

  return *this;
}

//! Destructor
Pulsar::PolnCalibratorExtension::~PolnCalibratorExtension ()
{
}

//! Set the type of the instrumental response parameterization
void Pulsar::PolnCalibratorExtension::set_type (Calibrator::Type _type)
{
  if (type == _type)
    return;

  type = _type;

  switch (type) {
  case Calibrator::SingleAxis:
    nparam = 3; break;
  case Calibrator::Polar:
    nparam = 6; break;
  case Calibrator::Hamaker:
  case Calibrator::Britton:
    nparam = 7; break;
  }
}


//! Set the number of frequency channels
void Pulsar::PolnCalibratorExtension::set_nchan (unsigned _nchan)
{
  CalibratorExtension::set_nchan( _nchan );
  response.resize( _nchan );
  construct ();
}

//! Set the weight of the specified channel
void Pulsar::PolnCalibratorExtension::set_weight (unsigned ichan, float weight)
{
  CalibratorExtension::set_weight (ichan, weight);
  if (weight == 0)
    set_valid (ichan, false);
}

bool Pulsar::PolnCalibratorExtension::get_valid (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_valid");
  return response[ichan].get_valid();
}

void Pulsar::PolnCalibratorExtension::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::set_valid");

  if (!valid)
    weight[ichan] = 0;

  response[ichan].set_valid (valid);
}

unsigned Pulsar::PolnCalibratorExtension::get_nparam () const
{
  return nparam;
}

bool Pulsar::PolnCalibratorExtension::get_has_covariance () const
{
  return has_covariance;
}

void Pulsar::PolnCalibratorExtension::set_has_covariance (bool has)
{
  has_covariance = has;
}

//! Get the transformation for the specified frequency channel
Pulsar::PolnCalibratorExtension::Transformation* 
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan)
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return &response[ichan];
}

//! Get the transformation for the specified frequency channel
const Pulsar::PolnCalibratorExtension::Transformation*
Pulsar::PolnCalibratorExtension::get_transformation (unsigned ichan) const
{
  range_check (ichan, "Pulsar::PolnCalibratorExtension::get_transformation");
  return &response[ichan];
}

void Pulsar::PolnCalibratorExtension::construct ()
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::PolnCalibratorExtension::construct nchan="
         << response.size() << " type=" 
         << Calibrator::Type2str (get_type()) << endl;

  for (unsigned ichan=0; ichan<response.size(); ichan++)
    response[ichan].set_nparam (nparam);
}


const char* Pulsar::Calibrator::Type2str (Type type)
{
  switch (type) {
  case Flux:
    return "Flux";
  case SingleAxis:
    return "SingleAxis";
  case Polar:
    return "Polar";
  case Hamaker:
    return "Hamaker";
  case Britton:
    return "Britton";
  case Hybrid:
    return "Hybrid";
  case Corrections:
    return "Corrections";
  }

  return "Invalid";

}


Pulsar::Calibrator::Type Pulsar::Calibrator::str2Type (const char* s)
{
  if (strcasecmp(s, "Flux") == 0)
    return Flux;
  if (strcasecmp(s, "SingleAxis") == 0)
    return SingleAxis;
  if (strcasecmp(s, "Polar") == 0)
    return Polar;
  if (strcasecmp(s, "Hamaker") == 0)
    return Hamaker;
  if (strcasecmp(s, "Britton") == 0)
    return Britton;
  return (Type) -1;
}

using namespace Pulsar;

PolnCalibratorExtension::Transformation::Transformation ()
{
  valid = true;
}

unsigned
PolnCalibratorExtension::Transformation::get_nparam() const
{
  return params.size();
}

void PolnCalibratorExtension::Transformation::set_nparam (unsigned s)
{
  params.resize(s);
}

double 
PolnCalibratorExtension::Transformation::get_param (unsigned i) const
{
  return params[i].get_value();
}

void PolnCalibratorExtension::Transformation::set_param 
(unsigned i, double value)
{
  params[i].set_value(value);
}

double
PolnCalibratorExtension::Transformation::get_variance (unsigned i) 
const
{
  return params[i].get_variance();
}

void PolnCalibratorExtension::Transformation::set_variance
(unsigned i, double var)
{
  params[i].set_variance(var);
}

Estimate<double>
PolnCalibratorExtension::Transformation::get_Estimate (unsigned i) const
{
  return params[i];
}

void PolnCalibratorExtension::Transformation::set_Estimate
(unsigned i, const Estimate<double>& e)
{
  params[i] = e;
}

bool PolnCalibratorExtension::Transformation::get_valid () const
{
  return valid;
}

void PolnCalibratorExtension::Transformation::set_valid (bool flag)
{
  valid = flag;
}

//! Get the covariance matrix of the model paramters
vector< vector<double> >
PolnCalibratorExtension::Transformation::get_covariance () const
{
  unsigned nparam = get_nparam();

  unsigned size = nparam * (nparam+1) / 2;
  if (size != covariance.size())
    throw Error (InvalidState,
		 "PolnCalibratorExtension::Transformation::get_covariance",
		 "covariance vector has incorrect length = %u (expect %u)",
		 covariance.size(), size);

  vector<vector<double> > matrix (nparam, vector<double>(nparam));

  unsigned count = 0;
  for (unsigned i=0; i<nparam; i++)
    for (unsigned j=i; j<nparam; j++) {
      matrix[i][j] = matrix[j][i] = covariance[count];
      count ++;
    }

  assert (count == covariance.size());

  return matrix;
}

//! Set the covariance matrix of the model paramters
void PolnCalibratorExtension::Transformation::set_covariance 
(const vector< vector<double> >& covar)
{
  unsigned nparam = get_nparam();

  assert (nparam == covar.size());

  covariance.resize( nparam * (nparam+1) / 2 );

  unsigned count = 0;
  for (unsigned i=0; i<nparam; i++) {
    assert (nparam == covar[i].size());
    for (unsigned j=i; j<nparam; j++) {
      covariance[count] = covar[i][j];
      count ++;
    }
  }

  assert (count == covariance.size());
}

//! Get the covariance matrix efficiently
void PolnCalibratorExtension::Transformation::get_covariance 
(vector<double>& covar) const
{
  covar = covariance;
}

//! Set the covariance matrix efficiently
void PolnCalibratorExtension::Transformation::set_covariance
(const vector<double>& covar)
{
  covariance = covar;

  unsigned nparam = get_nparam();
  unsigned expect = nparam * (nparam+1) / 2;
  if (covar.size() != expect)
    throw Error (InvalidParam,
		 "PolnCalibratorExtension::Transformation::set_covariance",
		 "covariance vector length=%u != expected=%u=%u*(%u+1)/2",
		 covar.size(), expect, nparam, nparam);

  unsigned icovar = 0;

  // set the variance stored in the transformation
  for (unsigned i=0; i<nparam; i++)
    for (unsigned j=i; j<nparam; j++) {
      if (i==j)
	set_variance (j,covar[icovar]);
      icovar++;
    }

  if (icovar != covar.size())
    throw Error (InvalidState,
		 "PolnCalibratorExtension::Transformation::set_covariance",
		 "covariance vector length=%u != icovar=%u",
		 covar.size(), icovar);

}

//! Get the text interface 
Reference::To< TextInterface::Class > PolnCalibratorExtension::get_text_interface()
{
  return new PolnCalibratorExtensionTI( this );
}



