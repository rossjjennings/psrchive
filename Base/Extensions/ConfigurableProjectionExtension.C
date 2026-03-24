/***************************************************************************
 *
 *   Copyright (C) 2022 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConfigurableProjectionExtension.h"
#include "templates.h"

#include <string.h>
#include <assert.h>

using namespace std;
using namespace Pulsar;

//! Default constructor
ConfigurableProjectionExtension::ConfigurableProjectionExtension ()
  : HasChannels ("ConfigurableProjectionExtension")
{
  init ();
}

void ConfigurableProjectionExtension::init ()
{
  nparam = 0;
  has_covariance = false;
}

//! Copy constructor
ConfigurableProjectionExtension::ConfigurableProjectionExtension
(const ConfigurableProjectionExtension& copy)
  : HasChannels (copy)
{
  operator = (copy);
}

//! Operator =
const ConfigurableProjectionExtension&
ConfigurableProjectionExtension::operator= 
(const ConfigurableProjectionExtension& copy)
{
  if (this == &copy)
    return *this;

  if (Archive::verbose > 2)
    cerr << "ConfigurableProjectionExtension::operator=" << endl;

  configuration = copy.get_configuration();

  nparam = copy.get_nparam();
  has_covariance = copy.get_has_covariance();

  unsigned nchan = copy.get_nchan();
  set_nchan (nchan);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
    response[ichan] = copy.response[ichan];

  return *this;
}

//! Destructor
ConfigurableProjectionExtension::~ConfigurableProjectionExtension ()
{
}

//! Set the number of frequency channels
void ConfigurableProjectionExtension::set_nchan (unsigned _nchan)
{
  response.resize( _nchan );
  construct ();
}

unsigned ConfigurableProjectionExtension::get_nchan () const
{
  return response.size();
}

void ConfigurableProjectionExtension::remove_chan (unsigned first, unsigned last)
{
  remove (response, first, last);
}

//! Set the weight of the specified channel
void ConfigurableProjectionExtension::set_weight (unsigned ichan, float weight)
{
  set_valid (ichan, weight != 0.0);
}

//! Set the weight of the specified channel
float ConfigurableProjectionExtension::get_weight (unsigned ichan) const
{
  if (get_valid (ichan))
    return 1.0;
  else
    return 0.0;
}

//! Get the weight of the specified channel

bool ConfigurableProjectionExtension::get_valid (unsigned ichan) const
{
  range_check (ichan, "ConfigurableProjectionExtension::get_valid");
  return response[ichan].get_valid();
}

void ConfigurableProjectionExtension::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "ConfigurableProjectionExtension::set_valid");
  response[ichan].set_valid (valid);
}

unsigned ConfigurableProjectionExtension::get_nparam () const
{
  return nparam;
}

void ConfigurableProjectionExtension::set_nparam (unsigned _nparam)
{
  nparam = _nparam;
  construct ();
}

bool ConfigurableProjectionExtension::get_has_covariance () const
{
  return has_covariance;
}

void ConfigurableProjectionExtension::set_has_covariance (bool has)
{
  has_covariance = has;
}

void ConfigurableProjectionExtension::range_check (unsigned ichan, const char* method) const
{
  if (ichan >= response.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d",
                 ichan, response.size());
}

//! Get the transformation for the specified frequency channel
ConfigurableProjectionExtension::Transformation* 
ConfigurableProjectionExtension::get_transformation (unsigned ichan)
{
  range_check (ichan, "ConfigurableProjectionExtension::get_transformation");
  return &response[ichan];
}

//! Get the transformation for the specified frequency channel
const ConfigurableProjectionExtension::Transformation*
ConfigurableProjectionExtension::get_transformation (unsigned ichan) const
{
  range_check (ichan, "ConfigurableProjectionExtension::get_transformation");
  return &response[ichan];
}

Estimate<float>
ConfigurableProjectionExtension::get_Estimate ( unsigned iparam, unsigned ichan ) const
{
  return get_transformation(ichan)->get_Estimate(iparam);
}

void ConfigurableProjectionExtension::set_Estimate (unsigned iparam, unsigned ichan,
                                                    const Estimate<float>& datum)
{
#if _DEBUG
  cerr << "ConfigurableProjectionExtension::set_Estimate iparam=" << iparam
       << " ichan=" << ichan << " val=" << datum << endl;
#endif

  get_transformation(ichan)->set_Estimate(iparam, datum);
}

void ConfigurableProjectionExtension::construct ()
{
  if (Archive::verbose > 2)
    cerr << "ConfigurableProjectionExtension::construct nchan="
         << response.size() << endl;

  for (unsigned ichan=0; ichan<response.size(); ichan++)
  {
    response[ichan].set_nparam (nparam);
  }
}

void ConfigurableProjectionExtension::frequency_append (Archive* to, const Archive* from)
{
  const ConfigurableProjectionExtension* ext = from->get<ConfigurableProjectionExtension>();
  if (!ext)
    throw Error (InvalidState, "ConfigurableProjectionExtension::frequency_append",
		 "other Archive does not have a ConfigurableProjectionExtension");

  if (nparam != ext->nparam)
    throw Error (InvalidState, "ConfigurableProjectionExtension::frequency_append",
		 "incompatible nparam this=%u other=%u",
		 nparam, ext->nparam);

  if (has_covariance != ext->has_covariance)
    throw Error (InvalidState, "ConfigurableProjectionExtension::frequency_append",
		 "incompatible has_covariance this=%u other=%u",
		 has_covariance, ext->has_covariance);

  bool in_order = in_frequency_order (to, from);
  
  response.insert ( in_order ? response.end() : response.begin(),
		    ext->response.begin(), ext->response.end() );
}


ConfigurableProjectionExtension::Transformation::Transformation ()
{
  valid = true;
}

unsigned ConfigurableProjectionExtension::Transformation::get_nparam() const
{
  return params.size();
}

//! Get the name of the specified model parameter
string ConfigurableProjectionExtension::Transformation::get_param_name (unsigned i) const
{
  return names[i];
}

//! Set the name of the specified model parameter
void ConfigurableProjectionExtension::Transformation::set_param_name (unsigned i, const string& n)
{
  names[i] = n;
}

//! Get the description of the specified model parameter
string ConfigurableProjectionExtension::Transformation::get_param_description (unsigned i) const
{
  return descriptions[i];
}

//! Set the description of the specified model parameter
void ConfigurableProjectionExtension::Transformation::set_param_description (unsigned i, const string& n)
{
  descriptions[i] = n;
}

void ConfigurableProjectionExtension::Transformation::set_nparam (unsigned s)
{
  params.resize(s);
  names.resize(s);
  descriptions.resize(s);
}

double 
ConfigurableProjectionExtension::Transformation::get_param (unsigned i) const
{
  return params[i].get_value();
}

void ConfigurableProjectionExtension::Transformation::set_param 
(unsigned i, double value)
{
  params[i].set_value(value);
}

double
ConfigurableProjectionExtension::Transformation::get_variance (unsigned i) 
const
{
  return params[i].get_variance();
}

void ConfigurableProjectionExtension::Transformation::set_variance
(unsigned i, double var)
{
  params[i].set_variance(var);
}

Estimate<double> ConfigurableProjectionExtension::Transformation::get_Estimate (unsigned i) const
{
  if (valid)
    return params[i];
  else
    return 0.0;
}

void ConfigurableProjectionExtension::Transformation::set_Estimate (unsigned i, const Estimate<double>& e)
{
  params[i] = e;
}

bool ConfigurableProjectionExtension::Transformation::get_valid () const
{
  return valid;
}

void ConfigurableProjectionExtension::Transformation::set_valid (bool flag)
{
  valid = flag;
}

//! Get the covariance matrix of the model paramters
vector<vector<double>> ConfigurableProjectionExtension::Transformation::get_covariance () const
{
  unsigned nparam = get_nparam();

  unsigned size = nparam * (nparam+1) / 2;
  if (size != covariance.size())
    throw Error (InvalidState,
		 "ConfigurableProjectionExtension::Transformation::get_covariance",
		 "covariance vector has incorrect length = %u (expect %u)",
		 covariance.size(), size);

  vector<vector<double> > matrix (nparam, vector<double>(nparam));

  unsigned count = 0;
  for (unsigned i=0; i<nparam; i++)
    for (unsigned j=i; j<nparam; j++)
    {
      matrix[i][j] = matrix[j][i] = covariance[count];
      count ++;
    }

  assert (count == covariance.size());

  return matrix;
}

//! Set the covariance matrix of the model paramters
void ConfigurableProjectionExtension::Transformation::set_covariance (const vector< vector<double> >& covar)
{
  unsigned nparam = get_nparam();

  assert (nparam == covar.size());

  covariance.resize( nparam * (nparam+1) / 2 );

  unsigned count = 0;
  for (unsigned i=0; i<nparam; i++)
  {
    assert (nparam == covar[i].size());
    for (unsigned j=i; j<nparam; j++)
    {
      covariance[count] = covar[i][j];
      count ++;
    }
  }

  assert (count == covariance.size());
}

//! Get the covariance matrix efficiently
void ConfigurableProjectionExtension::Transformation::get_covariance (vector<double>& covar) const
{
  covar = covariance;
}

//! Set the covariance matrix efficiently
void ConfigurableProjectionExtension::Transformation::set_covariance (const vector<double>& covar)
{
  covariance = covar;

  if (covar.size() == 0) {
    valid = false;
    return;
  }

  unsigned nparam = get_nparam();
  unsigned expect = nparam * (nparam+1) / 2;
  if (covar.size() != expect)
    throw Error (InvalidParam,
		 "ConfigurableProjectionExtension::Transformation::set_covariance",
		 "covariance vector length=%u != expected=%u=%u*(%u+1)/2",
		 covar.size(), expect, nparam, nparam);

  unsigned icovar = 0;

  // set the variance stored in the transformation
  for (unsigned i=0; i<nparam; i++)
    for (unsigned j=i; j<nparam; j++)
    {
      if (i==j)
      {
#ifdef _DEBUG
	cerr << j << " " << covar[icovar] << endl;
#endif
	set_variance (j,covar[icovar]);
      }
      icovar++;
    }

  if (icovar != covar.size())
    throw Error (InvalidState,
		 "ConfigurableProjectionExtension::Transformation::set_covariance",
		 "covariance vector length=%u != icovar=%u",
		 covar.size(), icovar);

}

//! Get the text interface 
TextInterface::Parser* ConfigurableProjectionExtension::get_interface()
{
  return new Interface( this );
}

