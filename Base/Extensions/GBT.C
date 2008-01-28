/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/GBT.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Receiver.h"
#include "tempo++.h"

using namespace std;

void Pulsar::GBT::Rcvr_350 (Receiver* r)
{
  r->set_name ("Rcvr_350");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(1); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::GBT::Rcvr_800 (Receiver* r)
{
  r->set_name ("Rcvr_800");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(1); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::GBT::Rcvr1_2 (Receiver* r)
{
  r->set_name ("Rcvr1_2");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(1); 
  r->set_tracking_mode(Receiver::Feed);
}

void Pulsar::GBT::Rcvr2_3 (Receiver* r)
{
  r->set_name ("Rcvr2_3");
  r->set_basis(Signal::Linear);
  r->set_nrcvr(1); 
  r->set_tracking_mode(Receiver::Feed);
}

//! Initialize the Receiver Extension with GBT best guess
void Pulsar::GBT::guess (Receiver* r, Archive* a)
{

}
