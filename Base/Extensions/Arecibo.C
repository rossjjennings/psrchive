/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Arecibo.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "tempo++.h"

using namespace std;

void Pulsar::Arecibo::Lwide (Receiver* r)
{
  r->set_name ("L-wide");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(1); 
  r->set_tracking_mode(Receiver::Feed);
}

//! Initialize the Receiver Extension with Arecibo best guess
void Pulsar::Arecibo::guess (Receiver* r, Archive* a)
{

}
