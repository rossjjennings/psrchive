#include "Calibration/MeanInstrument.h"
#include "Calibration/Instrument.h"

void Calibration::MeanInstrument::update (MEAL::Complex2* model) const
{
  Instrument* instrument = dynamic_cast<Instrument*>(model);
  if (!instrument)
    throw Error (InvalidParam, "Calibration::MeanInstrument::update",
		 "Complex2 model is not a Instrument");

  single_axis.update (instrument->get_backend());
  feed.update (instrument->get_feed());
}

void Calibration::MeanInstrument::integrate (const MEAL::Complex2* model)
{
  const Instrument* instrument = dynamic_cast<const Instrument*>(model);
  if (!instrument)
    throw Error (InvalidParam, "Calibration::MeanInstrument::update",
		 "Complex2 model is not a Instrument");

  single_axis.integrate (instrument->get_backend());
  feed.integrate (instrument->get_feed());

}

double Calibration::MeanInstrument::chisq (const MEAL::Complex2* model) const
{
  const Instrument* instrument = dynamic_cast<const Instrument*>(model);
  if (!instrument)
    throw Error (InvalidParam, "Calibration::MeanInstrument::chisq",
		 "Complex2 model is not a Instrument");

  Instrument test;
  update (&test);

  double chisq = 0.0;
  unsigned nparam = test.get_nparam();

  for (unsigned iparam=0; iparam<nparam; iparam++)
    if (instrument->get_infit(nparam)) {
      double diff = test.get_param(iparam) - instrument->get_param(iparam);
      double var = test.get_variance(iparam)+instrument->get_variance(iparam);
      chisq += diff*diff/var;
    }

  return chisq/nparam;
}
