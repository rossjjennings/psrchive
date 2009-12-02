/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PatchTime.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/ArchiveExpert.h"

#include <iostream>
using namespace std;

//! Default constructor
Pulsar::PatchTime::PatchTime ()
{
  minimum_contemporaneity = 0.5;
}

//! Set the policy used to judge contemporaneity
void Pulsar::PatchTime::set_contemporaneity_policy (Contemporaneity* policy)
{
  contemporaneity_policy = policy;
}

//! Set the minimum contemporaneity required for each sub-integration
void Pulsar::PatchTime::set_minimum_contemporaneity (double min)
{
  minimum_contemporaneity = min;
}

void insert (Pulsar::Archive* archive, unsigned& isub, const MJD& epoch)
{
  Reference::To<Pulsar::Integration> empty;
  empty = archive->get_Integration(0)->clone();
  empty->zero();
  empty->set_epoch( epoch );

  archive->expert()->insert (isub, empty);
  isub ++;
}

void tail (Pulsar::Archive* A, Pulsar::Archive* B, unsigned& isubA)
{
  while (isubA < A->get_nsubint()) 
  {
    Pulsar::Integration* subA = A->get_Integration (isubA);
    DEBUG("tail empty sub-integration isubA=" << isubA  \
         << " A::nsubint=" << A->get_nsubint() \
         << " B::nsubint=" << B->get_nsubint());
    insert (B, isubA, subA->get_epoch());
  }
}

//! Add to A whatever is missing with respect to B, and vice versa
void Pulsar::PatchTime::operate (Archive* A, Archive* B) try
{
  A->expert()->sort();
  B->expert()->sort();

  contemporaneity_policy->set_archives (A, B);

  unsigned isubA = 0;
  unsigned isubB = 0;

  while (isubA < A->get_nsubint() && isubB < B->get_nsubint())
  {
    Integration* subA = A->get_Integration (isubA);
    Integration* subB = B->get_Integration (isubB);

    double ratio = contemporaneity_policy->evaluate (subA, subB);

    if (ratio < minimum_contemporaneity)
    {
      // one of the archives requires an empty sub-integration

#if 0
    cerr << "mismatch ratio=" << ratio << " isubA=" << isubA << " isubB=" << isubB
         << " A::nsubint=" << A->get_nsubint()
         << " B::nsubint=" << B->get_nsubint() << endl;

    cerr << "A::epoch=" << subA->get_epoch().printdays(13) << endl;
    cerr << "B::epoch=" << subB->get_epoch().printdays(13) << endl;
#endif

      if (subA->get_epoch() > subB->get_epoch())
      {
	DEBUG("A requires an empty sub-integration isubA=" << isubA);
	insert (A, isubA, subB->get_epoch()); isubB++;
      }
      else
      {
	DEBUG("B requires an empty sub-integration isubB=" << isubB);
	insert (B, isubB, subA->get_epoch()); isubA++;
      }
    }
    else
    {
      // A and B are contemporaneous
      isubA++;
      isubB++;
    }
  }

  tail (A, B, isubA);
  tail (B, A, isubB);
}
catch (Error& error)
{
  throw error += "Pulsar::PatchTime::operate";
}

