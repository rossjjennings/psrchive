#include "Pulsar/IntegrationManager.h"
#include "Pulsar/Integration.h"
#include "Error.h"

bool Pulsar::IntegrationManager::verbose = false;

Pulsar::IntegrationManager::IntegrationManager () 
{ 
  if (verbose)
    cerr << "IntegrationManager null constructor" << endl;
}

Pulsar::IntegrationManager::IntegrationManager (const IntegrationManager& o)
{
  throw Error (Undefined, "IntegrationManager copy constructor",
	       "sub-classes must define copy constructor");
}

Pulsar::IntegrationManager::~IntegrationManager () 
{ 
  if (verbose)
    cerr << "IntegrationManager::destructor" << endl;

  for (unsigned isub=0; isub<subints.size(); isub++)
    if (subints[isub])
      delete subints[isub];
}

/*!
  \param subint the index of the requested Integration
  \return pointer to Integration instance
*/
Pulsar::Integration* 
Pulsar::IntegrationManager::get_Integration (unsigned subint)
{
  // ensure that the requested subint is valid
  if (subint >= get_nsubint())
    throw Error (InvalidRange, "IntegrationManager::get_Integration",
		 "isubint=%u nsubint=%u", subint, get_nsubint());

  // ensure that the subints vector is as large as the number of subints
  resize (get_nsubint(), false);

  // if the subint has not already been loaded, call the pure virtual
  // method, load_Integration, to load the requested sub-int.
  if (!subints[subint])
    subints[subint] = load_Integration (subint);

  return subints[subint];

}

const Pulsar::Integration* 
Pulsar::IntegrationManager::get_Integration (unsigned subint) const
{
  if (verbose)
    cerr << "Pulsar::IntegrationManager::get_Integration const" << endl;

  IntegrationManager* thiz = const_cast<IntegrationManager*> (this);
  return thiz->get_Integration (subint);
}

void Pulsar::IntegrationManager::append (const IntegrationManager* more)
{
  for (unsigned isub=0; isub<more->subints.size(); isub++)
    subints.push_back ( new_Integration (more->subints[isub]) );

  set_nsubint (subints.size());
}  

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*!
  If any current dimension is greater than that requested, the Profiles
  will be deleted and the dimension resized.  If any current dimension is
  smaller than that requested, the dimension will be resized and new Profiles
  will be constructed.  If any of the supplied paramaters is equal to zero,
  the dimension is left unchanged.
  */

void Pulsar::IntegrationManager::resize (unsigned nsubint, bool instances)
{
  unsigned cur_nsub = subints.size();

  if (verbose)
    cerr << "Pulsar::IntegrationManager::resize nsub=" << nsubint
	 << "  old nsub=" << cur_nsub  << endl;

  unsigned isub;
  for (isub=nsubint; isub<cur_nsub; isub++)
    delete subints[isub];

  subints.resize (nsubint);

  for (isub=cur_nsub; isub<nsubint; isub++)
    if (instances)
      subints[isub] = new_Integration ();
    else
      subints[isub] = 0;

  set_nsubint (nsubint);

  if (verbose)
    cerr << "Pulsar::IntegrationManager::resize exit" << endl;
}
