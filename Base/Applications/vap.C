/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>

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
#include <Pulsar/Pointing.h>
#include <TextInterface.h>
#include <Pulsar/Passband.h>
#include <Pulsar/PolnCalibratorExtension.h>
#include <Pulsar/DigitiserStatistics.h>
#include <Pulsar/DigitiserCounts.h>
#include <Pulsar/FITSSUBHdrExtension.h>
#include <Pulsar/CalInfoExtension.h>

#include <dirutil.h>
#include <strutil.h>
#include <tostring.h>
#include <Angle.h>
#include <table_stream.h>


#include <unistd.h>

#include <sstream>

#include <ctime>
#include <utc.h>
#include <FITSUTC.h>


/**
 * For the sake of cleanliness, readability and common sense, I decided to concede that the text interfaces
 * don't provide the facilities requested of the old vap. So if you are going to try to clean up this code
 * be forewarned that my attempt to clean the code resulted in far more code to deal with all the exceptions
 * that arise, as well as a great deal of lost time. KISS
 * 
 * David Smith nopeer@gmail.com
 **/

using namespace std;
using namespace Pulsar;


////////////////////////////////////////////////////////////////////////////////////////////
// PRECISION FOR tostring
////////////////////////////////////////////////////////////////////////////////////////////


unsigned int old_precision;
bool old_places;

void set_precision( unsigned int num_digits, unsigned int places = true )
{
  old_precision = tostring_precision;
  old_places = tostring_places;

  tostring_precision = num_digits;
  tostring_places = places;
}


void restore_precision( void )
{
  tostring_precision = old_precision;
  tostring_places = old_places;
}


////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR RETREIVING OBSERVATION PARAMETERS
////////////////////////////////////////////////////////////////////////////////////////////



string get_name( Reference::To< Archive > archive )
{
  return archive->get_source();
}

string get_stime( Reference::To< Archive > archive )
{
  return string("TODO");
}

string get_etime( Reference::To< Archive > archive )
{
  return string("TODO" );
}

string get_length( Reference::To< Archive > archive )
{
  tostring_precision = 6;
  return tostring<double>( archive->integration_length() );
}

string get_nbin_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    result = tostring<int>( first.nbin );
  }

  return result;
}

string get_nchan_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    result = tostring<int>( first.nchan );
  }

  return result;
}

string get_npol_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    result = tostring<int>( first.npol );
  }

  return result;
}

string get_nsub_obs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
  {
    ProcHistory::row first;
    first = ( *(ext->rows.begin()) );
    if( first.nsub == 0 )
      result = "*";
    else
      result = tostring<int>( first.nsub );
  }

  return result;
}

string get_dm( Reference::To< Archive > archive )
{
  return tostring<double>( archive->get_dispersion_measure() );
}

string get_rm( Reference::To< Archive > archive )
{
  return tostring<double>( archive->get_rotation_measure() );
}

string get_state( Reference::To< Archive > archive )
{
  return tostring<Signal::State>( archive->get_state() );
}


string get_scale( Reference::To< Archive > archive )
{
  return tostring<Signal::Scale>( archive->get_scale() );
}


string get_type( Reference::To< Archive > archive )
{
  return tostring<Signal::Source>( archive->get_type() );
}


string get_dmc( Reference::To< Archive > archive )
{
  return tostring<bool>( archive->get_dedispersed() );
}


string get_rmc( Reference::To< Archive > archive )
{
  return tostring<bool>( archive->get_faraday_corrected() );
}


string get_polc( Reference::To< Archive > archive )
{
  return tostring<bool>( archive->get_poln_calibrated() );
}


string get_freq( Reference::To< Archive > archive )
{
  ostringstream result;

  double cf = archive->get_centre_frequency();
  result << setiosflags( ios::fixed ) << setprecision(3) << cf;

  return result.str();
}

string get_bw( Reference::To< Archive > archive )
{
  set_precision( 3 );

  string result = tostring<double>( archive->get_bandwidth() );

  restore_precision();

  return result;
}


string get_intmjd( Reference::To< Archive > archive )
{
  return tostring<int>( archive->start_time_day() );
}


string get_fracmjd( Reference::To< Archive > archive )
{
  return tostring<double>( archive->start_time_fracday() );
}


string get_parang( Reference::To< Archive > archive )
{
  stringstream result;

  int nsubs = archive->get_nsubint();

  if( nsubs != 0 )
  {
    Reference::To< Integration > integration = archive->get_Integration( nsubs / 2 );
    if( integration )
    {
      Reference::To< Pointing > ext = integration->get<Pointing>();

      if( ext )
      {
        result << ext->get_parallactic_angle().getDegrees();
      }
    }
  }

  return result.str();
}


string get_tsub( Reference::To< Archive > archive )
{
  string result;

  int nsubs = archive->get_nsubint();
  if( nsubs != 0 )
  {
    Reference::To< Integration > first_int = archive->get_first_Integration();
    if( first_int )
    {
      set_precision( 6 );
      result = tostring<double>( first_int->get_duration() );
      restore_precision();
    }
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// OBSERVER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_observer( Reference::To<Archive> archive )
{
  string result;
  Reference::To< ObsExtension > ext = archive->get<ObsExtension>();

  if( !ext )
  {
    result = "UNDEF";
  }
  else
  {
    result = ext->get_observer();
  }

  return result;
}



string get_projid( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<ObsExtension> ext = archive->get<ObsExtension>();

  if( !ext )
  {
    result = "UNDEF";
  }
  else
  {
    result = ext->get_project_ID();
  }

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// RECEIVER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


string get_rcvr( Reference::To<Archive> archive )
{
  string result = "TODO";

  Reference::To<Receiver> ext = archive->get<Receiver>();

  if( ext )
  {
    result = ext->get_name();
  }

  return result;
}

string get_nrcvr( Reference::To<Archive> archive )
{
  string result = "TODO";

  Reference::To<Receiver> ext = archive->get<Receiver>();

  if( ext )
  {
    result = tostring<int>( ext->get_nrcvr() );
  }

  return result;
}

string get_ta( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_tracking_angle() );

  return result;
}

string get_fac( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<bool>( recv->get_feed_corrected() );

  return result;
}

string get_basis( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Signal::Basis>( recv->get_basis() );

  return result;
}

string get_fd_hand( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Signal::Hand>( recv->get_hand() );

  return result;
}

string get_fd_xyph( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_reference_source_phase() );

  return result;
}

string get_oa( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_orientation() );

  return result;
}

string get_fd_sang( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_field_orientation() );

  return result;
}

string get_xoffset( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_X_offset() );

  return result;
}

string get_yo( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_Y_offset() );

  return result;
}

string get_co( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<Receiver> recv = archive->get<Receiver>();

  if( !recv )
    result = "UNDEF";
  else
    result = tostring<Angle>( recv->get_calibrator_offset() );

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// TELESCOPE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_ant_x( Reference::To< Archive > archive )
{
  string result = "";
  Reference::To<ITRFExtension> ext = archive->get<ITRFExtension>();

  set_precision( 6 );

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->ant_x );

  restore_precision();

  return result;
}

string get_ant_y( Reference::To< Archive > archive )
{
  string result = "";
  Reference::To<ITRFExtension> ext = archive->get<ITRFExtension>();

  set_precision( 3, false );

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->ant_y );

  restore_precision();

  return result;
}

string get_ant_z( Reference::To< Archive > archive )
{
  string result = "";
  Reference::To<ITRFExtension> ext = archive->get<ITRFExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->ant_z );

  if( tostring_places == true ) return "f";

  return result;
}

string get_telescop( Reference::To< Archive > archive )
{
  string result = "";
  Reference::To<ObsExtension> ext = archive->get<ObsExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_telescope();

  return result;
}

string get_date( Reference::To< Archive > archive )
{
  string result = "";

  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_creation_date();

  return result;
}

string get_site( Reference::To< Archive > archive )
{
  string result = archive->get_telescope_code();

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// BACKEND FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////


string get_backend( Reference::To< Archive > archive )
{
  string result = "";

  Reference::To<Backend> ext;
  ext = archive->get<Backend>();
  if( !ext )
  {
    ext = archive->get<WidebandCorrelator>();
  }

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_name();

  return result;
}

string get_be_dcc( Reference::To< Archive > archive )
{
  string result = "";
  Reference::To<Backend> ext;
  ext = archive->get<Backend>();
  if( !ext )
  {
    ext = archive->get<WidebandCorrelator>();
  }

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<bool>( ext->get_downconversion_corrected() );

  return result;
}

string get_be_phase( Reference::To< Archive > archive )
{
  string result = "TODO";
  Reference::To<Backend> ext;
  ext = archive->get<Backend>();
  if( !ext )
  {
    ext = archive->get<WidebandCorrelator>();
  }

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<Signal::Argument>( ext->get_argument() );

  return result;
}

string get_beconfig( Reference::To< Archive > archive )
{
  string result = "TODO";
  Reference::To<WidebandCorrelator> ext = archive->get<WidebandCorrelator>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_config();

  return result;
}

string get_tcycle( Reference::To< Archive > archive )
{
  string result = "TODO";
  Reference::To<WidebandCorrelator> ext = archive->get<WidebandCorrelator>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_tcycle() );

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// PSRFITS SPECIFIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////




string get_obs_mode( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_obs_mode();

  return result;
}


string get_hdrver( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_hdrver();

  return result;
}

string get_stt_date( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_stt_date();

  return result;
}

string get_stt_time( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_stt_time();

  return result;
}


string get_stt_lst( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  set_precision( 10 );

  if( ext )
    result = tostring<double>( ext->get_stt_lst() );

  restore_precision();

  return result;
}

string get_coord_md( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_coordmode();

  return result;
}

string get_equinox( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_equinox();

  return result;
}

string get_trk_mode( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_trk_mode();

  return result;
}

string get_bpa( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_bpa() );

  return result;
}

string get_bmaj( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  set_precision(3);
  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_bmaj() );
  restore_precision();

  return result;
}

string get_bmin( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  set_precision( 3 );
  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_bmin() );
  restore_precision();

  return result;
}

string get_stt_imjd( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_stt_imjd() );

  return result;
}

string get_stt_smjd( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_stt_smjd() );

  return result;
}

string get_stt_offs( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_stt_offs() );

  return result;
}

string get_ra( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_ra();

  return result;
}

string get_dec( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_dec();

  return result;
}

string get_stt_crd1( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_stt_crd1();

  return result;
}

string get_stt_crd2( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_stt_crd2();

  return result;
}

string get_stp_crd1( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_stp_crd1();

  return result;
}

string get_stp_crd2( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSHdrExtension> ext = archive->get<FITSHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_stp_crd2();

  return result;
}





////////////////////////////////////////////////////////////////////////////////////////////
// FLUXCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_nchan_fluxcal( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<FluxCalibratorExtension> ext = archive->get<FluxCalibratorExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nchan() );

  return result;
}

string get_nrcvr_fluxcal( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FluxCalibratorExtension> ext = archive->get<FluxCalibratorExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nreceptor() );

  return result;
}

string get_epoch_fluxcal( Reference::To<Archive> archive )
{
  string result = "";
  Reference::To<FluxCalibratorExtension> ext = archive->get<FluxCalibratorExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_epoch() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// HISTORY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_nbin_prd( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
    result = tostring<unsigned int>( ext->get_last_nbin_prd() );

  return result;
}

string get_tbin( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
    result = tostring<double>( ext->get_last_tbin() );

  return result;
}

string get_chbw( Reference::To<Archive> archive )
{
  string result = "UNDEF";
  Reference::To<ProcHistory> ext = archive->get<ProcHistory>();

  if( ext )
    result = tostring<double>( ext->get_last_chan_bw() );

  return result;
}

string get_nbin( Reference::To< Archive > archive )
{
  return tostring<unsigned int>( archive->get_nbin() );
}

string get_nchan( Reference::To< Archive > archive )
{
  return tostring<unsigned int>( archive->get_nchan() );
}

string get_npol( Reference::To< Archive > archive )
{
  return tostring<unsigned int>( archive->get_npol() );
}

string get_nsub( Reference::To< Archive > archive )
{
  return tostring<unsigned int>( archive->get_nsubint() );
}



////////////////////////////////////////////////////////////////////////////////////////////
// BANDPASS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_npol_bp( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<Passband> ext = archive->get<Passband>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_npol() );

  return result;
}

string get_nch_bp( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<Passband> ext = archive->get<Passband>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nchan() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// FEED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_npar_feed( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<PolnCalibratorExtension> ext = archive->get<PolnCalibratorExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_ncpar() );

  return result;
}

string get_nchan_feed( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<PolnCalibratorExtension> ext = archive->get<PolnCalibratorExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nchan() );

  return result;
}

string get_MJD_feed( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<PolnCalibratorExtension> ext = archive->get<PolnCalibratorExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<MJD>( ext->get_epoch() );

  return result;
}



////////////////////////////////////////////////////////////////////////////////////////////
// DIGITISER STATISTICS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_ndigstat( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_ndigr() );

  return result;
}

string get_npar_digstat( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_npar() );

  return result;
}

string get_ncycsub( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_ncycsub() );

  return result;
}

string get_levmode_digstat( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserStatistics> ext = archive->get<DigitiserStatistics>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<string>( ext->get_diglev() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// DIGITISER COUNTS FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_dig_mode( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<string>( ext->get_dig_mode() );

  return result;
}

string get_nlev_digcnts( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nlev() );

  return result;
}

string get_npthist( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_npthist() );

  return result;
}

string get_levmode_digcnts( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<DigitiserCounts> ext = archive->get<DigitiserCounts>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<string>( ext->get_diglev() );

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////
// SUBINT FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_subint_type( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_int_type();

  return result;
}

string get_subint_unit( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = ext->get_int_unit();

  return result;
}

string get_tsamp( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<double>( ext->get_tsamp() );

  return result;
}

string get_nbin_subint( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nbin() );

  return result;
}

string get_nbits( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nbits() );

  return result;
}

string get_nch_file( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nch_file() );

  return result;
}

string get_nch_strt( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nch_strt() );

  return result;
}

string get_npol_subint( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_npol() );

  return result;
}

string get_nsblk( Reference::To<Archive> archive )
{
  string result = "TODO";
  Reference::To<FITSSUBHdrExtension> ext = archive->get<FITSSUBHdrExtension>();

  if( !ext )
    result = "UNDEF";
  else
    result = tostring<unsigned int>( ext->get_nsblk() );

  return result;
}




////////////////////////////////////////////////////////////////////////////////////////////
// CALIBRATION FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

string get_cal_mode( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();

  if( ext )
  {
    result = ext->cal_mode;
  }

  return result;
}


string get_cal_freq( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();

  if( ext )
  {
    set_precision( 3 );
    result = tostring<double>( ext->cal_frequency );
    restore_precision();
  }


  return result;
}


string get_cal_dcyc( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();

  if( ext )
  {
    set_precision(3);
    result = tostring<double>( ext->cal_dutycycle );
    restore_precision();
  }

  return result;
}


string get_cal_phs( Reference::To<Archive> archive )
{
  string result = "UNDEF";

  Reference::To<CalInfoExtension> ext = archive->get<CalInfoExtension>();

  if( ext )
  {
    set_precision(3);
    result = tostring<double>( ext->cal_phase );
    restore_precision();
  }

  return result;
}




bool ephemmode = false;
bool polycmode = false;
bool verbose = false;
bool show_extensions = false;
bool hide_headers = false;
vector< string > commands;
vector< vector< string > > results;
vector< string > current_row;
bool new_new_vap = false;

table_stream ts(&cout);







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
  cout << "" << endl;

  cout << "BACKEND PARAMETERS" << endl;
  cout << "backend                         Name of the backend instrument" << endl;
  cout << "be_dcc                          Downconversion conjugation corrected" << endl;
  cout << "be_phase                        Phase convention of backend" << endl;
  cout << "beconfig                        Backend Config file" << endl;
  cout << "tcycle                          Correlator cycle time" << endl;
  cout << "" << endl;

  cout << "BANDPASS PARAMETERS" << endl;
  cout << "nch_bp                          Number of channels in original bandpass" << endl;
  cout << "npol_bp                         Number of polarizations in bandpass" << endl;
  cout << "" << endl;

  cout << "OBSERVATION PARAMETERS" << endl;
  cout << "bw                              Bandwidth (MHz)" << endl;
  cout << "dmc                             Dispersion corrected (boolean)" << endl;
  cout << "length                          The full duration of the observation (s)" << endl;
  cout << "name                            Name of the source" << endl;
  cout << "nbin_obs                        Number of pulse phase bins" << endl;
  cout << "nchan_obs                       Number of frequency channels" << endl;
  cout << "npol_obs                        Number of polarizations" << endl;
  cout << "nsub_obs                        Number of Sub-Integrations" << endl;
  cout << "obs_mode                        Observation Mode (PSR, CAL, SEARCH)" << endl;
  cout << "polc                            Polarization calibrated (boolean)" << endl;
  cout << "rm                              Rotation measure (rad/m^2)" << endl;
  cout << "dm                              Dispersion measure" << endl;
  cout << "rmc                             Faraday Rotation corrected (boolean)" << endl;
  cout << "scale                           Units of profile amplitudes" << endl;
  cout << "state                           State of profile amplitudes" << endl;
  cout << "tsub                            The duration of the first subint (s)" << endl;
  cout << "type                            Observation type (Pulsar, PolnCal, etc.)" << endl;
  cout << endl;

  cout << "DIGITISER COUNTS PARAMETERS" << endl;
  cout << "dig_mode                        Digitiser mode" << endl;
  cout << "levmode_digcnts                 Digitiser level-setting mode (AUTO, FIX)" << endl;
  cout << "nlev_digcnts                    Number of digitiser levels" << endl;
  cout << "npthist                         Number of points in histogram (I)" << endl;
  cout << endl;

  cout << "DIGITISER STATISTICS PARAMETERS" << endl;
  cout << "levmode_digstat                 Digitiser level-setting mode (AUTO, FIX)" << endl;
  cout << "ncycsub                         Number of correlator cycles per subint" << endl;
  cout << "ndigstat                        Number of digitised channels (I)" << endl;
  cout << "npar_digstat                    Number of digitiser parameters" << endl;
  cout << "" << endl;

  cout << "FEED COUPLING PARAMETERS" << endl;
  cout << "MJD_feed                        [MJD] Epoch of calibration obs" << endl;
  cout << "nchan_feed                      Nr of channels in Feed coupling data" << endl;
  cout << "npar_feed                       Number of coupling parameters" << endl;
  cout << endl;

  cout << "FLUXCAL" << endl;
  cout << "epoch_fluxcal                   [MJD] Epoch of calibration obs" << endl;
  cout << "nchan_fluxcal                   Nr of frequency channels (I)" << endl;
  cout << "nrcvr_fluxcal                   Number of receiver channels (I)" << endl;
  cout << endl;

  cout << "HISTORY" << endl;
  cout << "chbw                            Channel bandwidth" << endl;
  cout << "freq                            Centre frequency (MHz)" << endl;
  cout << "nchan                           Number of frequency channels" << endl;
  cout << "nbin                            Number of pulse phase bins" << endl;
  cout << "nbin_prd                        Nr of bins per period" << endl;
  cout << "npol                            Number of polarizations" << endl;
  cout << "nsub                            Number of sub-integrations" << endl;
  cout << "tbin                            Time per bin or sample" << endl;
  cout << endl;

  cout << "OBSERVER PARAMETERS" << endl;
  cout << "observer                        Observer name(s)" << endl;
  cout << "projid                          Project name" << endl;
  cout << endl;

  cout << "COORDINATES & TIMES" << endl;
  cout << "coord_md                        The coordinate mode (J2000, GAL, ECLIP, etc )." << endl;
  cout << "dec                             Declination (-dd:mm:ss.sss)" << endl;
  cout << "equinox                         Equinox of coords (2000.0)" << endl;
  cout << "fracmjd                         MJD faction of day" << endl;
  cout << "intmjd                          MJD day" << endl;
  cout << "parang                          Parallactic angle at archive mid point" << endl;
  cout << "ra                              Right ascension (hh:mm:ss.ssss)" << endl;
  cout << "stp_crd1                        Stop coord 1 (hh:mm:ss.sss or ddd.ddd)" << endl;
  cout << "stp_crd2                        Stop coord 2 (-dd:mm:ss.sss or -dd.ddd)" << endl;
  cout << "stt_crd1                        Start coord 1 (hh:mm:ss.sss or ddd.ddd)" << endl;
  cout << "stt_crd2                        Start coord 2 (-dd:mm:ss.sss or -dd.ddd)" << endl;
  cout << "stt_date                        Start UT date (YYYY-MM-DD)" << endl;
  cout << "stt_imjd                        Start MJD (UTC days) (J - long integer)" << endl;
  cout << "stt_lst                         Start LST (hh:mm:ss)" << endl;
  cout << "stt_offs                        [s] Start time offset (D)" << endl;
  cout << "stt_smjd                        [s] Start time (sec past UTC 00h) (J)" << endl;
  cout << "stt_time                        Start UT (hh:mm:ss)" << endl;
  cout << "trk_mode                        Track mode ( TRACK, SCANGC, SCANLAT )" << endl;
  cout << endl;

  cout << "FEED & RECEIVER PARAMETERS" << endl;
  cout << "rcvr                            Name of receiver" << endl;
  cout << "basis                           Basis of receptors" << endl;
  cout << "fac                             Feed angle corrected" << endl;
  cout << "fd_hand                         Hand of receptor basis" << endl;
  cout << "fd_sang                         Feed symmetry angle (rcvr:ra)" << endl;
  cout << "fd_xyph                         Reference source phase (rcvr:rph)" << endl;
  cout << "nrcpt                           Number of receptors" << endl;
  cout << "ta                              Tracking angle of feed" << endl;
  cout << "xoffset                         Offset of feed X-axis wrt platform zero" << endl;
  cout << endl;

  cout << "SUBINT PARAMETERS" << endl;
  cout << "nch_file                        Number of channels/sub-bands in this file" << endl;
  cout << "nch_strt                        Start channel/sub-band number (0 to NCHAN-1)" << endl;
  cout << "nbin_subint                     Nr of bins (PSR/CAL mode; else 1)" << endl;
  cout << "nbits                           Nr of bits/datum (SEARCH mode 'X' data, else 1)" << endl;
  cout << "npol_subint                     Nr of polarisations in table" << endl;
  cout << "nsblk                           Samples/row (SEARCH mode, else 1)" << endl;
  cout << "subint_type                     Time axis (TIME, BINPHSPERI, BINLNGASC, etc)" << endl;
  cout << "subint_unit                     Unit of time axis (SEC, PHS (0-1), DEG)" << endl;
  cout << "tsamp                           [s] Sample interval for SEARCH-mode data" << endl;
  cout << endl;

  cout << "FILE & TELESCOPE PARAMETERS" << endl;
  cout << "ant_x                           ITRF X coordinate." << endl;
  cout << "ant_y                           ITRF Y coordinate." << endl;
  cout << "ant_z                           ITRF Z coordinate." << endl;
  cout << "bmaj                            [deg] beam major axis" << endl;
  cout << "bmin                            [deg] beam minor axis" << endl;
  cout << "bpa                             [deg] beam position angle" << endl;
  cout << "date                            File creation date" << endl;
  cout << "hdrver                          Header Version" << endl;
  cout << "site                            Telescope tempo code" << endl;
  cout << "telescop                        Telescope name" << endl;
  cout << endl;

  cout << "CALIBRATION PARAMETERS" << endl;
  cout << "cal_dcyc                        Cal duty cycle (E)" << endl;
  cout << "cal_freq                        [Hz] Cal modulation frequency (E)" << endl;
  cout << "cal_mode                        Cal mode (OFF, SYNC, EXT1, EXT2)" << endl;
  cout << "cal_phs                         Cal phase (wrt start time) (E)" << endl;
  cout << endl;

}




void Test( void )
{
  string src = "2007-07-24T06:35:26";

  FITSUTC thetime( src );
  cout << thetime << endl;

  MJD target = FITSUTC( string("2007-07-24T06:35:26") );
  cout << FITSUTC( target ) << endl;
}


/**
* Process the command line options, return the index into argv of the first non option.
**/

void ProcArgs( int argc, char *argv[] )
{
  int gotc;
  while ((gotc = getopt (argc, argv, "nc:sEphHvVtTX")) != -1)
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

    case 't':
      Test();
      break;

    default:
      cerr << "Unknown command line option" << endl;
      return;
    };
}




string FetchValue( Reference::To< Archive > archive, string command )
{
  try
  {
    if( command == "name" ) return get_name( archive );
    else if( command == "nbin" ) return get_nbin( archive );
    else if( command == "nchan" ) return get_nchan( archive );
    else if( command == "npol" ) return get_npol( archive );
    else if( command == "nsub" ) return get_nsub( archive );
    else if( command == "stime" ) return get_stime( archive );
    else if( command == "etime" ) return get_etime( archive );
    else if( command == "length" ) return get_length( archive );
    else if( command == "nbin_obs" ) return get_nbin_obs( archive );
    else if( command == "nchan_obs" ) return get_nchan_obs( archive );
    else if( command == "npol_obs" ) return get_npol_obs( archive );
    else if( command == "nsub_obs" ) return get_nsub_obs( archive );
    else if( command == "dm" ) return get_dm( archive );
    else if( command == "rm" ) return get_rm( archive );
    else if( command == "state" ) return get_state( archive );
    else if( command == "scale" ) return get_scale( archive );
    else if( command == "type" ) return get_type( archive );
    else if( command == "dmc" ) return get_dmc( archive );
    else if( command == "rmc" ) return get_rmc( archive );
    else if( command == "polc" ) return get_polc( archive );
    else if( command == "freq" ) return get_freq( archive );
    else if( command == "bw" ) return get_bw( archive );
    else if( command == "intmjd" ) return get_intmjd( archive );
    else if( command == "fracmjd" ) return get_fracmjd( archive );
    else if( command == "parang" ) return get_parang( archive );
    else if( command == "tsub" ) return get_tsub( archive );
    else if( command == "observer" ) return get_observer( archive );
    else if( command == "projid" ) return get_projid( archive );
    else if( command == "ta" ) return get_ta( archive );
    else if( command == "fac" ) return get_fac( archive );
    else if( command == "rcvr" ) return get_rcvr( archive );
    else if( command == "nrcvr" ) return get_nrcvr( archive );
    else if( command == "basis" ) return get_basis( archive );
    else if( command == "fd_hand" ) return get_fd_hand( archive );
    else if( command == "fd_xyph" ) return get_fd_xyph( archive );
    else if( command == "oa" ) return get_oa( archive );
    else if( command == "fd_sang" ) return get_fd_sang( archive );
    else if( command == "xoffset" ) return get_xoffset( archive );
    else if( command == "yo" ) return get_yo( archive );
    else if( command == "co" ) return get_co( archive );
    else if( command == "ant_x" ) return get_ant_x( archive );
    else if( command == "ant_y" ) return get_ant_y( archive );
    else if( command == "ant_z" ) return get_ant_z( archive );
    else if( command == "telescop" ) return get_telescop( archive );
    else if( command == "site" ) return get_site( archive );
    else if( command == "backend" ) return get_backend( archive );
    else if( command == "be_dcc" ) return get_be_dcc( archive );
    else if( command == "be_phase" ) return get_be_phase( archive );
    else if( command == "beconfig" ) return get_beconfig( archive );
    else if( command == "tcycle" ) return get_tcycle( archive );
    else if( command == "obs_mode" ) return get_obs_mode( archive );
    else if( command == "hdrver" ) return get_hdrver( archive );
    else if( command == "stt_date" ) return get_stt_date( archive );
    else if( command == "stt_time" ) return get_stt_time( archive );
    else if( command == "stt_lst" ) return get_stt_lst( archive );
    else if( command == "coord_md" ) return get_coord_md( archive );
    else if( command == "equinox" ) return get_equinox( archive );
    else if( command == "trk_mode" ) return get_trk_mode( archive );
    else if( command == "bpa" ) return get_bpa( archive );
    else if( command == "bmaj" ) return get_bmaj( archive );
    else if( command == "bmin" ) return get_bmin( archive );
    else if( command == "stt_imjd" ) return get_stt_imjd( archive );
    else if( command == "stt_smjd" ) return get_stt_smjd( archive );
    else if( command == "stt_offs" ) return get_stt_offs( archive );
    else if( command == "ra" ) return get_ra( archive );
    else if( command == "dec" ) return get_dec( archive );
    else if( command == "stt_crd1" ) return get_stt_crd1( archive );
    else if( command == "stt_crd2" ) return get_stt_crd2( archive );
    else if( command == "stp_crd1" ) return get_stp_crd1( archive );
    else if( command == "stp_crd2" ) return get_stp_crd2( archive );
    else if( command == "nbin_prd" ) return get_nbin_prd( archive );
    else if( command == "tbin" ) return get_tbin( archive );
    else if( command == "chbw" ) return get_chbw( archive );
    else if( command == "npol_bp" ) return get_npol_bp( archive );
    else if( command == "nch_bp" ) return get_nch_bp( archive );
    else if( command == "npar_feed" ) return get_npar_feed( archive );
    else if( command == "nchan_feed" ) return get_nchan_feed( archive );
    else if( command == "mjd_feed" ) return get_MJD_feed( archive );
    else if( command == "ndigstat" ) return get_ndigstat( archive );
    else if( command == "npar_digstat" ) return get_npar_digstat( archive );
    else if( command == "ncycsub" ) return get_ncycsub( archive );
    else if( command == "levmode_digstat" ) return get_levmode_digstat( archive );
    else if( command == "dig_mode" ) return get_dig_mode( archive );
    else if( command == "nlev_digcnts" ) return get_nlev_digcnts( archive );
    else if( command == "npthist" ) return get_npthist( archive );
    else if( command == "levmode_digcnts" ) return get_levmode_digcnts( archive );
    else if( command == "subint_type" ) return get_subint_type( archive );
    else if( command == "subint_unit" ) return get_subint_unit( archive );
    else if( command == "tsamp" ) return get_tsamp( archive );
    else if( command == "nbin_subint" ) return get_nbin_subint( archive );
    else if( command == "nbits" ) return get_nbits( archive );
    else if( command == "nch_file" ) return get_nch_file( archive );
    else if( command == "nch_strt" ) return get_nch_strt( archive );
    else if( command == "npol_subint" ) return get_npol_subint( archive );
    else if( command == "nsblk" ) return get_nsblk( archive );
    else if( command == "date" ) return get_date( archive );
    else if( command == "cal_mode" ) return get_cal_mode( archive );
    else if( command == "cal_freq" ) return get_cal_freq( archive );
    else if( command == "cal_dcyc" ) return get_cal_dcyc( archive );
    else if( command == "cal_phs" ) return get_cal_phs( archive );

    else return "UNDEF";
  }
  catch( Error e )
  {
    return "*error*";
  }


}



/**
* ProcessArchive - load an archive and process all the command line parameters using the text finder.
**/

void ProcessArchive( string filename )
{
  Reference::To< Archive > archive;
  try
  {
    archive = Archive::load( filename );
  }
  catch ( Error e )
  {
    cerr << "failed to load archive " << filename << endl;
    if( verbose )
    {
      cerr << e << endl;
    }
  }

  if( !archive )
    return;

  ts << filename;

  vector< string >::iterator it;
  for( it = commands.begin(); it != commands.end(); it ++ )
  {
    string val = FetchValue( archive, lowercase((*it)) );
    if ( val == "" ) val = "*";
    ts << val;
  }

  ts << endrow;
}




/**
* ExtractPolyco - Get the polyco data from the archive, this will be moved elsewhere
**/

void ExtractPolyco( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

  cout << filename << " has polyco:" << endl << endl;

  archive->get_model()->unload( stdout );
  
  cout << endl;
}



/**
* ExtractEphemeris - Get the ephemeris, this will be moved elsewhere.
**/

void ExtractEphemeris( string filename )
{
  Reference::To< Archive > archive = Archive::load( filename );

  if( !archive )
    return;

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

  // load files more quickly by ignoring profile data
  Pulsar::Profile::no_amps = true;

  ProcArgs( argc, argv );

  vector< string > filenames;
  for (int ai=optind; ai<argc; ai++)
    dirglob (&filenames, argv[ai]);

  // first heading is always the filename
  ts << "filename";

  // add the commands as headings for the table.
  vector< string >::iterator it;
  for( it = commands.begin(); it != commands.end(); it ++ )
    ts << (*it);
  ts << endrow;

  if( filenames.size() )
  {
    if( polycmode )
      for_each( filenames.begin(), filenames.end(), ExtractPolyco );
    else if( ephemmode )
      for_each( filenames.begin(), filenames.end(), ExtractEphemeris );
    else if( show_extensions )
      for_each( filenames.begin(), filenames.end(), ShowExtensions );
    else
    {
      for_each( filenames.begin(), filenames.end(), ProcessArchive );
      ts.flush();
    }
  }

  return 0;
}


