/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/StokesCrossCovariance.h"
#include "Pulsar/CrossCovarianceMatrix.h"

#include <algorithm>

using namespace std;
using namespace Pulsar;

//
//! An example of an application
//
class psr4th2txt : public Pulsar::Application
{
public:

  //! Default constructor
  psr4th2txt ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  string lag_indeces;
  string bin1_indeces;
  string bin2_indeces;
  string pol1_indeces;
  string pol2_indeces;

  bool correlation_coefficient;
  bool negative_lags;
  
  //! Add command line options
  void add_options (CommandLine::Menu&);
};


/*!

  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "psr4th2txt -h"

  This constructor makes use of

  - StandardOptions (-j -J etc.): an option set that provides standard
  preprocessing with the pulsar command language interpreter.

  - UnloadOptions (-e -m etc.): an option set that provides standard
  options for unloading data.

  This constructor also sets the default values of the attributes that
  are unique to the program.

*/

psr4th2txt::psr4th2txt ()
  : Application ("psr4th2txt", "extract data from Stokes cross-covariances")
{
  correlation_coefficient = false;
  negative_lags = false;
}


/*!

  Add application-specific command-line options.

*/

void psr4th2txt::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (correlation_coefficient, 'c');
  arg->set_help ("output the correlation coefficient");

  arg = menu.add (negative_lags, 'n');
  arg->set_help ("output values for the negative lags");

  arg = menu.add (lag_indeces, 't', "lag (turns)");
  arg->set_help ("select the sub-integration(s) to output");

  arg = menu.add (bin1_indeces, 'i', "longitude bin 1");
  arg->set_help ("select the longitude 1 bin(s) to output");

  arg = menu.add (bin2_indeces, 'j', "longitude bin 2");
  arg->set_help ("select the longitude 2 bin(s) to output");

  arg = menu.add (pol1_indeces, 'a', "polarization index a");
  arg->set_help ("select the polarization a index(es) to output");

  arg = menu.add (pol2_indeces, 'b', "polarization index b");
  arg->set_help ("select the polarization b index(es) to output");
}


vector<unsigned> indeces (unsigned n, const string& txt)
{
  vector<unsigned> result;
  if (txt == "")
  {
    result.resize (n);
    for (unsigned i=0; i<n; i++) result[i] = i;
  }
  else if (txt == "other")
  {
    result.resize (1, 0);
  }
  else
    TextInterface::parse_indeces (result, "[" + txt + "]", n);

  return result;
}

void psr4th2txt::process (Pulsar::Archive* archive)
{
  const CrossCovarianceMatrix* matrix = archive->get<CrossCovarianceMatrix>();
  if (!matrix)
    throw Error (InvalidState, "psr4th2txt::process",
		 "archive does not contain a CrossCovarianceMatrix extension");

  Reference::To<StokesCrossCovariance> stokes;

  stokes = new StokesCrossCovariance (matrix);

  unsigned npol = 4;
  vector<unsigned> ilags = indeces (stokes->get_nlag(), lag_indeces);
  vector<unsigned> ibins = indeces (stokes->get_nbin(), bin1_indeces);
  vector<unsigned> jbins = indeces (stokes->get_nbin(), bin2_indeces);
  vector<unsigned> ipols = indeces (npol, pol1_indeces);
  vector<unsigned> jpols = indeces (npol, pol2_indeces);

#if _DEBUG
  cerr << "subints=" << subints.size() << endl;
  cerr << "chans=" << chans.size() << endl;
  cerr << "bins=" << bins.size() << endl;
#endif

  bool ibin_other = bin1_indeces == "other";
  bool jbin_other = bin2_indeces == "other";
  
  bool ipol_other = pol1_indeces == "other";
  bool jpol_other = pol2_indeces == "other";

  string sign;
  
  if (negative_lags)
  {
    std::reverse (ilags.begin(), ilags.end());
    if (ilags.back() == 0)
      ilags.resize (ilags.size() - 1);
    
    sign = "-";
  }
  
  for (unsigned ilag=0; ilag < ilags.size(); ilag++)
  {
    for (unsigned ibin=0; ibin < ibins.size(); ibin++)
    {
      if (jbin_other)
	jbins[0] = ibins[ibin];

      for (unsigned jbin=0; jbin < jbins.size(); jbin++)
      {
	if (ibin_other)
	  ibins[0] = jbins[jbin];

	cout << sign << ilags[ilag] << " " << ibins[ibin] << " " << jbins[jbin];

	unsigned i = ibins[ibin];
	unsigned j = jbins[jbin];

	if (negative_lags)
	  swap (i, j);

	Matrix<4,4,double> ilag0;
	Matrix<4,4,double> jlag0;
	if (correlation_coefficient)
	{
	  ilag0 = stokes->get_cross_covariance(i, i, 0);
	  jlag0 = stokes->get_cross_covariance(j, j, 0);
	}

	Matrix<4,4,double> c = stokes->get_cross_covariance(i, j, ilags[ilag]);

	for (unsigned ipol=0; ipol < ipols.size(); ipol++)
	{
	  if (jpol_other)
	    jpols[0] = ipols[ipol];

	  for (unsigned jpol=0; jpol < jpols.size(); jpol++)
	  {
	    if (ipol_other)
	      ipols[0] = jpols[jpol];

	    unsigned i = ipols[ipol];
	    unsigned j = jpols[jpol];

	    if (negative_lags)
	      swap (i, j);

	    double norm = 1.0;
	    if (correlation_coefficient)
	    {
	      double ivar = ilag0[i][i];
	      double jvar = jlag0[j][j];
	      norm = 1.0 / sqrt ( ivar * jvar );
	    }
	    
	    cout << " " << c[i][j] * norm;
	  }
	}

	cout << endl;
      }
    }
    cout << endl;
  }
}

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  psr4th2txt program;
  return program.main (argc, argv);
}

