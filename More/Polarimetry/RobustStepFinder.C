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

// extern double sqr ( double x );

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

  Reference::To<const CoherencyMeasurement::Uncertainty> error;
  error = A.get_uncertainty();

  /* 
     WvS 21 July 2021

     I currently fail to understand why adding the uncertainty of B to the
     normalization causes the median chi to be close to 1/sqrt(2).  It makes
     me worry that the reduced chisq is also incorrect.
     
     error = error->clone();
     error->add (B.get_uncertainty());
  */
  
  Jones<double> diff = A.get_coherency() - B.get_coherency();
 
  Stokes< std::complex<double> > Schi = error->get_weighted_components (diff);
  
  for (unsigned ipol=0; ipol < npol; ipol++)
  {
#if _DEBUG
    cerr << ipol
	 << " " << fabs(Schi[ipol].real())
	 << " " << fabs(Schi[ipol].imag()) << endl;
#endif
    chi.push_back( fabs(Schi[ipol].real()) );
    if (Schi[ipol].imag() != 0.0)
      chi.push_back( fabs(Schi[ipol].imag()) );
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

  erase_empty (caldata);
  erase_empty (psrdata);

  unsigned nsubint = psrdata.size();
  
  // compute chi for all Stokes at once
  Index pol;
  pol.set_integrate (true);

  // count of consistent sub-integrations before this one
  vector<unsigned> before (nsubint, 0);
  // count of consistent sub-integrations after this one
  vector<unsigned> after (nsubint, 0);

  for (unsigned isub=0; isub < nsubint; isub++)
  {
    unsigned jmax = std::min (nsubint, isub+depth+1);
    
    for (unsigned jsub=isub+1; jsub < jmax; jsub++)
    {
      double chi = get_chi (psrdata[isub], psrdata[jsub], pol);

      // if isub is consistent with jsub ...
      if (chi < step_threshold)
      {
	// ... add to the count of consistent sub-integrations before
	// all sub-integrations after and including jsub
	for (unsigned ksub=jsub; ksub < jmax; ksub++)
	  before[ksub] ++;

	// ... add to the count of consistent sub-integrations after
	// all sub-integrations before jsub
	for (unsigned ksub=isub; ksub < jsub; ksub++)
	  after[ksub] ++;
      }
    }
  }

  vector<MJD> steps;

  bool step_after_cal = calibrator->get_step_after_cal();
  unsigned good = (depth * (depth+1)) / 2;
  
  for (unsigned isub=depth; isub+depth < nsubint; isub++)
  {
    if (before[isub] == good && after[isub] == 0)
    {
      cerr << "RobustStepFinder::process isub=" << isub 
	   << " before=" << before[isub]
	   << " after=" << after[isub] << endl;

      unsigned jsub = isub+1;
      
      MJD i_epoch = psrdata[isub][0].get_epoch();
      string i_id = psrdata[isub][0].get_identifier();
      
      MJD j_epoch = psrdata[jsub][0].get_epoch();
      string j_id = psrdata[jsub][0].get_identifier();

      cerr << "RobustStepFinder::process searching for CAL between \n\t"
	   << "MJD1=" << i_epoch << " id=" << i_id << "\n\t"
	   << "MJD2=" << j_epoch << " id=" << j_id << endl;

      vector<unsigned> cals_between;
      
      for (unsigned ical=0; ical < caldata.size(); ical++)
      {
	MJD cal_epoch = caldata[ical][0].epoch;
	if (cal_epoch > i_epoch && cal_epoch < j_epoch)
	{
	  string cal_id = caldata[ical][0].get_identifier();
	  cerr << "RobustStepFinder::process found CAL at \n\t"
	       << "MJD=" << cal_epoch << " id=" << cal_id << endl;

	  cals_between.push_back (ical);
	}
      }

      if (cals_between.size() == 0)
      {
	cerr << "RobustStepFinder::process no CAL found - no jump" << endl;
	continue;
      }

      unsigned ical = cals_between.front();
      unsigned jcal = cals_between.back();
      
      MJD ical_epoch = caldata[ical][0].epoch;
      string ical_id = caldata[ical][0].get_identifier();

      MJD jcal_epoch = caldata[jcal][0].epoch;
      string jcal_id = caldata[jcal][0].get_identifier();

      MJD step_epoch;
      
      if (step_after_cal)
      {
        cerr << "RobustStepFinder::process adding step after "
             << jcal_id << endl;
	step_epoch = jcal_epoch + 30.0;
      }
      else
      {
        cerr << "RobustStepFinder::process adding step before "
             << ical_id << endl;
	step_epoch = ical_epoch - 30.0;
      }

      cerr << "RobustStepFinder::process step epoch=" << step_epoch << endl;
      
      steps.push_back (step_epoch);
    }
  }

  if (!steps.size())
    return;
  
  unsigned nchan = calibrator->get_nchan();

  vector< vector<MJD> > mjds ( nchan, vector<MJD> (nsubint) );
  for (unsigned isub=0; isub < nsubint; isub++)
    for (unsigned jchan=0; jchan < psrdata[isub].size(); jchan++)
    {
      unsigned ichan = psrdata[isub][jchan].get_ichan();
      mjds[ichan][isub] = psrdata[isub][jchan].get_epoch();
    }

  Reference::Vector< SignalPath >& model = get_model (calibrator);
    
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    unsigned istep = 0;
    unsigned isub = 0;
    unsigned nbefore = 0;  // count of observations before step

    MJD zero (0.0);
    
    while (isub < nsubint && istep < steps.size())
    {
      if (mjds[ichan][isub] == zero)
      {
	isub++;
	// ignore
      }
      else if (mjds[ichan][isub] < steps[istep])
      {
	isub++;
	nbefore++;
      }
      else
      {
	// igore steps with no preceding obs
	if (nbefore)
	  model[ichan]->add_step (steps[istep], new VariableBackend);

	nbefore = 0;
	istep++;
      }
    }
  }
}
