#include "Pulsar/FluxCalibratorExtension.h"

//! Default constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension ()
  : CalibratorExtension ("FluxCalibratorExtension")
{
  type = Calibrator::Flux;
}

//! Copy constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension
(const FluxCalibratorExtension& copy)
  : CalibratorExtension (copy)
{
  type = Calibrator::Flux;
  operator = (copy);
}

//! Operator =
const Pulsar::FluxCalibratorExtension&
Pulsar::FluxCalibratorExtension::operator= (const FluxCalibratorExtension& fc)
{
  if (this == &fc)
    return *this;

  CalibratorExtension::operator= (fc);
  cal_flux = fc.cal_flux;
  T_sys = fc.T_sys;

  return *this;
}

//! Destructor
Pulsar::FluxCalibratorExtension::~FluxCalibratorExtension ()
{
}

//! Set the number of frequency channels
void Pulsar::FluxCalibratorExtension::set_nchan (unsigned _nchan)
{
  CalibratorExtension::set_nchan( _nchan );

  cal_flux.resize( _nchan );
  T_sys.resize( _nchan );
}

using namespace Pulsar;

void FluxCalibratorExtension::set_T_sys (unsigned ichan,
					 const Estimate<double>& _T_sys)
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::set_T_sys");
  T_sys[ichan] = _T_sys;
}

Estimate<double> FluxCalibratorExtension::get_T_sys (unsigned ichan) const
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::get_T_sys");
  return T_sys[ichan];
}

void FluxCalibratorExtension::set_cal_flux (unsigned ichan,
					    const Estimate<double>& _cal_flux)
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::set_cal_flux");
  cal_flux[ichan] = _cal_flux;
}

Estimate<double> FluxCalibratorExtension::get_cal_flux (unsigned ichan) const
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::get_cal_flux");
  return cal_flux[ichan];
}
