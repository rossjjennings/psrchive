/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/PolnCalibrator.h"

#include "Pulsar/SingleAxis.h"
#include "Pulsar/Instrument.h"
#include "MEAL/Polar.h"

template<class T, class F>
void copy (T* to, const F* from)
{
  if (to->get_nparam() != from->get_nparam())
    throw Error (InvalidParam, "copy<To,From>",
		 "to nparam=%d != from nparam=%d",
		 to->get_nparam(), from->get_nparam());

  for (unsigned i=0; i<to->get_nparam(); i++)
    to->set_Estimate(i, from->get_Estimate(i));
}

//! Construct from a PolnCalibrator instance
Pulsar::PolnCalibratorExtension::PolnCalibratorExtension
(const PolnCalibrator* calibrator) 
  : CalibratorExtension ("PolnCalibratorExtension")
{
  if (!calibrator)
    throw Error (InvalidParam, "Pulsar::PolnCalibratorExtension",
                 "null PolnCalibrator*");

  try {

    if (Archive::verbose == 3)
      cerr << "Pulsar::PolnCalibratorExtension(PolnCalibrator*)" << endl;

    CalibratorExtension::build (calibrator);

    unsigned nchan = get_nchan();
    for (unsigned ichan=0; ichan < nchan; ichan++)
      if ( calibrator->get_transformation_valid(ichan) ) {
        copy( get_transformation(ichan), 
	      calibrator->get_transformation(ichan) );
	set_valid (ichan, true);
      }
      else
        set_valid (ichan, false);

  }
  catch (Error& error) {
    throw error += "Pulsar::PolnCalibratorExtension (PolnCalibrator*)";
  }

}

//! Return a new MEAL::Complex2 instance, based on type attribute
MEAL::Complex2*
Pulsar::new_transformation (const PolnCalibratorExtension* ext, unsigned ichan)
{
  if( !ext->get_valid(ichan) )
    return 0;

  MEAL::Complex2* xform = new_transformation( ext->get_type() );

  copy( xform, ext->get_transformation(ichan) );

  return xform;
}

MEAL::Complex2* Pulsar::new_transformation( Calibrator::Type type )
{
  switch (type) {
  case Calibrator::SingleAxis:
    return new Calibration::SingleAxis;
  case Calibrator::Polar:
    return new MEAL::Polar;
  case Calibrator::Hamaker:
    return new MEAL::Polar;
  case Calibrator::Britton:
    return new Calibration::Instrument;
  default:
    throw Error (InvalidState,
               "Pulsar::PolnCalibrator::new_transformation",
               "unrecognized Calibrator::Type = %d", (int) type);
  }
}

