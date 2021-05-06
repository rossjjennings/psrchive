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

  unsigned find_max (StokesCrossCovariance* stokes,
		     unsigned ipol,
		     unsigned jpol);

  vector<unsigned> ilags;
  vector<unsigned> ibins;
  vector<unsigned> jbins;
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
  arg->set_long_help
    ("  -i max[X|XY] -> search for longitude bin with maximum value \n"
     "     X  -> mean of Stokes X \n"
     "     XY -> absolute value of cross-covariance between Stokes X and Y \n"
     "  -i other -> longitude 1 equals longitude 2");
  
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

unsigned polindex (char code)
{
  switch (code) {
  case 'I':
    return 0;
  case 'Q':
    return 1;
  case 'U':
    return 2;
  case 'V':
    return 3;
  default:
    throw Error (InvalidState, "polindex", "'%c' not recognized", code);
  }
}

unsigned psr4th2txt::find_max (StokesCrossCovariance* stokes,
			       unsigned ipol,
			       unsigned jpol)
{
  unsigned ibin_max = 0;
  double val_max = -1.0;

  for (unsigned ilag=0; ilag < ilags.size(); ilag++)
  {
    unsigned k = ilags[ilag];
    if (k == 0)
      continue;

    // search for ibin over all jbins != ibin
    for (unsigned ibin=0; ibin < jbins.size(); ibin++)
    {
      unsigned i = jbins[ibin];

      for (unsigned jbin=0; jbin < jbins.size(); jbin++)
      {
	unsigned j = jbins[jbin];

	if (i == j)
	  continue;

        unsigned ib = i;
        unsigned jb = j;
        unsigned ip = ipol;
        unsigned jp = jpol;

        bool also_search_negative_lags = false;
        bool execute_at_least_once = true;

        while (execute_at_least_once)
        {
	  Matrix<4,4,double> ilag0;
	  Matrix<4,4,double> jlag0;
	  if (correlation_coefficient)
	  {
	    ilag0 = stokes->get_cross_covariance(ib, ib, 0);
	    jlag0 = stokes->get_cross_covariance(jb, jb, 0);
	  }

	  Matrix<4,4,double> c = stokes->get_cross_covariance(ib, jb, k);
	
	  double norm = 1.0;
	  if (correlation_coefficient)
	  {
	    double ivar = ilag0[ip][ip];
	    double jvar = jlag0[jp][jp];
	    norm = 1.0 / sqrt ( ivar * jvar );
	  }
	    
	  double val = fabs(c[ip][jp]) * norm;
	  if (val > val_max)
	  {
	    val_max = val;
	    ibin_max = i;
	  }

          if (!also_search_negative_lags)
            break;

          swap (ib, jb);
          swap (ip, jp);
        }
      }
    }
  }

  return ibin_max;
}



void psr4th2txt::process (Pulsar::Archive* archive)
{
  const CrossCovarianceMatrix* matrix = archive->get<CrossCovarianceMatrix>();
  if (!matrix)
    throw Error (InvalidState, "psr4th2txt::process",
		 "archive does not contain a CrossCovarianceMatrix extension");

  Reference::To<StokesCrossCovariance> stokes;

  stokes = new StokesCrossCovariance (matrix);

  ilags = indeces (stokes->get_nlag(), lag_indeces);
  jbins = indeces (stokes->get_nbin(), bin2_indeces);

  if (bin1_indeces.substr(0,3) == "max")
  {
    cerr << "searching for ibin with " << bin1_indeces << endl;
    ibins.resize (1);
    
    if (bin1_indeces.length () == 4)
    {
      unsigned ipol = polindex (bin1_indeces[3]);
      cerr << "find_max amp ipol=" << ipol << endl;
      ibins[0] = archive->get_Profile(0,ipol,0)->find_max_bin();
    }
    else if (bin1_indeces.length () == 5)
    {
      unsigned ipol = polindex (bin1_indeces[3]);
      unsigned jpol = polindex (bin1_indeces[4]);
      cerr << "find_max cov ipol=" << ipol << " jpol=" << jpol << endl;
      ibins[0] = find_max (stokes, ipol, jpol);
    }
  }
  else
    ibins = indeces (stokes->get_nbin(), bin1_indeces);
  
  unsigned npol = 4;
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

	    unsigned ip = ipols[ipol];
	    unsigned jp = jpols[jpol];

	    if (negative_lags)
	      swap (ip, jp);

	    double norm = 1.0;
	    if (correlation_coefficient)
	    {
	      double ivar = ilag0[ip][ip];
	      double jvar = jlag0[jp][jp];
	      norm = 1.0 / sqrt ( ivar * jvar );
	    }
	    
	    cout << " " << c[ip][jp] * norm;
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
