#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

void usage ()
{
  cerr << 
    "pascii - pulsar archive ascii dump\n"
    "\n"
    "pascii [options] filename\n"
    "options:\n"
    "  -b ibin    select a single phase bin, from 0 to nbin-1\n"
    "  -c ichan   select a single frequency channel, from 0 to nchan-1\n"
    "  -i isub    select a single integration, from 0 to nsubint-1\n"
    "  -p phase   select a single phase, from 0 to 1 (overrides -b)\n"
    "\n"
    "Each row output by pascii contains:\n"
    "\n"
    "isub ichan ibin S0 [S1 ... SN]\n" 
    "\n"
    "Where Si are fluxes of the type stored in the archive (e.g. I Q U V)\n"
       << endl;
}

int main (int argc, char** argv)
{
  bool phase_chosen = false;
  float phase = 0.0;

  int cbin  = -1;
  int cchan = -1;
  int csub  = -1;

  char c;
  while ((c = getopt(argc, argv, "b:c:i:p:hqvV")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'b':
      cbin = atoi (optarg);
      break;

    case 'c':
      cchan = atoi (optarg);
      break;

    case 'i':
      csub = atoi (optarg);
      break;


    case 'p':
      phase_chosen = true;
      phase = atof (optarg);
      break;

    } 


  if (optind >= argc) {
    cerr << "pascii: specify filename" << endl;
    return -1;
  }

  Pulsar::Archive* archive = Pulsar::Archive::load( argv[optind] );

  archive->remove_baseline();
 
  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  unsigned nbin = archive->get_nbin();

  if (phase_chosen)
    cbin = int (phase * (nbin-1));

  if (cbin > 0 && unsigned(cbin) >= nbin) {
    cerr << "pascii: -b " << cbin << " >= nbin=" << nbin << endl;
    return -1;
  }

  if (cchan > 0 && unsigned(cchan) >= nchan) {
    cerr << "pascii: -c " << cchan << " >= nchan=" << nchan << endl;
    return -1;
  }

  if (csub > 0 && unsigned(csub) >= nsub) {
    cerr << "pascii: -i " << csub << " >= nsub=" << nsub << endl;
    return -1;
  }

  for (unsigned isub=0; isub < nsub; isub++) {

    if (csub > 0)
      isub = csub;

    Pulsar::Integration* integration = archive->get_Integration(isub);

    for (unsigned ichan=0; ichan < nchan; ichan++) {

      if (cchan > 0)
	ichan = cchan;

      for (unsigned ibin=0; ibin < nbin; ibin++) {

	if (cbin > 0)
	  ibin = cbin;

	cout << isub << " " << ichan << " " << ibin;
	for (unsigned ipol=0; ipol < npol; ipol++)
	  cout<<" "<< integration->get_Profile(ipol,ichan)->get_amps()[ibin];
	cout << endl;

	if (cbin > 0)
	  break;

      }

      if (cchan > 0)
	break;

    }

    if (csub > 0)
      break;

  }

  return 0;

}

