/***************************************************************************
 *
 *   Copyright (C) 2024 Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicResponse.h"
#include <stdlib.h> 

using namespace std;
using Pulsar::DynamicResponse;

//! Default Constructor 
DynamicResponse::DynamicResponse () 
 : Extension ("DynamicResponse")
{
  nchan = ntime = 0;
  npol = 1;
}

//! Copy constructor
DynamicResponse::DynamicResponse (const DynamicResponse& ext)
 : Extension ("DynamicResponse")
{
  operator = (ext);
}

//! Assignment operator
const DynamicResponse&
DynamicResponse::operator= (const DynamicResponse& extension)
{
  nchan = extension.nchan;
  npol = extension.npol;
  ntime = extension.ntime;

  minimum_epoch = extension.minimum_epoch;
  maximum_epoch = extension.maximum_epoch;

  minimum_frequency = extension.minimum_frequency;
  maximum_frequency = extension.maximum_frequency;

  response = extension.response;

  return *this;
}

//! Destructor
DynamicResponse::~DynamicResponse ()
{
}

void DynamicResponse::set_nchan (unsigned _nchan)
{
  nchan = _nchan;
}

unsigned DynamicResponse::get_nchan () const
{
  return nchan;
}

void DynamicResponse::set_npol (unsigned _npol)
{
  npol = _npol;
}

unsigned DynamicResponse::get_npol () const
{
  return npol;
}

void DynamicResponse::set_ntime (unsigned _ntime)
{
  ntime = _ntime;
}

unsigned DynamicResponse::get_ntime () const
{
  return ntime;
}

void DynamicResponse::resize_data ()
{
  response.resize ( nchan * ntime * npol );
}

void DynamicResponse::set_data(const std::vector<std::complex<double>>& data)
{
  auto expected_size = nchan * ntime * npol;
  if (data.size() != expected_size)
    throw Error(InvalidParam, "DynamicResponse::set_data", "data.size=%u does not equal expected size=%u", data.size(), expected_size);
  response = data;
}
