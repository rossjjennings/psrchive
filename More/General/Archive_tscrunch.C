/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/TimeIntegrate.h"

using namespace std;

static Pulsar::TimeIntegrate* operation = 0;
static Pulsar::TimeIntegrate::EvenlySpaced* policy = 0;

static void static_init ()
{
  operation = new Pulsar::TimeIntegrate;
  policy    = new Pulsar::TimeIntegrate::EvenlySpaced;

  operation->set_range_policy( policy );
}

/*!
  \param nscrunch number of neighbouring Integrations to add. 
                  If nscrunch == 0, then add all Integrations together
 */
void Pulsar::Archive::tscrunch (unsigned nscrunch)
{
  if (!policy)
    static_init ();

  policy->set_nintegrate (nscrunch);
  operation->transform (this);
}

/////////////////////////////////////////////////////////////////////////////
//
/*!
  Useful wrapper for Archive::tscrunch
*/
void Pulsar::Archive::tscrunch_to_nsub (unsigned new_nsub)
{
  if (new_nsub <= 0)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "Invalid nsub request (new_nsub=%d nsub=%d)",
		 new_nsub,get_nsubint());
  else if (get_nsubint() < new_nsub)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "Archive has too few subints (new_nsub=%d nsub=%d)",
		 new_nsub,get_nsubint());
  else
    tscrunch(get_nsubint() / new_nsub);
}
