




#include <Pulsar/FITSHdrExtensionTI.h>





namespace Pulsar
{

  FITSHdrExtensionTI::FITSHdrExtensionTI()
  {
    SetupMethods();
  }

  FITSHdrExtensionTI::FITSHdrExtensionTI( FITSHdrExtension *c )
  {
    SetupMethods();
    set_instance(c);
  }
  void FITSHdrExtensionTI::SetupMethods( void )
  {
    add( &FITSHdrExtension::get_obs_mode, "obs_mode", "Observation Mode (PSR, CAL, SEARCH)" );
    add( &FITSHdrExtension::get_obsfreq, "obsfreq", "[MHz] Centre frequency for observation" );
    add( &FITSHdrExtension::get_hdrver, "hdrver", "Header Version" );
    add( &FITSHdrExtension::get_creation_date, "date", "File Creation Date" );
    add( &FITSHdrExtension::get_coordmode, "coord_md", "The coordinate mode (EQUAT, GAL, ECLIP, etc )." );
    add( &FITSHdrExtension::get_equinox, "equinox", "Equinox of coords (J2000, B1950)" );
    add( &FITSHdrExtension::get_trk_mode, "trk_mode", "Track mode ( TRACK, SCANGC, SCANLAT )" );
    add( &FITSHdrExtension::get_bpa, "bpa", "[deg] beam position angle" );
    add( &FITSHdrExtension::get_bmaj, "bmaj", "[deg] beam major axis" );
    add( &FITSHdrExtension::get_bmin, "bmin", "[deg] beam minor axis" );
    add( &FITSHdrExtension::get_stt_date, "stt_date", "Start UT date (YYYY-MM-DD)" );
    add( &FITSHdrExtension::get_stt_time, "stt_time", "Start UT (hh:mm:ss)" );
    add( &FITSHdrExtension::get_stt_imjd, "stt_imjd", "Start MJD (UTC days) (J - long integer)" );
    add( &FITSHdrExtension::get_stt_smjd, "stt_smjd", "[s] Start time (sec past UTC 00h) (J)" );
    add( &FITSHdrExtension::get_stt_offs, "stt_offs", "[s] Start time offset (D)" );
    add( &FITSHdrExtension::get_stt_lst, "stt_lst", "Start LST (D)" );
    add( &FITSHdrExtension::get_stt_crd1, "stt_crd1", "Start coord 1 (hh:mm:ss.sss or ddd.ddd)" );
    add( &FITSHdrExtension::get_stt_crd2, "stt_crd2", "Start coord 2 (-dd:mm:ss.sss or -dd.ddd)" );
    add( &FITSHdrExtension::get_stp_crd1, "stp_crd1", "Stop coord 1 (hh:mm:ss.sss or ddd.ddd)" );
    add( &FITSHdrExtension::get_stp_crd2, "stp_crd2", "Stop coord 2 (-dd:mm:ss.sss or -dd.ddd)" );
    add( &FITSHdrExtension::get_ra, "ra", "Right ascension (hh:mm:ss.ssss)" );
    add( &FITSHdrExtension::get_dec, "dec", "Declination (-dd:mm:ss.sss)" );
  }

  TextInterface::Parser *FITSHdrExtensionTI::clone()
  {
    FITSHdrExtensionTI *cl = new FITSHdrExtensionTI();
    if( instance )
      cl->set_instance( instance );
    return cl;
  }
}


