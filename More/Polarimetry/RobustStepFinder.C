/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RobustStepFinder.h"
#include "Pulsar/VariableBackend.h"
#include "UnaryStatistic.h"

using namespace Calibration;
using namespace Pulsar;
using namespace std;

typedef vector<Calibration::SourceObservation> ObsVector;
typedef vector<Calibration::CoherencyMeasurementSet> SetVector;

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

void get_chi (vector<double>& chi,
	      const CoherencyMeasurement& A,
	      const CoherencyMeasurement& B, Index pol)
{
  // do all four Stokes at once ...
  unsigned npol = 4;
  unsigned ipol = 0;

  // ... or do a specific Stokes parameter
  if (!pol.get_integrate())
  {
    ipol = pol.get_value();
    npol = ipol + 1;
  }

  CoherencyMeasurement::Uncertainty* error = A.get_uncertainty()->clone();
  error->add (B.get_uncertainty());

  Jones<double> diff = A.get_coherency() - B.get_coherency();
 
  Stokes< std::complex<double> > Schi = error->get_weighted_components (diff);
  
  for (unsigned ipol=0; ipol < npol; ipol++)
  {
#if _DEBUG
    cerr << ipol
	 << " " << fabs(Schi[ipol].real())
	 << " " << fabs(Schi[ipol].imag()) << endl;
#endif
    chi.push_back( fabs(Schi[ipol].real()) + fabs(Schi[ipol].imag()) );
  }
}

void get_chi (vector<double>& chi,
	      const CoherencyMeasurementSet& A,
	      const CoherencyMeasurementSet& B, Index pol)
{
  auto Aptr = A.begin();
  auto Bptr = B.begin();

  while (Aptr != A.end() && Bptr != B.end())
  {
    while (Aptr->get_input_index() < Bptr->get_input_index() && Aptr != A.end())
      Aptr ++;

    if (Aptr == A.end())
      break;
	
    while (Bptr->get_input_index() < Aptr->get_input_index() && Bptr != B.end())
      Bptr ++;

    if (Bptr == B.end())
      break;

    get_chi (chi, *Aptr, *Bptr, pol);

    Aptr++;
    Bptr++;
  }
}
    
double get_chi (const SetVector& A, const SetVector& B, Index pol)
{
  auto Aptr = A.begin();
  auto Bptr = B.begin();

  vector<double> chi;
 
  while (Aptr != A.end() && Bptr != B.end())
  {
    while (Aptr->get_ichan() < Bptr->get_ichan() && Aptr != A.end())
      Aptr ++;

    if (Aptr == A.end())
      break;
	
    while (Bptr->get_ichan() < Aptr->get_ichan() && Bptr != B.end())
      Bptr ++;

    if (Bptr == B.end())
      break;

    get_chi (chi, *Aptr, *Bptr, pol);

    Aptr++;
    Bptr++;
  }

  return median (chi);
}


template<typename Container>
unsigned erase_empty (Container& container)
{
  unsigned erased = 0;
  unsigned idat = 0;
  
  while (idat < container.size())
  {
    if (container[idat].size() == 0)
    {
#if _DEBUG
      cerr << "RobustStepFinder::process removing empty sub-container at " 
           << idat + erased << endl;
#endif
      container.erase (container.begin() + idat);
      erased ++;
    }
    else
      idat ++;
  }

  return erased;
}

void RobustStepFinder::process (SystemCalibrator* calibrator)
{
  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);

  Reference::Vector< SignalPath >& model = get_model (calibrator);

  bool step_after_cal = calibrator->get_step_after_cal();
  unsigned nchan = calibrator->get_nchan();
  
  unsigned cal_erased = erase_empty (caldata);
  unsigned psr_erased = erase_empty (psrdata);

  unsigned nsubint = psrdata.size();
  unsigned isub=0;
  
  // compute chi for all Stokes at once
  Index pol;
  pol.set_integrate (true);

  isub=0;

  vector<bool> jump (nsubint, false);

  while (isub+1 < nsubint)
  {
    unsigned jsub = isub+1;
      
    double chi = get_chi (psrdata[isub], psrdata[jsub], pol);

    cerr << "isub=" << isub << " jsub=" << jsub << " chi=" << chi << endl;
    
    if (chi > step_threshold)
    {
      cerr << "RobustStepFinder::process isub=" << isub
	   << " chi=" << chi
	   << " > threshold=" << step_threshold << endl;

      if (step_after_cal)
      {
        cerr << "RobustStepFinder::process adding step after "
             << psrdata[isub][0].get_identifier() << endl;
	jump[isub] = true;
      }
      else
      {
        cerr << "RobustStepFinder::process adding step before "
             << psrdata[jsub][0].get_identifier() << endl;
	jump[jsub] = true;
      }
    }

    isub ++;
  }

  bool jump_needed = false;
  for (unsigned isub=0; isub < nsubint; isub++)
    if (jump[isub])
      jump_needed = true;

  if (jump_needed)
    return;

  // NOW A SECOND LOOP OVER CHANNELS INSERTING JUMPS ONLY WHERE DATA EXISTS
  // TO CONSTRAIN IT

  // REALLY NEED PULSAR DATA TO DETERMINE WHERE JUMPS SHOULD BE PLACED
  
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    // set if this channel is observed in this subint
    vector<bool> observed (nsubint, false);
    unsigned observed_total = 0;
    
    for (unsigned isub=0; isub < nsubint; isub++)
    {
      unsigned jchan = 0;
      for (; jchan < psrdata[isub].size(); jchan++)
	if (psrdata[isub][jchan].get_ichan() == ichan)
	{
	  observed[isub] = true;
	  observed_total ++;
	  break;
	}
    }

    if (observed_total == 0)
      continue;

#if 0
    for (unsigned isub=0; isub < nsubint; isub++)
    {
      if (jump[isub] && observed_so_far)
      {
	unsigned ichan = 
	
	MJD step_epoch;
      
	if (step_after_cal)
	  step_epoch = caldata[isub][0].epoch + 30.0;
	else
	  step_epoch = caldata[jsub][0].epoch - 30.0;

      	model[ichan]->add_step (step_epoch, new VariableBackend);
      }
    }
    isub = jsub;
#endif
  }
}
