//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Attic/DynamicSNSpectrumTI.C,v $
   $Revision: 1.2 $
   $Date: 2007/10/01 10:39:58 $
   $Author:  */



#include "Pulsar/DynamicSNSpectrum.h"



using namespace Pulsar;


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

    add
        ( &DynamicSNSpectrum::get_subint,
                &DynamicSNSpectrum::set_subint,
                "subint", "Set the sub integration to plot" );

    add
        ( &DynamicSNSpectrum::get_pol,
                &DynamicSNSpectrum::set_pol,
                "pol", "Set the polarization to plot" );

    add
        ( &DynamicSNSpectrum::get_fsub, &DynamicSNSpectrum::set_fsub, "fsub", "The lower subint limit" );

    add
        ( &DynamicSNSpectrum::get_lsub, &DynamicSNSpectrum::set_lsub, "lsub", "The upper subint limit" );
}



