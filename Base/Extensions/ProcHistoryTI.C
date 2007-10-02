/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ProcHistoryTI.h"

Pulsar::ProcHistoryTI::ProcHistoryTI ()
{
  setup();
}


Pulsar::ProcHistoryTI::ProcHistoryTI( ProcHistory *c )
{
  setup();
  set_instance( c );
}


void Pulsar::ProcHistoryTI::setup( void )
{
  add( &ProcHistory::get_last_nbin_prd, "nbin_prd", "Nr of bins per period" );
  add( &ProcHistory::get_last_tbin, "tbin", "Time per bin or sample" );
  add( &ProcHistory::get_last_chan_bw, "chan_bw", "Channel bandwidth" );
}

TextInterface::Parser *Pulsar::ProcHistoryTI::clone()
{
  if( instance )
    return new ProcHistoryTI( instance );
  else
    return new ProcHistoryTI();
}

