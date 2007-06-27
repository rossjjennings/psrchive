/***************************************************************************
 *
 *   Copyright (C) 2006 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <Pulsar/Archive.h>
#include <Pulsar/FITSHdrExtension.h>
#include <Pulsar/ObsExtension.h>
#include <Pulsar/ITRFExtension.h>
#include <Pulsar/Backend.h>
#include <Pulsar/Receiver.h>
#include <Pulsar/ArchiveTI.h>
#include <Pulsar/WidebandCorrelator.h>
#include <Pulsar/FITSHdrExtensionTI.h>
#include <Pulsar/ObsExtensionTI.h>
#include <Pulsar/ITRFExtensionTI.h>
#include <Pulsar/BackendTI.h>
#include <Pulsar/ReceiverTI.h>
#include <Pulsar/WidebandCorrelatorTI.h>
#include <Pulsar/FluxCalibratorExtension.h>
#include <Pulsar/ProcHistory.h>
#include <Pulsar/Parameters.h>
#include <Pulsar/Predictor.h>
#include <TextInterface.h>

#include <dirutil.h>
#include <strutil.h>
#include "remap.h"
#include "tostring.h"
#include <Angle.h>


#include "TextFinder.h"

#include <unistd.h>

#include <sstream>




using namespace std;
using namespace Pulsar;




bool ephemmode = false;
bool polycmode = false;
bool verbose = false;
bool show_extensions = false;
bool hide_headers = false;
vector< string > commands;
vector< vector< string > > results;
vector< string > current_row;




class VapTextFinder : public ArchiveTextFinder
{
public:
  VapTextFinder() : ArchiveTextFinder()
  {
    AddGrp( "COMMON PARAMETERS" );
    AddMap( "name", "ArchiveTI-name" );
    AddMap( "nbin", "ArchiveTI-nbin" );
    AddMap( "nchan", "ArchiveTI-nchan" );
    AddMap( "npol", "ArchiveTI-npol" );
    AddMap( "nsub", "ArchiveTI-nsub" );
    AddMap( "stime", "ArchiveTI-stime" );
    //AddMap( "etime", "ArchiveTI-etime" );
    AddMap( "length", "ArchiveTI-length" );
    AddMap( "rm", "ArchiveTI-rm" );
    AddMap( "state", "ArchiveTI-state" );
    AddMap( "scale", "ArchiveTI-scale" );
    AddMap( "type", "ArchiveTI-type" );
    AddMap( "dmc", "ArchiveTI-dmc" );
    AddMap( "rmc", "ArchiveTI-rmc" );
    AddMap( "polc", "ArchiveTI-polc" );
    AddMap( "freq", "ArchiveTI-freq" );
    AddMap( "bw", "ArchiveTI-bw" );
    AddMap( "intmjd", "ArchiveTI-intmjd" );
    AddMap( "fracmjd", "ArchiveTI-fracmjd" );
    AddMap( "para", "ArchiveTI-para" );
    AddMap( "tsub", "ArchiveTI-tsub" );

    AddGrp( "OBSERVER PARAMETERS" );
    AddMap( "observer", "ObsExtensionTI-observer" );
    AddMap( "projid", "ObsExtensionTI-projid" );

    AddGrp( "RECEIVER PARAMETERS" );
    AddMap( "ta", "ReceiverTI-ta" );
    AddMap( "fac", "ReceiverTI-fac" );
    AddMap( "basis", "ReceiverTI-basis" );
    AddMap( "hand", "ReceiverTI-hand" );
    AddMap( "rph", "ReceiverTI-rph" );
    AddMap( "oa", "ReceiverTI-oa" );
    AddMap( "recv_ra", "ReceiverTI-ra" );
    AddMap( "xo", "ReceiverTI-xo" );
    AddMap( "yo", "ReceiverTI-yo" );
    AddMap( "co", "ReceiverTI-co" );

    AddGrp( "TELESCOPE PARAMETERS" );
    AddMap( "ant_x", "ITRFExtensionTI-ant_x" );
    AddMap( "ant_y", "ITRFExtensionTI-ant_y" );
    AddMap( "ant_z", "ITRFExtensionTI-ant_z" );
    AddMap( "telescop", "ObsExtensionTI-telescop" );
    AddMap( "site", "ArchiveTI-site" );

    AddGrp( "BACKEND PARAMETERS" );
    AddMap( "backend", "BackendTI-name" );
    AddMap( "be_dcc", "BackendTI-dcc" );
    AddMap( "be_phase", "BackendTI-phase" );
    AddMap( "beconfig", "WidebandCorrelatorTI-beconfig" );
    AddMap( "tcycle", "WidebandCorrelatorTI-tcycle" );

    AddGrp( "PSRFITS PARAMETERS" );
    AddMap( "obs_mode", "FITSHdrExtensionTI-obs_mode" );
    AddMap( "hdrver", "FITSHdrExtensionTI-hdrver" );
    AddMap( "stt_date", "FITSHdrExtensionTI-stt_date" );
    AddMap( "stt_time", "FITSHdrExtensionTI-stt_time" );
    AddMap( "coord_md", "FITSHdrExtensionTI-coord_md" );
    AddMap( "equinox", "FITSHdrExtensionTI-equinox" );
    AddMap( "trk_mode", "FITSHdrExtensionTI-trk_mode" );
    AddMap( "bpa", "FITSHdrExtensionTI-bpa" );
    AddMap( "bmaj", "FITSHdrExtensionTI-bmaj" );
    AddMap( "bmin", "FITSHdrExtensionTI-bmin" );
    AddMap( "stt_imjd", "FITSHdrExtensionTI-stt_imjd" );
    AddMap( "stt_smjd", "FITSHdrExtensionTI-stt_smjd" );
    AddMap( "stt_offs", "FITSHdrExtensionTI-stt_offs" );
    AddMap( "ra", "FITSHdrExtensionTI-ra" );
    AddMap( "dec", "FITSHdrExtensionTI-dec" );
    AddMap( "stt_crd1", "FITSHdrExtensionTI-stt_crd1" );
    AddMap( "stt_crd2", "FITSHdrExtensionTI-stt_crd2" );
    AddMap( "stp_crd1", "FITSHdrExtensionTI-stp_crd1" );
    AddMap( "stp_crd2", "FITSHdrExtensionTI-stp_crd2" );

    AddGrp( "FLUXCAL" );
    AddMap( "nchan_fluxcal", "FluxCalibratorExtensionTI-nchan" );
    AddMap( "nrcvr_fluxcal", "FluxCalibratorExtensionTI-nrcvr" );
    AddMap( "epoch_fluxcal", "FluxCalibratorExtensionTI-epoch" );

    AddGrp( "HISTORY" );
    AddMap( "nbin_prd", "ProcHistoryTI-nbin_prd" );
    AddMap( "tbin", "ProcHistoryTI-tbin" );
    AddMap( "chbw", "ProcHistoryTI-chan_bw" );

    AddGrp( "BANDPASS" );
    AddMap( "npol_bp", "PassbandTI-npol" );
    AddMap( "nch_bp", "PassbandTI-nchan" );

    AddGrp( "FEED PARAMETERS" );
    AddMap( "npar_feed", "PolnCalibratorExtensionTI-npar" );
    AddMap( "nchan_feed", "PolnCalibratorExtensionTI-nchan" );
    AddMap( "MJD_feed", "PolnCalibratorExtensionTI-epoch" );

    AddGrp( "DIGITISER STATISTICS PARAMETERS" );
    AddMap( "ndigstat", "DigitiserStatisticsTI-ndigr" );
    AddMap( "npar_digstat", "DigitiserStatisticsTI-npar" );
    AddMap( "ncycsub", "DigitiserStatisticsTI-ncycsub" );
    AddMap( "levmode_digstat", "DigitiserStatisticsTI-diglev" );

    AddGrp( "DIGITISER COUNTS PARAMETERS" );
    AddMap( "dig_mode", "DigitiserCountsTI-dig_mode" );
    AddMap( "nlev_digcnts", "DigitiserCountsTI-nlev" );
    AddMap( "npthist", "DigitiserCountsTI-npthist" );
    AddMap( "levmode_digcnts", "DigitiserCountsTI-diglev" );

    AddGrp( "SUBINT PARAMETERS" );
    AddMap( "subint_type", "FITSSUBHdrExtensionTI-int_type" );
    AddMap( "subint_unit", "FITSSUBHdrExtensionTI-int_unit" );
    AddMap( "tsamp", "FITSSUBHdrExtensionTI-tsamp" );
    AddMap( "nbin_subint", "FITSSUBHdrExtensionTI-nbin" );
    AddMap( "nbits", "FITSSUBHdrExtensionTI-nbits" );
    AddMap( "nch_file", "FITSSUBHdrExtensionTI-nch_file" );
    AddMap( "nch_strt", "FITSSUBHdrExtensionTI-nch_strt" );
    AddMap( "npol_subint", "FITSSUBHdrExtensionTI-npol" );
    AddMap( "nsblk", "FITSSUBHdrExtensionTI-nsblk" );
  }
};


// NOTE - who would have thought is_numeric would be so complicated, need to find a better one, this comes nowhere
//        near what we require for vap.

bool is_numeric( string src )
{
  for( int i =0; i < src.size(); i ++ )
  {
    char next_char = src[i];
    if( next_char != '.' &&
        next_char != 'e' &&
        next_char != '-' &&
        next_char != '+' &&
        (next_char < char('0') || next_char > char('9') ) )
      return false;
  }

  return true;
}


// NOTE - These methods are just for formatting the output, In future I will create a class derived from ostream
//        that overrides the << operators and automatically places all the data into a table, then outputs it
//        correctly formatted when you call flush.

void FixColumnWidths( vector< vector< string > > &all_rows, vector< int > &col_widths )
{
  if( !results.size() )
    return;
  // start off setting the column widths to the size of the headings

  vector< string >::iterator hit;
  for( hit = (*all_rows.begin()).begin(); hit != (*all_rows.begin()).end(); hit ++ )
  {
    string heading = (*hit).substr( 0, (*hit).find_last_of('=') );
    col_widths.push_back( heading.size() + 1);
  }

  // check every cell, if the col width is to small for that cell, set it to the
  // cell width.

  vector< vector< string > >::iterator row_it;
  for( row_it = all_rows.begin(); row_it != all_rows.end(); row_it ++ )
  {
    int col = 0;
    vector< string >::iterator col_it;
    for( col_it = (*row_it).begin(); col_it != (*row_it).end(); col_it ++ )
    {
      string data = (*col_it).substr((*col_it).find_last_of('=')+1);
      if( data.size() + 1 > col_widths[col] )
        col_widths[col] = data.size() + 1;
      col ++;
    }
  }
}


void OutputTable( vector< vector< string > > &all_rows, vector< int > &col_widths )
{
  if( !results.size() )
    return;

  int col = 0;
  if( !hide_headers )
  {
    vector< string >::iterator hit;
    for( hit = (*all_rows.begin()).begin(); hit != (*all_rows.begin()).end(); hit ++ )
    {
      string heading = (*hit).substr( 0, (*hit).find_last_of('=') );
      cout << pad( col_widths[col], heading ) << ' ';
      col ++;
    }
    cout << endl;
  }

  vector< vector< string > >::iterator row_it;
  for( row_it = all_rows.begin(); row_it != all_rows.end(); row_it ++ )
  {
    col = 0;
    vector< string >::iterator col_it;
    for( col_it = (*row_it).begin(); col_it != (*row_it).end(); col_it ++ )
    {
      string data = (*col_it).substr((*col_it).find_last_of('=')+1);
      if( is_numeric( data ) )
      {
        for( int i = 0; i < col_widths[col] - data.size(); i ++ )
          cout << ' ';
        cout << data;
      }
      else
        cout << pad( col_widths[col], data );
      cout << ' ';
      col ++;
    }
    cout << endl;
  }
}


/////////////////////////////////////////////////////////////////////////






/**
 * PrintBasicHlp - message output when we get -h on command line.
 **/

void PrintBasicHlp( void )
{
  cout <<
  "A program for getting Pulsar::Archive attributes\n"
  "Usage:\n"
  "     vap -c PAR [-c PAR ...] [-E] [-p] filenames\n"
  "Where:\n"
  "\n"
  "PAR is a string containing one or more parameter names, separated by\n"
  "    commas.\n"
  "    If any whitespace is required, then the string containing it must\n"
  "    be enclosed in quotation marks. Multiple parameters may also be\n"
  "    specified by using multiple -c options.\n"
  "\n"
  "    vap -c name,freq\n"
  "\n"
  "    will print the source name and centre frequency.\n"
  "    Note that parameter names are case insensitive.\n"
  "    For a full list of parameter names, type \"vap -H\"\n"
  "\n"
  "-n  suppress outputting headers when doing -c\n"
  "\n"
  "-E  is used to print the most recent ephemeris in an archive\n"
  "\n"
  "-p  is used to print the set of polynomial coefficients\n"
  "\n"
  "-s show the extensions present in an archive\n"
  << endl;
}


/**
 * PrintExtdHelp - Show all the parameteres available via the text finder.
 **/

void PrintExtdHlp( void )
{
  VapTextFinder tf;
  tf.DisplayHelp();
}



/**
 * Process the command line options, return the index into argv of the first non option.
 **/

void ProcArgs( int argc, char *argv[] )
{
  int gotc;
  while ((gotc = getopt (argc, argv, "nc:sEphHvVtT")) != -1)
    switch (gotc)
    {

    case 'E':
      ephemmode = true;
      break;

    case 'p':
      polycmode = true;
      break;

    case 'c':
      separate (optarg, commands, " ,");
      break;
    case 'h':
      PrintBasicHlp();
      break;

    case 'H':
      PrintExtdHlp();
      break;

    case 'v':
      verbose = true;
      Pulsar::Archive::set_verbosity(2);
      break;

    case 'V':
      verbose = true;
      Error::verbose = true;
      Pulsar::Archive::set_verbosity(3);
      break;

    case 'n':
      hide_headers = true;
      break;

    case 's':
      show_extensions = true;
      break;

    default:
      cerr << "Unknown command line option" << endl;
      return;
    };
}



/**
 * ProcessArchive - load an archive and process all the command line parameters using the text finder.
 **/

void ProcessArchive( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  current_row.push_back( string("File=") + filename );

  VapTextFinder tf;
  tf.SetArchive( archive );

  vector< string >::iterator it;
  for( it = commands.begin(); it != commands.end(); it ++ )
  {
    current_row.push_back( tf.FetchValue( lowercase((*it)) ) );
  }

  results.push_back( current_row );
  current_row.clear();
}




/**
 * ExtractPolyco - Get the polyco data from the archive, this will be moved elsewhere
 **/

void ExtractPolyco( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  cout << endl << filename << endl;

  archive->get_model()->unload( stdout );
}



/**
 * ExtractEphemeris - Get the ephemeris, this will be moved elsewhere.
 **/

void ExtractEphemeris( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  cout << endl << filename << endl;

  archive->get_ephemeris()->unload( stdout );
}



/**
 * ShowExtensions - a simple debugging routine for showing the extensions available on an archive.
 **/

void ShowExtensions( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  cout << "Extensions for " << filename << endl;

  unsigned int num_extensions = archive->get_nextension();
  for( unsigned int i = 0; i < num_extensions; i ++ )
  {
    Reference::To< Archive::Extension > next_ext = archive->get_extension(i);
    cout << next_ext->get_extension_name() << endl;
  }
}



/**
 * main - 
 **/

int main( int argc, char *argv[] )
{
  tostring_precision = 4;
  Angle::default_type = Angle::Degrees;
  
  ProcArgs( argc, argv );

  vector< string > filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);


  if( filenames.size() )
  {
    if( polycmode )
      for_each( filenames.begin(), filenames.end(), ExtractPolyco );
    else if( ephemmode )
      for_each( filenames.begin(), filenames.end(), ExtractEphemeris );
    else if( show_extensions )
      for_each( filenames.begin(), filenames.end(), ShowExtensions );
    else
      for_each( filenames.begin(), filenames.end(), ProcessArchive );

    vector< int > column_widths;

    FixColumnWidths( results, column_widths );
    OutputTable( results, column_widths );
  }

  return 0;
}


