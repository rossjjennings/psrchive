#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Pulsar/getopt.h"

void usage ()
{
  cerr << 
    "pascii [-p phase] filename\n" 
    "outputs:\n"
    "isub ichan ibin I Q U V" 
       << endl;
}

int main (int argc, char** argv)
{
  bool verbose = false;

  bool phase_chosen = false;
  float phase = 0.0;

  char c;
  while ((c = getopt(argc, argv, "hp:vV")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
    case 'v':
      verbose = true;
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

  unsigned pbin = unsigned (phase * nbin);

  for (unsigned isub=0; isub < nsub; isub++) {

    Pulsar::Integration* integration = archive->get_Integration(0);

    for (unsigned ichan=0; ichan < nchan; ichan++)

      if (!phase_chosen)
	for (unsigned ibin=0; ibin < nbin; ibin++) {
	  cout << isub << " " << ichan << " " << ibin;
	  for (unsigned ipol=0; ipol < npol; ipol++)
	    cout<<" "<< integration->get_Profile(ipol,ichan)->get_amps()[ibin];
	  cout << endl;
	}
    
      else {
	cout << isub << " " << ichan << " " << pbin;
	for (unsigned ipol=0; ipol < npol; ipol++)
	  cout<<" "<< integration->get_Profile(ipol,ichan)->get_amps()[pbin];
	cout << endl;
      }

  }

  return 0;

}
