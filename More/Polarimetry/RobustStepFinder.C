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

// #define _DEBUG 1
#include "debug.h"

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
      DEBUG("RobustStepFinder::process removing empty sub-container at " << idat+erased);
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
  remove_empty (caldata);

  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  remove_empty (psrdata);

  remove_outliers ();
  remove_extra_calibrators ();
  insert_steps ();
}

void RobustStepFinder::remove_extra_calibrators ()
{
  cerr << "RobustStepFinder::remove_extra_calibrators" << endl;

  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  vector< ObsVector >& caldata = get_calibrator_data (calibrator);

  unsigned ical = 0;

  vector<unsigned> before;
  vector<MJD> epochs;
  
  for (unsigned ipsr=0; ipsr < psrdata.size(); ipsr++)
  {
    MJD psr_epoch = psrdata[ipsr][0].get_epoch();

    while (ical < caldata.size() && caldata[ical][0].get_epoch() < psr_epoch)
    {
      before.push_back (ical);
      epochs.push_back (caldata[ical][0].get_epoch());
      ical ++;
    }
    
    if (before.size())
    {
#if _DEBUG
      cerr << "RobustStepFinder::remove_extra_calibrators testing " 
           << before.size() << " calibrator epochs" << endl;
#endif

      unsigned removed = 0;
      unsigned ibefore = 0;
      double max_diff = 5.0; // five minutes (should be parameterized)

      while (ibefore+1 < epochs.size())
      {
        double diff = (epochs[ibefore+1] - epochs[ibefore]).in_minutes ();
	if (diff > max_diff)
        {
          unsigned before_ical = before[ibefore] - removed;
          unsigned after_ical = before[ibefore+1] - removed;

	  cerr << "RobustStepFinder::remove_extra_calibrators gap="
	       << diff << " minutes between \n\t"
               << caldata[before_ical][0].get_identifier() << " and\n\t"
               << caldata[after_ical][0].get_identifier() << endl;

	  if (calibrator->get_step_after_cal())
	  {
	    cerr << "RobustStepFinder::remove_extra_calibrators keeping "
		 << ibefore+1 << " sub-integrations before gap" << endl;

	    unsigned ical_offset = before[ibefore+1] - removed;

	    while (ibefore+1 < epochs.size())
	    {
	      epochs.erase (epochs.begin() + ibefore+1);

	      cerr << "RobustStepFinder::remove_extra_calibrators removing \n\t"
		   << caldata[ical_offset][0].get_identifier() << endl;

	      caldata.erase (caldata.begin() + ical_offset);
              removed ++;
	    }
	  }
	  else
	  {
	    cerr << "RobustStepFinder::remove_extra_calibrators keeping "
		 << epochs.size() - (ibefore+1)
		 << " sub-integrations after gap" << endl;

            unsigned ical_offset = before[0] - removed;

	    for (unsigned i=0; i < ibefore+1; i++)
	    {
	      epochs.erase (epochs.begin());
              before.erase (before.begin());

	      cerr << "RobustStepFinder::remove_extra_calibrators removing \n\t"
		   << caldata[ical_offset][0].get_identifier() << endl;
	      
	      caldata.erase (caldata.begin() + ical_offset);
              removed ++;
	    }

	    ibefore = 0;
	  }
	}
        else
          ibefore ++;

      } // end while

      before.clear();
      epochs.clear();
    }
  } 
  DEBUG ("RobustStepFinder::remove_extra_calibrators done");
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
  DEBUG ("RobustStepFinder::remove_outliers");

  bool wedge = false;

  // search for outliers using all four Stokes parameters
  compare = all_four ();

  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  count_consistent (psrdata, psr_before, psr_after, wedge);
  remove_inconsistent (psrdata, psr_before, psr_after);
  
  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  count_consistent (caldata, cal_before, cal_after, wedge);
  remove_inconsistent (caldata, cal_before, cal_after);

  DEBUG ("RobustStepFinder::remove_outliers done");
}

template <class Container, class Element>
  bool found (const Container& container, const Element& element)
{
  return std::find (container.begin(),
		    container.end(), element) != container.end();
}
	    
void RobustStepFinder::insert_steps ()
{
  Reference::To<VariableBackend> backend = new VariableBackend;
  
  // search for steps using all four Stokes parameters
  compare = all_four ();

  bool wedge = true;

  cerr << "RobustStepFinder::insert_steps searching pulsar data" << endl;
  
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  count_consistent (psrdata, psr_before, psr_after, wedge);

  vector<MJD> psr_steps;
  find_steps_pulsar (psr_steps);

  if (psr_steps.size())
    insert_steps (psr_steps, backend);

  bool search_only_differential_phase = true;

  if (search_only_differential_phase)
  {
    // search for steps in differential phase only
    compare.resize (2);
    compare[0] = 2;
    compare[1] = 3;
  }

  vector< ObsVector >& caldata = get_calibrator_data (calibrator);

  if (caldata.size() == 0)
    return;

  cerr << "RobustStepFinder::insert_steps searching calibrator data" << endl;

  count_consistent (caldata, cal_before, cal_after, wedge);

  vector<MJD> cal_steps;
  find_steps_calibrator (cal_steps);

  unsigned ical=0;
  while (ical < cal_steps.size())
  {
    if (found (psr_steps, cal_steps[ical]))
    {
      cerr << "RobustStepFinder::insert_steps"
	" remove CAL step already in PSR steps at MJD="
	   << cal_steps[ical] << endl;

      cal_steps.erase (cal_steps.begin() + ical);
    }
    else
      ical ++;
  }
  
  if (!cal_steps.size())
    return;

  bool fit_only_differential_phase = false;

  if (fit_only_differential_phase)
  {
    // disable variations in gain and differential gain
    backend->set_infit (0, false);
    backend->set_infit (1, false);
  }

  insert_steps (cal_steps, backend);
}

double get_chi (const ObsVector& A, const ObsVector& B, vector<unsigned>& pol)
{
  DEBUG ("get_chi ObsVector");

  auto Aptr = A.begin();
  auto Bptr = B.begin();

  vector<double> chi;
 
  while (Aptr != A.end() && Bptr != B.end())
  {
    while (Aptr != A.end() && (Aptr->ichan < Bptr->ichan))
      Aptr ++;

    if (Aptr == A.end())
      break;
	
    while (Bptr != B.end() && (Bptr->ichan < Aptr->ichan))
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

  if (chi.size() == 0)
  {
    DEBUG ("get_chi ObsVector no data");
    return 0;
  }

  DEBUG ("get_chi ObsVector ok");

  return median (chi);
}

// extern double sqr ( double x );

void get_chi (vector<double>& chi,
	      const CoherencyMeasurement& A,
	      const CoherencyMeasurement& B, vector<unsigned>& pol)
{
  DEBUG ("get_chi CoherencyMeasurement");

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
    chi.push_back( fabs(Schi[ipol].real()) );
    if (Schi[ipol].imag() != 0.0)
      chi.push_back( fabs(Schi[ipol].imag()) );
  }

  DEBUG ("get_chi CoherencyMeasurement ok");
}

void get_chi (vector<double>& chi,
	      const CoherencyMeasurementSet& A,
	      const CoherencyMeasurementSet& B, vector<unsigned>& pol)
{
  auto Aptr = A.begin();
  auto Bptr = B.begin();

  while (Aptr != A.end() && Bptr != B.end())
  {
    while (Aptr != A.end() && (Aptr->get_input_index() < Bptr->get_input_index()))
      Aptr ++;

    if (Aptr == A.end())
      break;
	
    while (Bptr != B.end() && (Bptr->get_input_index() < Aptr->get_input_index()))
      Bptr ++;

    if (Bptr == B.end())
      break;

    DEBUG ("get_chi CoherencyMeasurementSet get_chi");
    get_chi (chi, *Aptr, *Bptr, pol);
    DEBUG ("get_chi CoherencyMeasurementSet get_chi ok");

    Aptr++;
    Bptr++;
  }
}
    
double get_chi (const SetVector& A, const SetVector& B, vector<unsigned>& pol)
{
  auto Aptr = A.begin();
  auto Bptr = B.begin();

  vector<double> chi;

  DEBUG ("get_chi SetVector");
 
  while (Aptr != A.end() && Bptr != B.end())
  {
    while (Aptr != A.end() && (Aptr->get_ichan() < Bptr->get_ichan()))
      Aptr ++;

    if (Aptr == A.end())
      break;
	
    while (Bptr != B.end() && (Bptr->get_ichan() < Aptr->get_ichan()))
      Bptr ++;

    if (Bptr == B.end())
      break;

    DEBUG ("get_chi SetVector get_chi");
    get_chi (chi, *Aptr, *Bptr, pol);
    DEBUG ("get_chi SetVector get_chi ok");

    Aptr++;
    Bptr++;
  }

  if (chi.size() == 0)
  {
    DEBUG ("get_chi SetVector no data");
    return 0.0;
  }

  return median (chi);
}

template<typename Container>
void RobustStepFinder::count_consistent (const Container& container,
					 vector<unsigned>& before,
					 vector<unsigned>& after,
					 bool wedge)
{
  DEBUG ("RobustStepFinder::count_consistent");

  before.clear();
  after.clear();
  
  unsigned nsubint = container.size();

  if (nsubint == 0)
    return;
 
  before.resize (nsubint, 0);
  after.resize (nsubint, 0);

  for (unsigned isub=0; isub < nsubint; isub++)
  {
    unsigned jmax = std::min (nsubint, isub+depth+1);
 
    for (unsigned jsub=isub+1; jsub < jmax; jsub++)
    {
      DEBUG ("RobustStepFinder::count_consistent get_chi");
      double chi = get_chi (container[isub], container[jsub], compare);
      DEBUG ("RobustStepFinder::count_consistent chi=" << chi);

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
  DEBUG ("RobustStepFinder::count_consistent done");
}

template<typename Container>
void RobustStepFinder::remove_inconsistent (Container& container,
					    vector<unsigned>& before,
					    vector<unsigned>& after)
{
  DEBUG ("RobustStepFinder::remove_inconsistent");

  unsigned nsubint = container.size();

  if (nsubint == 0)
    return;

  assert (before.size() == nsubint);
  assert (after.size() == nsubint);

  unsigned min_threshold = depth - 1;

  unsigned before_min = 0;
  unsigned after_min = min_threshold;

  unsigned isub=0;
  while (isub < container.size())
  {
#if _DEBUG
    cerr << isub << " before=" << before[isub] << " after=" << after[isub]
         << " min before=" << before_min << " after=" << after_min << endl;
#endif

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

  DEBUG ("RobustStepFinder::remove_inconsistent done");
}
      
void RobustStepFinder::find_steps_pulsar (vector<MJD>& steps)
{
  vector<unsigned> step_sub;
  find_steps (step_sub, psr_before, psr_after);

  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  add_steps (steps, step_sub, psrdata, true);
}

void RobustStepFinder::find_steps_calibrator (vector<MJD>& steps)
{
  vector<unsigned> step_sub;
  find_steps (step_sub, cal_before, cal_after);

  vector< ObsVector >& caldata = get_calibrator_data (calibrator);
  add_steps (steps, step_sub, caldata, false);
}

template<class Container>
void RobustStepFinder::add_steps (vector<MJD>& steps,
				  vector<unsigned>& step_sub,
				  Container& data,
				  bool align)
{
  for (unsigned ksub=0; ksub<step_sub.size(); ksub++)
  {
    unsigned isub = step_sub[ksub];
    unsigned jsub = isub+1;
      
    MJD i_epoch = data[isub][0].get_epoch();
    string i_id = data[isub][0].get_identifier();
      
    MJD j_epoch = data[jsub][0].get_epoch();
    string j_id = data[jsub][0].get_identifier();

    cerr << "RobustStepFinder::add_steps step detected between \n\t"
	 << "MJD1=" << i_epoch << " id=" << i_id << "\n\t"
	 << "MJD2=" << j_epoch << " id=" << j_id << endl;

    if (align)
    {
      bool aligned = align_to_cal (i_epoch, i_id, j_epoch, j_id);
      if (!aligned)
	continue;
    }

    MJD step_epoch;
    
    if (calibrator->get_step_after_cal())
    {
      cerr << "RobustStepFinder::add_steps adding step after "
	   << i_id << endl;
      step_epoch = i_epoch + 30.0;
    }
    else
    {
      cerr << "RobustStepFinder::add_steps adding step before "
	   << j_id << endl;
      step_epoch = j_epoch - 30.0;
    }
    
#if _DEBUG
    cerr << "RobustStepFinder::add_steps step epoch="
	 << step_epoch << endl;
#endif
    
    steps.push_back (step_epoch);
  }
}

bool RobustStepFinder::align_to_cal (MJD& i_epoch, string& i_id,
				     MJD& j_epoch, string& j_id)
{
  cerr << "RobustStepFinder::add_steps searching for CAL between \n\t"
       << "MJD1=" << i_epoch << " id=" << i_id << "\n\t"
       << "MJD2=" << j_epoch << " id=" << j_id << endl;

  vector< ObsVector >& caldata = get_calibrator_data (calibrator);

  vector<unsigned> between;
    
  for (unsigned ical=0; ical < caldata.size(); ical++)
  {
    MJD cal_epoch = caldata[ical][0].get_epoch();
    if (cal_epoch > i_epoch && cal_epoch < j_epoch)
    {
      string cal_id = caldata[ical][0].get_identifier();
      cerr << "RobustStepFinder::align found CAL at \n\t"
	   << "MJD=" << cal_epoch << " id=" << cal_id << endl;
	  
      between.push_back (ical);
    }
  }
    
  if (between.size() == 0)
  {
    cerr << "RobustStepFinder::align no CAL found - no jump" << endl;
    return false;
  }
    
  unsigned ical = between.back();
  i_epoch = caldata[ical][0].get_epoch();
  i_id = caldata[ical][0].get_identifier();
      
  unsigned jcal = between.front();
  j_epoch = caldata[jcal][0].get_epoch();
  j_id = caldata[jcal][0].get_identifier();

  return true;
}




void RobustStepFinder::find_steps (vector<unsigned>& steps,
				   const vector<unsigned>& before,
				   const vector<unsigned>& after)
{
  unsigned nsubint = before.size();

  for (unsigned isub=1; isub+2 < nsubint; isub++)
  {

#if _DEBUG
    cerr << isub << " before=" << before[isub] << " after=" << after[isub] << endl;
#endif

    if (after[isub] < 2)
    {
      cerr << "RobustStepFinder::find_steps isub=" << isub 
	   << " before=" << before[isub]
	   << " after=" << after[isub] << endl;

      steps.push_back (isub);
    }
  }
}

void RobustStepFinder::insert_steps (vector<MJD>& steps, VariableBackend* xform)
{
  vector< SetVector >& psrdata = get_pulsar_data (calibrator);
  unsigned nsubint = psrdata.size();

  if (nsubint == 0)
    return;

  unsigned nchan = calibrator->get_nchan();

  // construct a two-dimensional array of epochs (ichan, isubint)

  vector< vector<MJD> > mjds ( nchan, vector<MJD> (nsubint) );
  for (unsigned isub=0; isub < nsubint; isub++)
    for (unsigned jchan=0; jchan < psrdata[isub].size(); jchan++)
    {
      unsigned ichan = psrdata[isub][jchan].get_ichan();
      mjds[ichan][isub] = psrdata[isub][jchan].get_epoch();
    }

  // get_model returns SystemCalibrator::model
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
	// ignore steps with no preceding obs
	if (nbefore)
	  model[ichan]->add_step (steps[istep], xform->clone());

	nbefore = 0;
	istep++;
      }
    }
  }
}

