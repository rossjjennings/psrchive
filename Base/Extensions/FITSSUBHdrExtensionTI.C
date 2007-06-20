/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
 
 
 






#include "Pulsar/FITSSUBHdrExtensionTI.h"

Pulsar::FITSSUBHdrExtensionTI::FITSSUBHdrExtensionTI ()
{
  setup();
}


Pulsar::FITSSUBHdrExtensionTI::FITSSUBHdrExtensionTI( FITSSUBHdrExtension *c )
{
  setup();
  set_instance( c );
}


void Pulsar::FITSSUBHdrExtensionTI::setup( void )
{
  add( &FITSSUBHdrExtension::get_int_type, "int_type", "Time axis (TIME, BINPHSPERI, BINLNGASC, etc)" );
  add( &FITSSUBHdrExtension::get_int_unit, "int_unit", "Unit of time axis (SEC, PHS (0-1), DEG)" );
  add( &FITSSUBHdrExtension::get_tsamp, "tsamp", "[s] Sample interval for SEARCH-mode data" );
  add( &FITSSUBHdrExtension::get_nbin, "nbin", "Nr of bins (PSR/CAL mode; else 1)" );
  add( &FITSSUBHdrExtension::get_nbits, "nbits", "Nr of bits/datum (SEARCH mode 'X' data, else 1)" );
  add( &FITSSUBHdrExtension::get_nch_file, "nch_file", "Number of channels/sub-bands in this file" );
  add( &FITSSUBHdrExtension::get_nch_strt, "nch_strt", "Start channel/sub-band number (0 to NCHAN-1)" );
  add( &FITSSUBHdrExtension::get_npol, "npol", "Nr of polarisations in table" );
  add( &FITSSUBHdrExtension::get_nsblk, "nsblk", "Samples/row (SEARCH mode, else 1)" );
}

TextInterface::Class *Pulsar::FITSSUBHdrExtensionTI::clone()
{
  if( instance )
    return new FITSSUBHdrExtensionTI( instance );
  else
    return new FITSSUBHdrExtensionTI();
}

