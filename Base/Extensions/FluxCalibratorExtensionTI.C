/***************************************************************************
 *
 *   Copyright (C) 2009 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorExtensionInterface.h"
#include "TextInterfaceEstimate.h"

Pulsar::FluxCalibratorExtension::Interface::Interface
( FluxCalibratorExtension *s_instance )
{
  if (s_instance)
    set_instance (s_instance);

  import ( CalibratorExtension::Interface() );

  // read-only: requires resize
  add( &FluxCalibratorExtension::get_nreceptor,
       "nrcvr", "Number of receiver channels" );

  add( &FluxCalibratorExtension::has_scale,
       "scale", "scale (1=Native 0=Reference)" );

  import( "sol", Solution::Interface(),
          (Solution*(FluxCalibratorExtension::*)(unsigned))
          &FluxCalibratorExtension::get_solution,
          &FluxCalibratorExtension::get_nchan );

}

Pulsar::FluxCalibratorExtension::Solution::Interface::Interface ()
{
  VGenerator< Estimate<double> > generator;
  generator( this, "S_sys", "System equivalent flux density",
             &Solution::get_S_sys,
             &Solution::get_nreceptor );

  generator( this, "S_cal", "Reference source flux density",
             &Solution::get_S_cal,
             &Solution::get_nreceptor );
}

