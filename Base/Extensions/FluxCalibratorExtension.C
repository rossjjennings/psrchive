#include "Pulsar/FluxCalibratorExtension.h"

//! Default constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension ()
  : CalibratorExtension ("FluxCalibratorExtension")
{
}

//! Copy constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension
(const FluxCalibratorExtension& copy)
  : CalibratorExtension (copy)
{
  operator = (copy);
}

//! Operator =
const Pulsar::FluxCalibratorExtension&
Pulsar::FluxCalibratorExtension::operator= 
(const FluxCalibratorExtension& copy)
{
  if (this == &copy)
    return *this;

  CalibratorExtension::operator= (copy);
  cal_flux = copy.cal_flux;
  T_sys = copy.T_sys;

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

