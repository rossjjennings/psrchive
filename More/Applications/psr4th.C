
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/FourthMomentStats.h"
#include "Pulsar/StokesPlot.h"

#include <iostream>
#include <cpgplot.h>

using namespace std;
using namespace Pulsar;

int main (int argc, char** argv) try
{
  if (argc < 2) {
    cerr << "Please specify a filename" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> archive;
  archive = Pulsar::Archive::load (argv[1]);
  cout << "Hello, PSR " << archive->get_source() << "!" << endl;

  archive->remove_baseline();

  FourthMomentStats stats;

  Reference::To<PolnProfile> profile;
  profile = archive->get_Integration(0)->new_PolnProfile(0);
  stats.set_profile (profile);

  PolnProfile modeA;
  PolnProfile modeB;

  stats.separate (modeA, modeB);

  PolnProfile p1;
  PolnProfile p2;
  PolnProfile p3;

  stats.eigen (*profile, p2, p3);

  archive->unload("psr4th.ar");

  cpgopen ("?");

  StokesPlot plot;

  archive->centre ();
  plot.plot (archive);

  cpgend ();

  return 0;
}
 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
