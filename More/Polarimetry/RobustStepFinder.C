/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RobustStepFinder.h"
#include "Pulsar/VariableBackend.h"
#include "UnaryStatistic.h"

#include <cassert>

using namespace Calibration;
using namespace Pulsar;
using namespace std;

typedef vector<Calibration::SourceObservation> ObsVector;
typedef vector<Calibration::CoherencyMeasurementSet> SetVector;

template<typename Container>
unsigned remove_empty (Container& container)
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

void RobustStepFinder::process (SystemCalibrator* _calibrator)
{
  calibrator = _calibrator;

  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);

  remove_empty (caldata);
  remove_empty (psrdata);
  
  remove_outliers ();
  insert_steps ();
}

double get_chi (const ObsVector& A, const ObsVector& B, vector<unsigned>& pol)
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

    for (unsigned jpol=0; jpol < pol.size(); jpol++)
    {
      unsigned ipol = pol[jpol];
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
	      const CoherencyMeasurement& B, vector<unsigned>& pol)
{
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

  for (unsigned jpol=0; jpol < pol.size(); jpol++)
  {
    unsigned ipol = pol[jpol];
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
	      const CoherencyMeasurementSet& B, vector<unsigned>& pol)
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
    
double get_chi (const SetVector& A, const SetVector& B, vector<unsigned>& pol)
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
void RobustStepFinder::count_consistent (const Container& container,
					 vector<unsigned>& before,
					 vector<unsigned>& after,
					 bool wedge)
{
  before.clear();
  after.clear();
  
  unsigned nsubint = container.size();
  
  before.resize (nsubint, 0);
  after.resize (nsubint, 0);

  for (unsigned isub=0; isub < nsubint; isub++)
  {
    unsigned jmax = std::min (nsubint, isub+depth+1);
    
    for (unsigned jsub=isub+1; jsub < jmax; jsub++)
    {
      double chi = get_chi (container[isub], container[jsub], compare);

      // if isub is consistent with jsub ...
      if (chi < step_threshold)
      {
	if (wedge)
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
	else
	{
	  after[isub] ++;
	  before[jsub] ++;
	}
      }
    }
  }
}

template<typename Container>
void RobustStepFinder::remove_inconsistent (Container& container,
					    vector<unsigned>& before,
					    vector<unsigned>& after)
{
  unsigned nsubint = container.size();

  assert (before.size() == nsubint);
  assert (after.size() == nsubint);

#if _DEBUG 
  cerr << "RobustStepFinder::remove_inconsistent" << endl;
#endif

  unsigned min_threshold = depth - 1;

  unsigned before_min = 0;
  unsigned after_min = min_threshold;

  unsigned isub=0;
  while (isub < container.size())
  {
    cerr << isub << " before=" << before[isub] << " after=" << after[isub]
         << " min before=" << before_min << " after=" << after_min << endl;

    if (before[isub] < before_min && after[isub] < after_min)
    {
      cerr << "RobustStepFinder::remove_inconsistent removing outlier \n\t"
	   << container[isub][0].get_identifier() << endl;
      
      container.erase (container.begin()+isub);
      before.erase (before.begin()+isub);
      after.erase (after.begin()+isub);
    }
    else
      isub++;

    if (before_min < min_threshold)
      before_min ++;

    if (isub + min_threshold >= container.size() && after_min > 0)
      after_min --;
  }
}

vector<unsigned> all_four ()
{
  vector<unsigned> pol (4);
  for (unsigned i=0; i<4; i++)
    pol[i] = i;
  return pol;
}

void RobustStepFinder::remove_outliers ()
{
  bool wedge = false;

  // search for outliers using all four Stokes parameters
  compare = all_four ();

  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  count_consistent (psrdata, psr_before, psr_after, wedge);
  remove_inconsistent (psrdata, psr_before, psr_after);
  
  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  count_consistent (caldata, cal_before, cal_after, wedge);
  remove_inconsistent (caldata, cal_before, cal_after);
}

void RobustStepFinder::insert_steps ()
{
  Reference::To<VariableBackend> xform = new VariableBackend;
  
  // search for steps using all four Stokes parameters
  compare = all_four ();
  insert_steps (xform);

  // search for steps in differential phase only
  compare.resize (2);
  compare[0] = 2;
  compare[1] = 3;

  // disable fits in gain and differential gain
  xform->set_infit (0, false);
  xform->set_infit (1, false);
  insert_steps (xform);
}

void RobustStepFinder::insert_steps (VariableBackend* backend)
{
  bool wedge = true;
  
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  count_consistent (psrdata, psr_before, psr_after, wedge);

  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  count_consistent (caldata, cal_before, cal_after, wedge);

  vector<MJD> steps;
  find_steps (steps);

  if (!steps.size())
    return;

  insert_steps (steps, backend);
}

void RobustStepFinder::find_steps (vector<MJD>& steps)
{
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  
  unsigned nsubint = psrdata.size();
  
  bool step_after_cal = calibrator->get_step_after_cal();
  unsigned good = (depth * (depth+1)) / 2;
  
  for (unsigned isub=depth; isub+depth < nsubint; isub++)
  {
    if (psr_before[isub] == good && psr_after[isub] == 0)
    {
      cerr << "RobustStepFinder::process isub=" << isub 
	   << " before=" << psr_before[isub]
	   << " after=" << psr_after[isub] << endl;

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

      MJD step_epoch;
      
      if (step_after_cal)
      {
	unsigned ical = cals_between.back();
	MJD cal_epoch = caldata[ical][0].epoch;
	string cal_id = caldata[ical][0].get_identifier();
	
	cerr << "RobustStepFinder::process adding step after "
             << cal_id << endl;
      
	step_epoch = cal_epoch + 30.0;
      }
      else
      {
	unsigned ical = cals_between.front();
	MJD cal_epoch = caldata[ical][0].epoch;
	string cal_id = caldata[ical][0].get_identifier();

        cerr << "RobustStepFinder::process adding step before "
             << cal_id << endl;
	step_epoch = cal_epoch - 30.0;
      }

#if _DEBUG
      cerr << "RobustStepFinder::process step epoch=" << step_epoch << endl;
#endif
      
      steps.push_back (step_epoch);
    }
  }
}

void RobustStepFinder::insert_steps (vector<MJD>& steps, VariableBackend* xform)
{
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  unsigned nsubint = psrdata.size();

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
	  model[ichan]->add_step (steps[istep], xform->clone());

	nbefore = 0;
	istep++;
      }
    }
  }
}

