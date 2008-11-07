/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/IntegrationManager.h"
#include "Pulsar/IntegrationManagerExpert.h"
#include "Pulsar/Integration.h"
#include "Error.h"

using namespace std;

unsigned Pulsar::IntegrationManager::verbose = 1;

Pulsar::IntegrationManager::IntegrationManager () 
{ 
  expert_interface = new Expert (this);

  if (verbose == 3)
    cerr << "IntegrationManager null constructor" << endl;
}

Pulsar::IntegrationManager::IntegrationManager (const IntegrationManager& o)
{
  throw Error (Undefined, "IntegrationManager copy constructor",
	       "sub-classes must define copy constructor");
}

Pulsar::IntegrationManager::~IntegrationManager () 
{ 
  if (verbose == 3)
    cerr << "Pulsar::IntegrationManager dtor this=" << this << endl;
}

/*!
  \param subint the index of the requested Integration
  \return pointer to Integration instance
*/
Pulsar::Integration* 
Pulsar::IntegrationManager::get_Integration (unsigned subint)
{
  if (verbose > 2) 
    cerr << "Pulsar::IntegrationManager::get_Integration subint=" 
	 << subint << endl;

  // ensure that the requested subint is valid
  if (subint >= get_nsubint())
    throw Error (InvalidRange, "IntegrationManager::get_Integration",
		 "isubint=%u nsubint=%u", subint, get_nsubint());

  // ensure that the subints vector is as large as the number of subints
  subints.resize (get_nsubint());

  // if the subint has not already been loaded, call the pure virtual
  // method, load_Integration, to load the requested sub-int.
  if (!subints[subint])
  {
    if (verbose > 2)
      cerr << "Pulsar::IntegrationManager::get_Integration load" << endl;
    subints[subint] = load_Integration (subint);
  }

  return subints[subint];
}

Pulsar::Integration* 
Pulsar::IntegrationManager::get_first_Integration ()
{  
  return get_Integration(0);
}

Pulsar::Integration* 
Pulsar::IntegrationManager::get_last_Integration ()
{  
  return get_Integration(get_nsubint()-1);
}

const Pulsar::Integration* 
Pulsar::IntegrationManager::get_Integration (unsigned subint) const
{
  if (verbose == 3)
    cerr << "Pulsar::IntegrationManager::get_Integration const" << endl;

  IntegrationManager* thiz = const_cast<IntegrationManager*> (this);
  return thiz->get_Integration (subint);
}

const Pulsar::Integration* 
Pulsar::IntegrationManager::get_first_Integration () const
{
  return get_Integration(0);
}

const Pulsar::Integration* 
Pulsar::IntegrationManager::get_last_Integration () const
{
  return get_Integration(get_nsubint()-1);
}

void Pulsar::IntegrationManager::append (const IntegrationManager* more)
{
  // ensure that the vector is as large as the current number of subints
  subints.resize (get_nsubint());

  for (unsigned isub=0; isub<more->get_nsubint(); isub++)
    subints.push_back ( new_Integration (more->get_Integration(isub)) );

  set_nsubint (subints.size());
}

void Pulsar::IntegrationManager::manage (IntegrationManager* more)
{
  // ensure that the vector is as large as the current number of subints
  subints.resize (get_nsubint());

  for (unsigned isub=0; isub<more->get_nsubint(); isub++)
    subints.push_back ( use_Integration (more->get_Integration(isub)) );

  set_nsubint (subints.size());
}  

void Pulsar::IntegrationManager::manage (Integration* integration)
{
  // ensure that the vector is as large as the current number of subints
  subints.resize (get_nsubint());

  subints.push_back( use_Integration (integration) );
  set_nsubint( subints.size() );
}

void Pulsar::IntegrationManager::unmanage (const Integration* integration)
{
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    if (get_Integration(isub) == integration)
    {
      unmanage (isub);
      break;
    }
}

void Pulsar::IntegrationManager::unmanage (unsigned isubint)
{
  if (isubint >= get_nsubint())
    throw Error (InvalidParam, "Pulsar::IntegrationManager::unmanage",
                 "isubint=%u >= nsubint=%u", isubint, get_nsubint());

  /*
    Ensure that all sub-integrations have been loaded from file.  This
    is necessary so that internal sub-integration indeces do not later
    mismatch those in the file.
  */
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    get_Integration(isub);

  subints.erase( subints.begin() + isubint );
  set_nsubint( subints.size() );
}

/*!
  If any current dimension is greater than that requested, the Profiles
  will be destroyed and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
*/

void Pulsar::IntegrationManager::resize (unsigned nsubint, bool instances)
{
  unsigned cur_nsub = subints.size();

  if (verbose == 3)
    cerr << "Pulsar::IntegrationManager::resize nsub=" << nsubint
	 << "  old nsub=" << cur_nsub  << endl;

  subints.resize (nsubint);

  if (instances)
    for (unsigned isub=cur_nsub; isub<nsubint; isub++)
      subints[isub] = new_Integration ();

  set_nsubint (nsubint);

  if (verbose == 3)
    cerr << "Pulsar::IntegrationManager::resize exit" << endl;
}

//! Provides access to the expert interface
Pulsar::IntegrationManager::Expert* Pulsar::IntegrationManager::expert ()
{
  return expert_interface;
}
