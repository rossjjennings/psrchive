/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tempo++.h"

using namespace std;

int main () try
{
  Tempo::obsys ();

  for (unsigned i=0; i<Tempo::antennae.size(); i++)
  {
    Tempo::Observatory* obs = Tempo::antennae[i];

    cout << "obs name=" << obs->get_name() << " code=" << obs->get_code()
	 << " itoa=" << obs->get_itoa_code() << endl;
  }

  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}
