/***************************************************************************
 *
 *   Copyright (C) 2008 - 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RobustStepFinder.h"
#include "UnaryStatistic.h"

using namespace Pulsar;
using namespace std;

typedef std::vector<Calibration::SourceObservation> ObsVector;

double get_chi (const ObsVector& A, const ObsVector& B)
{
  auto Aptr = A.begin();
  auto Bptr = B.begin();

  vector<double> chi;
 
  while (Aptr != A.end() && Bptr != B.end())
  {
    while (Aptr->ichan < Bptr->ichan && Aptr != A.end())
      Aptr ++;

    if (Aptr == A.end())
      break;
	
    while (Bptr->ichan < Aptr->ichan && Bptr != B.end())
      Bptr ++;

    if (Bptr == B.end())
      break;

    const Stokes< Estimate<double> >& Astokes = Aptr->observation;
    const Stokes< Estimate<double> >& Bstokes = Bptr->observation;

    const unsigned npol = 4;
    for (unsigned ipol=0; ipol < npol; ipol++)
    {
      double diff = fabs(Astokes[ipol].val - Bstokes[ipol].val);
      double err = sqrt(Astokes[ipol].var + Bstokes[ipol].var);
      chi.push_back( diff / err);
    }
    
    Aptr++;
    Bptr++;
  }

  return median (chi);
}
	      
void RobustStepFinder::process (SystemCalibrator* calibrator)
{
  std::vector< ObsVector >& data = get_calibrator_data (calibrator);

  unsigned nsubint = data.size();

  unsigned ncross = nsubint * (nsubint - 1) / 2;

  vector<double> chisq (ncross);
  unsigned icross = 0;

  for (unsigned isub=0; isub < nsubint; isub++)
  {
    ObsVector& idata = data[isub];
    
    for (unsigned jsub=isub+1; jsub < nsubint; jsub++)
    {
      ObsVector& jdata = data[jsub];
      
      chisq[icross] = get_chi (idata, jdata);
      cerr << chisq[icross] << " ";

      icross ++;
    }

    cerr << endl;
  }

  cerr << "ncross=" << ncross << " icross=" << icross << endl;

  float threshold = 1.6;

  string indent = "i";

  icross = 0;
  for (unsigned isub=0; isub < nsubint; isub++)
  {
    cerr << indent;
    for (unsigned jsub=isub+1; jsub < nsubint; jsub++)
    {
      if (chisq[icross] < threshold)
	cerr << "1";
      else
	cerr << "0";

      icross ++;
    }
    cerr << endl;
    indent = " " + indent;
  }

}
