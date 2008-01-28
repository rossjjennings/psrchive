//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Attic/DynamicSNSpectrumTI.C,v $
   $Revision: 1.3 $
   $Date: 2008/01/28 23:39:32 $
   $Author:  */



#include "Pulsar/DynamicSNSpectrum.h"
#include <pairutil.h>


using Pulsar::DynamicSNSpectrum;


/**
 * Constructor
 *
 * DOES     - Set the instance, add get/set methods
 * RECEIVES - The object to use
 * RETURNS  - Nothing
 * THROWS   - Nothing
 * TODO     - Nothing
 **/

DynamicSNSpectrum::Interface::Interface( DynamicSNSpectrum *s_instance = NULL )
{
  if( s_instance )
    set_instance( s_instance );

  add( &DynamicSNSpectrum::get_subint,
        &DynamicSNSpectrum::set_subint,
        "subint", "Set the sub integration to plot" );

  add( &DynamicSNSpectrum::get_pol,
        &DynamicSNSpectrum::set_pol,
        "pol", "Set the polarization to plot" );

  add( &DynamicSNSpectrum::get_srange,
         &DynamicSNSpectrum::set_srange,
         "srange", "Range of subints to plot" );
}



