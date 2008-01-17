/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/StandardApplication.h"
#include "Pulsar/Archive.h"

//
//! An example of a standard application
//

/*! 
  A standard application provides preprocessing functionality using
  the standard pulsar shell interpreter 
*/

class example : public Pulsar::StandardApplication
{
public:

  //! Default constructor
  example ();

  //! Process the given archive
  void process (Pulsar::Archive*);
};


/*!
  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "example -h"
*/

example::example ()
  : StandardApplication ("example", "example psrchive program")
{
}

/*!

  This example simply loads every profile into memory.

  If you want to use the Profile class, then you will have to

  #include "Pulsar/Profile.h"

*/
void example::process (Pulsar::Archive* archive)
{
  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  
  for (unsigned isub=0; isub < nsub; isub++)    
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
	Pulsar::Profile* profile = archive->get_Profile (isub, ipol, ichan);
}

static example program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

