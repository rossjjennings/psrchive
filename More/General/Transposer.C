#include "Pulsar/Transposer.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

//! Default constructor
Pulsar::Transposer::Transposer (const Archive* _archive)
{
  archive = _archive;

  x1 = Signal::Phase;
  x2 = Signal::Frequency;
  x3 = Signal::Polarization;

  verbose = false;
}

Pulsar::Transposer::~Transposer ()
{
}

void Pulsar::Transposer::set_Archive (const Pulsar::Archive* _archive)
{
  archive = _archive; 
}

void Pulsar::Transposer::get_amps (vector<float>& amps) const
{
  if (!archive)
    throw Error (InvalidState, "Pulsar::Transposer::get_amps",
		 "no Archive set");

  Dimensions dim (archive);
  amps.resize ( dim.nsub * dim.npol * dim.nchan * dim.nbin );

  // establish the pyramid
  int increment = 1;
  Dimensions jump;

  jump.set_ndim (x1, increment);
  increment *= dim.get_ndim (x1);

  jump.set_ndim (x2, increment);
  increment *= dim.get_ndim (x2);
 
  jump.set_ndim (x3, increment);
  increment *= dim.get_ndim (x3);

  jump.set_if_zero (increment);

  if (verbose)
    cerr << "Transposer::get_amps nsub=" << dim.nsub << " npol=" << dim.npol 
	 << " nchan=" << dim.nchan << " nbin=" << dim.nbin << endl;

  if (verbose)
    cerr << "Transposer::get_amps jsub=" << jump.nsub << " jpol=" << jump.npol 
	 << " jchan=" << jump.nchan << " jbin=" << jump.nbin << endl;
  
  for (unsigned isub=0; isub < dim.nsub; isub++)
    get_amps (archive->get_Integration(isub), 
	      &(amps[0]) + isub*jump.nsub,
	      jump);

}

void Pulsar::Transposer::get_amps (const Integration* integration,
				   float* amps, const Dimensions& dim) const
{
  unsigned npol = integration->get_npol();
  unsigned nchan = integration->get_nchan();

  if (verbose)
    cerr << "int.npol=" << npol << " int.nchan=" << nchan << endl;

  for (unsigned ipol=0; ipol<npol; ipol++) {
    float* chandat = amps + ipol * dim.npol;
    for (unsigned ichan=0; ichan<nchan; ichan++)
      get_amps (integration->get_Profile(ipol, ichan), 
		chandat + ichan * dim.nchan,
		dim.nbin);
  }
}

void Pulsar::Transposer::get_amps (const Profile* profile,
				   float* amps, unsigned jbin) const
{
  const float* ptr = profile->get_amps();
  unsigned nbin = profile->get_nbin();

  for (unsigned ibin=0; ibin<nbin; ibin++) {
    *amps = *ptr;
    ptr ++;
    amps += jbin;
  }
}



Pulsar::Dimensions::Dimensions (const Pulsar::Archive* archive)
{
  if (!archive)
    nsub = npol = nchan = nbin = 0;
  else {
    nsub  = archive -> get_nsubint ();
    npol  = archive -> get_npol ();
    nchan = archive -> get_nchan ();
    nbin  = archive -> get_nbin ();
  }
}

void Pulsar::Dimensions::set_if_zero (unsigned ndim)
{
  if (!nsub) nsub = ndim;
  if (!npol) npol = ndim;
  if (!nchan) nchan = ndim;
  if (!nbin) nbin = nbin;
}

unsigned Pulsar::Dimensions::get_ndim (Signal::Dimension axis) const
{
  switch (axis)
    {
    case Signal::Phase:
      return nbin;
    case Signal::Frequency:
      return nchan;
    case Signal::Polarization:
      return npol;
    case Signal::Time:
      return nsub;
    default:
      return 0;
    }
}

void Pulsar::Dimensions::set_ndim (Signal::Dimension axis, unsigned ndim)
{
  switch (axis)
    {
    case Signal::Phase:
      nbin = ndim; break;
    case Signal::Frequency:
      nchan = ndim; break;
    case Signal::Polarization:
      npol = ndim; break;
    case Signal::Time:
      nsub = ndim; break;
    default:
      break;
    }
}
