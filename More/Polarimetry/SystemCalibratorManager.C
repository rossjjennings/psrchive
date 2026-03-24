/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SystemCalibratorManager.h"
#include "Pulsar/Archive.h"

#include <cassert>

using namespace Pulsar;
using namespace std;

  /* TO DO: think about how to manage different channels flagged as
     invalid for different pulsars */

SystemCalibratorManager::SystemCalibratorManager ()
{
}

void SystemCalibratorManager::manage (SystemCalibrator* newcal)
{
  // protect against multiple/ambiguous matches
  for (auto & cal: calibrator)
    if (cal->get_name() == newcal->get_name())
      throw Error (InvalidParam, "SystemCalibratorManager::manage",
                  "SystemCalibrator for " + cal->get_name() + " already added");

  if (sharing_setup)
    newcal->share(calibrator[0]);

  calibrator.push_back (newcal);
}

unsigned SystemCalibratorManager::get_ncalibrator ()
{
  return calibrator.size ();
}

SystemCalibrator* SystemCalibratorManager::get_calibrator (unsigned i)
{
  return calibrator[i];
}

SystemCalibrator* SystemCalibratorManager::get_model ()
{
  return calibrator[0];
}

SystemCalibrator* SystemCalibratorManager::get_calibrator (const Archive* data)
{
  bool throw_exception = false;

  if (calibrator.size() == 0)
    throw Error (InvalidParam, "SystemCalibratorManager::get_calibrator", "no calibrators added");

  for (auto cal: calibrator)
  {
    if (fscrunch_data_to_model)
      cal->match_check_nchan = false;
	
    bool match = cal->match (data, throw_exception);

    cal->match_check_nchan = true;

    if (match)
      return cal;
    else if (Archive::verbose > 1)
      cerr << "SystemCalibratorManager::get_calibrator mismatch reason=" << cal->get_mismatch_reason() << endl;
  }
  
  throw Error (InvalidState, "SystemCalibratorManager::get_calibrator",
	       "no model matches file=" + data->get_filename() + " with source=" + data->get_source()); 
}

void SystemCalibratorManager::solve ()
{
  assert (calibrator.size() > 0);
  
  for (auto cal: calibrator)
  {
    if (!cal->has_calibrator())
      throw Error (InvalidState, "SystemCalibratorManager::solve",
		   "SystemCalibrator for " + cal->get_name() + " has no data");
    
    if (Archive::verbose > 1)
      cerr << "SystemCalibratorManager::solve preparing name="
          << cal->get_calibrator()->get_source () << endl;
    
    cal->solve_prepare ();
  }

  if (Archive::verbose > 1)
    cerr << "SystemCalibratorManager::solve calling SystemCalibrator::solve" << endl;
  
  get_model()->solve ();
}

void SystemCalibratorManager::preprocess (Archive* data)
{
  SystemCalibrator* model = get_calibrator (data);
  
  if (fscrunch_data_to_model && model->get_nchan() < data->get_nchan())
  {
    cerr << "SystemCalibratorManager::preprocess frequency integrating data"
      " (nchan=" << data->get_nchan() << ") to match model"
      " (nchan=" << model->get_nchan() << ")" << endl;
    data->fscrunch_to_nchan (model->get_nchan());
  }

  model->preprocess (data);
}

void SystemCalibratorManager::add_observation (const Archive* data)
{
  if (Archive::verbose > 1)
  {
    cerr << "SystemCalibratorManager::add_observation" << endl;
    for (auto & cal: calibrator)
      cerr << "\tname=" << cal->get_name() << " nchan=" << cal->get_nchan() << endl;
  }

  SystemCalibrator* model = get_calibrator (data);
  model->add_observation (data);

  if (!sharing_setup)
  {
    if (Archive::verbose > 1)
      cerr << "SystemCalibratorManager::add_observation setup sharing" << endl;

    setup_sharing (model);
  }
}

void SystemCalibratorManager::setup_sharing (SystemCalibrator* model)
{
  if (model->get_nchan() == 0)
    return;

  // if the first SystemCalibrator does not yet have data, swap with model that does
  if (calibrator[0]->get_nchan() == 0)
  {
    if (Archive::verbose > 1)
    {
      cerr << "SystemCalibratorManager::setup_sharing before swap:" << endl;
      for (auto & cal: calibrator)
        cerr << "\t" << cal->get_name() << endl;
    }

    for (auto & cal: calibrator)
    {
      if (cal.ptr() == model)
      {
        if (Archive::verbose > 1)
          cerr << "SystemCalibratorManager::setup_sharing"
                  " swapping " << model->get_name() << " with " << calibrator[0]->get_name() << endl;
        std::swap(cal,calibrator[0]);
      }
    }

    if (Archive::verbose > 1)
    {
      cerr << "SystemCalibratorManager::setup_sharing after swap:" << endl;
      for (auto & cal: calibrator)
        cerr << "\t" << cal->get_name() << endl;
    }
  }

  for (unsigned i=1; i < calibrator.size(); i++)
    calibrator[i]->share (calibrator[0]);

  sharing_setup = true;
}

//! Calibrate the the given archive using the current state of the model
void SystemCalibratorManager::precalibrate (Archive* data)
{
  SystemCalibrator* fiducial = get_model();
  SystemCalibrator* model = get_calibrator (data);

  unsigned nchan = fiducial->get_nchan();
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!fiducial->get_valid(ichan) && model->get_valid(ichan))
    {
      if (Archive::verbose > 1)
        cerr << "SystemCalibratorManager::precalibrate invalid ichan=" << ichan << " in fiducial model" << endl;
      model->set_valid (ichan, false, "invalid in fiducial model");
    }
  }

  if (Archive::verbose > 1)
    cerr << "SystemCalibratorManager::precalibrate calling SystemCalibrator::precalibrate" << endl;
  model->precalibrate (data);
}

