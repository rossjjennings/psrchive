/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendFeed.h"
#include "Pulsar/VariableBackend.h"

using namespace std;
using namespace Calibration;

void BackendFeed::init ()
{
  backend = new VariableBackend;
  add_model( backend );
}

void BackendFeed::set_frontend (MEAL::Complex2* xform)
{
  frontend = xform;
  add_model( frontend );
}

BackendFeed::BackendFeed ()
{
  init ();
}

BackendFeed::BackendFeed (const BackendFeed& s)
{
  init ();
  operator = (s);
}

//! Equality Operator
const BackendFeed& BackendFeed::operator = (const BackendFeed& s)
{
  if (&s != this)
    *backend = *(s.backend);
  return *this;
}

BackendFeed::~BackendFeed ()
{
  if (verbose)
    cerr << "BackendFeed::dtor" << endl;
}

void BackendFeed::set_cyclic (bool flag)
{
  if (verbose)
    cerr << "BackendFeed::set_cyclic" << endl;

  backend->set_cyclic (flag);
}

//! Provide access to the backend model
const VariableBackend* BackendFeed::get_backend () const
{
  return backend;
}

VariableBackend* BackendFeed::get_backend ()
{
  return backend;
}
