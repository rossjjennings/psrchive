#include "Calibration/MeanSingleAxis.h"
#include "Calibration/SingleAxis.h"

using namespace std;

void Calibration::MeanSingleAxis::update (MEAL::Complex2* model)
{
  if (MEAL::Function::very_verbose)
    cerr << "Calibration::MeanSingleAxis::update (MEAL::Complex2*)" << endl;

  SingleAxis* single_axis = dynamic_cast<SingleAxis*>(model);
  if (!single_axis)
    throw Error (InvalidParam, "Calibration::MeanSingleAxis::update",
		 "Complex2 model is not a SingleAxis");

  update (single_axis);
}

void Calibration::MeanSingleAxis::update (SingleAxis* single_axis)
{
  if (MEAL::Function::very_verbose)
    cerr << "Calibration::MeanSingleAxis::update (SingleAxis*)"
      "\n  gain=" << mean_gain.get_Estimate() <<
      "\n  diff_gain=" << mean_diff_gain.get_Estimate() <<
      "\n  diff_phase=" << mean_diff_phase.get_Estimate() << endl;

  single_axis->set_gain       (mean_gain.get_Estimate());
  single_axis->set_diff_gain  (mean_diff_gain.get_Estimate());
  single_axis->set_diff_phase (mean_diff_phase.get_Estimate());
}

void Calibration::MeanSingleAxis::integrate (const MEAL::Complex2* model)
{
  const SingleAxis* single_axis = dynamic_cast<const SingleAxis*>(model);
  if (!single_axis)
    throw Error (InvalidParam, "Calibration::MeanSingleAxis::integrate",
		 "Complex2 model is not a SingleAxis");
  
  integrate (single_axis);
}

void Calibration::MeanSingleAxis::integrate (const SingleAxis* single_axis)
{
  mean_gain       += single_axis->get_gain ();
  mean_diff_gain  += single_axis->get_diff_gain ();
  mean_diff_phase += single_axis->get_diff_phase ();
}
