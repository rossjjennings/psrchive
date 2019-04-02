/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/UnloadJones.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "strutil.h"

using namespace std;

Pulsar::UnloadJones::UnloadJones ()
{
  filename = "Jones_tf.txt";
}

void flat (std::ostream& output, const Jones<double>& J)
{
  output << J.j00.real() << " " << J.j00.imag() << " "
         << J.j01.real() << " " << J.j01.imag() << " "
         << J.j10.real() << " " << J.j10.imag() << " "
         << J.j11.real() << " " << J.j11.imag();
}

void Pulsar::UnloadJones::unload (SystemCalibrator* model)
{
  if (filename.empty())
    throw Error (InvalidState, "Pulsar::UnloadJones::unload",
		 "filename not set");

  std::ofstream output (filename.c_str());

  unsigned nchan = model->get_nchan();
  MJD start = model->get_start_epoch ();
  MJD end = model->get_end_epoch ();

  unsigned nstep = 100;
  MJD step = (end - start) / (nstep - 1);

  const Integration* subint = model->get_Archive()->get_Integration(0);

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    Calibration::SignalPath* path
        = const_cast<Calibration::SignalPath*>( model->get_model(ichan) );

    if (!path->get_valid())
      continue;

    double cfreq_in_Hz = subint->get_centre_frequency(ichan) * 1e6;

    path->engage_time_variations();

    // Calibration::ReceptionModel* equation = path->get_equation();
    // path->set_transformation_index (ipath);
    // path->set_input_index (isource);

    //! Get the instrumental transformation
    const MEAL::Complex2* instrument = path->get_transformation ();

    for (unsigned ipt=0; ipt<nstep; ipt++) try
    {
      MJD index = start + step * ipt;
      path->time.set_value (index);

      Jones<double> J = instrument->evaluate();
      output << index.printdays(10) << " " << cfreq_in_Hz << " ";
      flat (output, J);
      output << endl;
    }
    catch (Error& err)
    {
    }
  }
}

