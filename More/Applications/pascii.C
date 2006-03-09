#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <stdlib.h>
#include <unistd.h>

void usage ()
{
  cerr << 
    "pascii - pulsar archive ascii print\n"
    "\n"
    "pascii [options] filename\n"
    "options:\n"
    "  -b ibin    select a single phase bin, from 0 to nbin-1\n"
    "  -c ichan   select a single frequency channel, from 0 to nchan-1\n"
    "  -i isub    select a single integration, from 0 to nsubint-1\n"
    "  -p phase   select a single phase, from 0.0 to 1.0 (overrides -b)\n"
    "  -r phase   rotate the profiles by phase before printing\n"
    "  -F         Fscrunch first\n"
    "  -T         Tscrunch first\n"
    "  -P         Pscrunch first\n"
    "  -C         Centre first\n"
    "  -B b       Bscrunch by this factor first\n"
    "  -x         Convert to Stokes and also print fraction polarisation\n"
    "  -y         Convert to Stokes and also print fraction linear\n"
    "  -z         Convert to Stokes and also print fraction circular\n"
    "  -R         Remove baseline\n"
    "\n"
    "Each row output by pascii contains:\n"
    "\n"
    "isub ichan ibin S0 [S1 ... SN]\n" 
    "\n"
    "Where Si are fluxes of the type stored in the archive (e.g. I Q U V)\n"
       << endl;
}

int main (int argc, char** argv){ try {
  bool phase_chosen = false;
  float phase = 0.0;
  float rot_phase = 0.0;

  int cbin  = -1;
  int cchan = -1;
  int csub  = -1;

  bool do_fscr = false;
  bool do_pscr = false;
  bool do_tscr = false;
  bool do_centre = false;
  unsigned bscr = 1;
  bool show_pol_frac = false;
  bool show_lin_frac = false;
  bool show_circ_frac = false;
  bool remove_baseline = false;

  char c;
  while ((c = getopt(argc, argv, "b:B:c:CFi:p:Pr:RhpqTvVxyz")) != -1) 

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

    case 'r':
      rot_phase = atof (optarg);
      break;

    case 'F':
      do_fscr = true;
      break;
    case 'T':
      do_tscr = true;
      break;
    case 'P':
      do_pscr = true;
      break;
    case 'C':
      do_centre = true;
      break;
    case 'B':
      bscr = atoi(optarg);
      break;
    case 'x':
      show_pol_frac = true;
      break;
    case 'y':
      show_lin_frac = true;
      break;
    case 'z':
      show_circ_frac = true;
      break;
    case 'R':
      remove_baseline = true;
      break;

    } 


  if (optind >= argc) {
    cerr << "pascii: specify filename" << endl;
    return -1;
  }

  Pulsar::Archive* archive = Pulsar::Archive::load( argv[optind] );

  if( do_centre )
    archive->centre();
  if( remove_baseline )
    archive->remove_baseline();
  if( do_fscr )
    archive->fscrunch();
  if( do_tscr )
    archive->tscrunch();
  if( do_pscr )
    archive->pscrunch();
  if( bscr > 1 )
    archive->bscrunch( bscr );
  if (rot_phase)
    archive->rotate_phase (rot_phase);

  if( archive->get_state() != Signal::Stokes && (show_pol_frac || show_lin_frac || show_circ_frac ) )
    archive->convert_state(Signal::Stokes);

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
	if( show_pol_frac || show_lin_frac || show_circ_frac ){
	  float stokesI = integration->get_Profile(0,ichan)->get_amps()[ibin];
	  float stokesQ = integration->get_Profile(1,ichan)->get_amps()[ibin];
	  float stokesU = integration->get_Profile(2,ichan)->get_amps()[ibin];
	  float stokesV = integration->get_Profile(3,ichan)->get_amps()[ibin];

	  float frac_lin  = sqrt(stokesQ*stokesQ + stokesU*stokesU)/stokesI;
	  float frac_circ = fabs(stokesV)/stokesI;
	  float frac_pol  = sqrt(stokesQ*stokesQ + stokesU*stokesU + stokesV*stokesV)/stokesI;

	  if( show_pol_frac )  cout << " " << frac_pol;
	  if( show_lin_frac )  cout << " " << frac_lin;
	  if( show_circ_frac ) cout << " " << frac_circ;
	}
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

} catch(Error& er){ cerr << er << endl; exit(-1);
}
  return 0;
}


