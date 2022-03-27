/***************************************************************************
 *
 *   Copyright (C) 2009-2011 by Willem van Straten, Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/FITSHdrExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Dispersion.h"
#include "Pulsar/Integration.h"

#include "strutil.h"

using namespace std;
using namespace Pulsar;

//! Pulsar Archive splitting application
class psrsplit: public Pulsar::Application
{
public:

  //! Default constructor
  psrsplit ();

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Process the given archive
  void split_cal_extensions (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  unsigned nsubint;
  unsigned nchannel;

  bool resize_extensions;
  // deal correctly with frequency-only splitting
  bool useall_subints;

  bool cal_extensions;

  string directory;
};

int main (int argc, char** argv)
{
  psrsplit program;
  return program.main (argc, argv);
}

psrsplit::psrsplit ()
  : Pulsar::Application ("psrsplit", "splits an archive into multiple files")
{
  version = "$Id: psrsplit.C,v 1.5 2011/03/08 02:54:31 sosl Exp $";
  nsubint = 0;
  nchannel = 0;
  resize_extensions = false;
  cal_extensions = false;
}

void psrsplit::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (nsubint, 'n', "nsubint");
  arg->set_help ("number of sub-integrations per output file");

  arg = menu.add (nchannel, 'c', "nchannel");
  arg->set_help ("number of channels per output file\n"
		 "\t\tBy default all the channels are kept");

  arg = menu.add (directory, 'O', "path");
  arg->set_help ("write files to a new directory");

  arg = menu.add (resize_extensions, 'e');
  arg->set_help ("Attempt to strip the extensions in the output archives\n"
		  "\t\tWarning: this is almost only useful for reducing the size of output archives\n"
		  "\t\tbut can render the extensions meaningless");

  arg = menu.add (cal_extensions, "cal");
  arg->set_help ("split calibration-related extensions");
}

void psrsplit::setup ()
{
  if (nsubint == 0 && nchannel == 0)
    throw Error (InvalidState, "psrsplit::setup",
		 "please specify number of sub-integrations per output file");

  if (cal_extensions && nchannel == 0)
    throw Error (InvalidState, "psrsplit::setup",
                 "please specify number of channel per output calibrator file");

  useall_subints = nsubint == 0;
}

string get_extension (const std::string& filename)
{
  string::size_type index = filename.find_last_of( ".", filename.length() );
  if (index != string::npos)
    return filename.substr (index);
  else
    return "";
}

void psrsplit::process (Pulsar::Archive* archive)
{
  cerr << "psrsplit::process " << archive->get_filename() << endl;

  if (cal_extensions)
  {
    cerr << "psrsplit::process calibrator extensions" << endl;
    split_cal_extensions (archive);
    return;
  }

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  if ( nsubint > nsub )
    throw Error (InvalidState, "psrsplit::process",
		    "More sub-integrations per output file requested than available in "+archive->get_filename());

  if ( nchannel > nchan )
    throw Error (InvalidState, "psrsplit::process",
		    "More channels per output file requested than available in "+archive->get_filename());

  // if no desired number of subints was specified
  // (i.e., splitting in frequency dimension only), use all subints
  if ( useall_subints )
  {
    nsubint = nsub;
  }

  unsigned isplit = 0;
  unsigned ichansplit = 0;
  unsigned isub=0;
  unsigned ichan=0;

  Pulsar::Dispersion correction;
  bool was_dedispersed = archive->get_dedispersed () ;
  if ( was_dedispersed && nchannel > 0 )
    correction.revert (archive);

  while( isub < nsub )
  {
    cerr << "psrsplit: extracting";
    vector<unsigned> subints;
    for (unsigned jsub=0; jsub < nsubint && isub < nsub; jsub++)
    {
      cerr << " " << isub;
      subints.push_back( isub );
      isub ++;
    }
    cerr << endl;

    Reference::To<Pulsar::Archive> sub_archive = archive->extract(subints);
    Reference::To<Pulsar::Archive> sub_chan_archive;

#if THIS_IS_EVER_FIXED // or necessary

A) this loop repeatedly deletes the first subints.size() sub-integrations
   (not the sub-integrations indexed by the subints array)

B) after deleting these sub-integrations, the Archive tries to reload them
   (causing TimerArchive to fail)

    // delete the subintegrations that have been cloned in sub_archive
    for (unsigned isub=0; isub < subints.size(); isub++)
      delete archive->get_Integration (isub);

#endif

    // resize extensions to avoid bloating of the disk space used
    if ( resize_extensions )
      for ( unsigned iext = 0; iext < sub_archive->get_nextension (); iext++ )
	sub_archive->get_extension ( iext )->resize( nsubint );

    while ( ichan < nchan )
    {
      // prepare for channel separation
      sub_chan_archive = sub_archive->clone ();

      if ( nchannel > 0 )
      {

	ichan += nchannel ;

	if ( ichan < nchan )
	  sub_chan_archive->remove_chan (ichan, nchan - 1);
	if ( ichan - nchannel > 0 )
	  sub_chan_archive->remove_chan (0, ichan - nchannel - 1 );
	if ( was_dedispersed )
	  sub_chan_archive->dedisperse ();
	if (sub_chan_archive->has_model() && archive->get_type() == Signal::Pulsar)
	    sub_chan_archive->update_model ();
      }

      string ext;
      string filename = archive->get_filename();
      if ( nchannel == 0)
      {
	ext = stringprintf ("%04d", isplit) + get_extension (filename);
	ichan = nchan;
      }
      else 
	ext = stringprintf ("%04d_%04d", ichansplit, isplit) + get_extension (filename);

      filename = replace_extension( filename, ext );

      if (!directory.empty())
        filename = directory + "/" + basename (filename);

      cerr << "psrsplit: writing " << filename << endl;

      sub_chan_archive->unload ( filename );

      ichansplit++;
    }
    ichan = 0;
    ichansplit = 0;

    isplit ++;
  }
}

template<typename Container>
void remove (Container* container, unsigned ichan_start, unsigned nchannel)
{
  unsigned nchan = container->get_nchan();

  if ( ichan_start + nchannel < nchan )
    container->remove_chan (ichan_start + nchannel, nchan - 1);
  if ( ichan_start > 0 )
    container->remove_chan (0, ichan_start - 1 );
}

void psrsplit::split_cal_extensions (Pulsar::Archive* archive)
{
  CalibratorExtension* ext = archive->get<CalibratorExtension>();

  if (!ext)
    throw Error (InvalidState, "psrsplit::split_cal_extensions",
                 "Archive does not contain a calibrator extension");

  const unsigned nchan = ext->get_nchan();
  unsigned ichansplit = 0;

  for (unsigned ichan_start = 0; ichan_start < nchan; ichan_start += nchannel)
  {
    Reference::To<Archive> output = archive->clone();
    ext = output->get<CalibratorExtension>();

    remove (ext, ichan_start, nchannel);

    unsigned new_nchan = ext->get_nchan();

    double f0 = ext->get_centre_frequency (0);
    double fN = ext->get_centre_frequency (new_nchan - 1);
    double bw = (fN - f0) * new_nchan / double (new_nchan - 1);

    f0 = ext->get_centre_frequency (new_nchan / 2);
    fN = ext->get_centre_frequency (new_nchan / 2 -1);
    double cf = (f0+fN) / 2;

    CalibratorStokes* cs = output->get<CalibratorStokes>();
    if (cs)
      remove (cs, ichan_start, nchannel);

    string filename = archive->get_filename();
    string ext = stringprintf ("%04d", ichansplit) + get_extension (filename);
    filename = replace_extension( filename, ext );

    cerr << "psrsplit: writing " << filename << endl;

    output->expert()->set_nchan (nchannel);

    output->set_dedispersed (false);
    output->set_bandwidth (bw);
    output->set_centre_frequency (cf);

    // overcome the historical vagaries of OBSFREQ and OBSBW
    FITSHdrExtension* hdr = output->get<FITSHdrExtension> ();
    if (hdr)
    {
      hdr->obsfreq = cf;
      hdr->obsbw = bw;
    }
    
    output->unload ( filename );

    ichansplit ++;
  }
}

