#include "Pulsar/IntegrationOrder.h"

//! Default constructor
Pulsar::IntegrationOrder::IntegrationOrder ()
  : Extension ("IntegrationOrder")
{
  IndexState = "Null State";
}

//! Copy constructor
Pulsar::IntegrationOrder::IntegrationOrder (const IntegrationOrder& extension)
  : Extension ("IntegrationOrder")
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
}

//! Operator =
const Pulsar::IntegrationOrder&
Pulsar::IntegrationOrder::operator= (const IntegrationOrder& extension)
{
  IndexState = extension.IndexState;
  Unit       = extension.Unit;
  indices    = extension.indices;
  return *this;
}

//! Destructor
Pulsar::IntegrationOrder::~IntegrationOrder ()
{
}

string Pulsar::IntegrationOrder::get_IndexState () const
{
  return IndexState;
}

string Pulsar::IntegrationOrder::get_Unit () const
{
  return Unit;
}

Estimate<double> Pulsar::IntegrationOrder::get_Index (unsigned subint) const
{
  if (subint > indices.size())
    throw Error (InvalidParam, "Pulsar::IntegrationOrder::get_Index",
		 "Invalid index number");
  
  return indices[subint];
}

void Pulsar::IntegrationOrder::set_Index (unsigned subint, Estimate<double> i)
{
  if (subint > indices.size())
    throw Error (InvalidParam, "Pulsar::IntegrationOrder::set_Index",
		 "Invalid index number");

  indices[subint] = i;
}
