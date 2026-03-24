/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TextParameters.h"
#include "Pulsar/ParametersDM.h"

#include <assert.h>
#include <math.h>

using namespace std;
using namespace Pulsar;

template<typename T>
void compare (TextParameters& test, const std::string& key, T expected)
{
  T value = test.get<T> (key);

  if (value != expected)
  {
    Error error (InvalidState, "test_ParametersDM");
    error << "key=" << key << " got=" << value << " expected=" << expected;
    throw error;
  }
}

int main (int argc, char** argv) try
{
  // test Fortran-formatted float/double issue

  string text =
    "DM 1.23 \n"
    "DM1 1.2e-3 \n"
    "DM2 -4.5e-6 \n"
    "DM3 7.8e-9 \n";

  TextParameters test;
  test.set_text( text );

  ParametersDM params_dm;

  try
  {
    params_dm.set_parameters( &test );
    cerr << "test_ParametersDM: error, expected exception not thrown" << endl;
    return -1;
  }
  catch(const Error& error)
  {
    cerr << "test_ParametersDM: caught expected exception '" << error.get_message() << "'" << endl;
  }
  
  // add required DMEPOCH keyword
  text += "DMEPOCH 58000.0 \n";
  test.set_text( text );
  params_dm.set_parameters( &test );

  double dm = params_dm.get_dm();
  assert(dm == 1.23);

  vector<double> dm_coeffs;
  params_dm.get_dm_coeffs(dm_coeffs);

  assert(dm_coeffs.size() == 3);
  assert(dm_coeffs[0] == 1.2e-3);
  assert(dm_coeffs[1] == -4.5e-6);
  assert(dm_coeffs[2] == 7.8e-9);

  MJD expected_epoch(58000.0);
  MJD dm_epoch = params_dm.get_dm_epoch();
  assert(dm_epoch == expected_epoch);

  MJD one_year_later = expected_epoch + MJD(365.25);
  double dm_later = params_dm.get_dm(one_year_later);

  // default is TAYLOR series
  double expected_dm_later = 1.23
    + 1.2e-3
    + -4.5e-6 / 2.0
    + 7.8e-9 / 6.0;

  assert(fabs(dm_later - expected_dm_later) < 1e-12);

  // now test POLY series
  text += "DM_SERIES POLY \n";
  test.set_text( text );
  params_dm.set_parameters( &test );

  dm_later = params_dm.get_dm(one_year_later);

  expected_dm_later = 1.23
    + 1.2e-3
    + -4.5e-6
    + 7.8e-9;

  assert(fabs(dm_later - expected_dm_later) < 1e-12);

  // bugs/506 ensure that DIST_DM1 is not misinterpreted as DM1
  text =
    "DM 1.23 \n"
    "DIST_DM1 0.5 \n"
    "PEPOCH 58000.0 ";

  test.set_text( text );
  params_dm.set_parameters( &test );
  params_dm.get_dm_coeffs(dm_coeffs);
  assert(dm_coeffs.size() == 0);

  cerr << "test_ParametersDM: all tests passed" << endl;

  return 0;
}
catch (Error& error)
{
  cerr << "test_ParametersDM: " << error << endl;
  return -1;
}
