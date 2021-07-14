/***************************************************************************
 *
 *   Copyright (C) 2008 - 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RobustStepFinder.h"
#include "Pulsar/VariableBackend.h"
#include "UnaryStatistic.h"

using namespace Calibration;
using namespace Pulsar;
using namespace std;

typedef std::vector<Calibration::SourceObservation> ObsVector;

double get_chi (const ObsVector& A, const ObsVector& B, Index pol)
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

    // do all four Stokes at once ...
    unsigned npol = 4;
    unsigned ipol = 0;

    // ... or do a specific Stokes parameter
    if (!pol.get_integrate())
    {
      ipol = pol.get_value();
      npol = ipol + 1;
    }
    
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
  unsigned isub=0;
  unsigned erased = 0;

  while (isub < nsubint)
  {
    if (data[isub].size() == 0)
    {
#if _DEBUG
      cerr << "RobustStepFinder::process removing empty subint=" 
           << isub + erased << endl;
#endif
      data.erase (data.begin() + isub);
      nsubint --;
      erased ++;
    }
    else
      isub ++;
  }

  // compute chi for all Stokes at once
  Index pol;
  pol.set_integrate (true);

  isub=0;

  while (isub+1 < nsubint)
  {
    if (data[isub][0].source != Signal::PolnCal)
    {
      isub ++;
      continue;
    }
    
    unsigned jsub = isub+1;
      
    while (jsub < nsubint)
      if (data[jsub][0].source == Signal::PolnCal)
	break;
      else
	jsub ++;

    if (jsub >= nsubint)
      break;

    double chi = get_chi (data[isub], data[jsub], pol);

    // cerr << "isub=" << isub << " jsub=" << jsub << " chi=" << chi << endl;
    
    if (chi > step_threshold)
    {
      cerr << "RobustStepFinder::process isub=" << isub
	   << " chi=" << chi
	   << " > threshold=" << step_threshold << endl;

      if (calibrator->get_step_after_cal())
      {
        cerr << "RobustStepFinder::process adding step after "
             << data[isub][0].identifier << endl;

	calibrator->add_step (data[isub][0].epoch + 30.0,
			      new VariableBackend);
      }
      else
      {
        cerr << "RobustStepFinder::process adding step before "
             << data[jsub][0].identifier << endl;

	calibrator->add_step (data[jsub][0].epoch - 30.0,
			      new VariableBackend);
      }
    }

    isub = jsub;
  }
}
