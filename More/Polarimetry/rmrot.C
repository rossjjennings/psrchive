#include <iostream>
#include <unistd.h>

#include "Calibration/Faraday.h"

using namespace std;

void usage()
{
  cerr <<
    "rmrot - output change in position angle across observed band\n"
    "  Usage: rmrot [-d DM] [-b BW] [-f FREQ] [-n NCHAN] -t\n"
    "\n"
    "   -r RM      rotation measure in rad/m^2\n"
    "   -b BW      bandwidth in MHz\n"
    "   -f FREQ    centre frequency in MHz\n"
    "\n"
    "   -q         quiet (outputs only delta P.A.)\n"
    "   -v         verbose \n"
    "\n"
    "  Program returns the number of degrees over which the position angle \n"
    "  rotates across the band, as well as the differential group delay \n"
       << endl;
}

int main(int argc, char ** argv)
{ try {

  bool verbose = false;
  bool quiet = false;

  // these default parameters correspond to a delta PA = 0.129536 deg
  double rm = 1;
  double centrefreq = 430;
  double bw = 1;

  int c;
  while ((c = getopt(argc, argv, "hb:f:r:qv")) != -1)
    switch (c) {

    case 'b':
      bw = atof (optarg);
      break;

    case 'r':
      rm = atof (optarg);
      break;

    case 'f':
      centrefreq = atof (optarg);
      break;

    case 'h':
      usage ();
      return 0;

    case 'q':
      quiet = true;
      verbose = false;
      break;

    case 'v':
      Calibration::Faraday::verbose = true;
      verbose = true;
      quiet = false;
      break;

    default:
      cerr << "invalid param '" << c << "'" << endl;
    }

  Calibration::Faraday kernel;

  kernel.set_reference_frequency (centrefreq);
  kernel.set_rotation_measure (rm);

  if (!quiet)
    cout << "\nInput Parameters:\n"
      "Centre Frequency: " << kernel.get_reference_frequency () << " MHz\n"
      "Bandwidth:        " << bw << " MHz\n"
      "Rotation Measure: " << kernel.get_rotation_measure().val <<" rad/m^2\n";
  
  kernel.set_frequency (centrefreq + 0.5*bw);
  double pa_hi = kernel.get_rotation ();

  kernel.set_frequency (centrefreq - 0.5*bw);
  double pa_lo = kernel.get_rotation ();

  double delta_PA = (pa_hi - pa_lo) / (2.0*M_PI);
  // phase delay between RHC and LHS is twice the change in P.A.
  double dgd = 2.0 * fabs(delta_PA) / centrefreq;

  if (!quiet)
    cout << "\nOutput parameters:\n"
      "Position angle change:    " << delta_PA*360.0 << " deg\n"
      "Differential group delay: " << dgd << " us\n";
  else
    cout << delta_PA*360.0 << endl;

  return 0;

}
catch (...) {
  cerr << "exception thrown: " << endl;
  return -1;
}
}

