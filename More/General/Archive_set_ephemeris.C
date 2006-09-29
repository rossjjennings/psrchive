/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

/*! \param new_ephemeris the ephemeris to be installed
    \param update create a new polyco for the new ephemeris
 */
void Pulsar::Archive::set_ephemeris (const psrephem& new_ephemeris, bool update)
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::set_ephemeris" << endl;

  ephemeris = new psrephem (new_ephemeris);

  if (get_source() != ephemeris->psrname()) {

    // a CAL observation shouldn't have an ephemeris
    // no need for the removed test

    string temp1 = get_source();
    string temp2 = ephemeris->psrname();

    bool change = false;

    if (temp1.length() > temp2.length())
      change = temp1.substr(1,temp1.length()) != temp2;
    else
      change = temp2.substr(1,temp2.length()) != temp1;

    if (change) {
      if (verbose == 3)
        cerr << "Archive::set_ephemeris Informative Notice:\n"
             << "   Source name will be updated to match new ephemeris\n"
             << "   New name: " << temp2 << endl;
      set_source(temp2);
    }
  }

  if (get_dispersion_measure() != ephemeris->get_dm()) {
    if (verbose == 3)
      cerr << "Archive::set_ephemeris Informative Notice:\n"
           << "   Dispersion measure will be updated to match new ephemeris\n"
           << "   Old DM = " << get_dispersion_measure() << endl
           << "   New DM = " << ephemeris->get_dm() << endl;
    set_dispersion_measure(ephemeris->get_dm());
  }

  if (update)  {
    if (verbose == 3)
      cerr << "Pulsar::Archive::set_ephemeris update polyco" << endl;
    update_model ();
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::set_ephemeris exit" << endl;
}

