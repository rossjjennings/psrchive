#include "Calibration/MeanInstrument.h"
#include "Calibration/Instrument.h"

void Calibration::MeanInstrument::update (MEAL::Complex2* model)
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
