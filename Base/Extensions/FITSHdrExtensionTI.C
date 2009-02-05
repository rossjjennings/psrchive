/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#include <Pulsar/FITSHdrExtension.h>



using Pulsar::FITSHdrExtension;



FITSHdrExtension::Interface::Interface( FITSHdrExtension *s_instance )
{
  if( s_instance )
    set_instance( s_instance );

  add( &FITSHdrExtension::get_obs_mode,
       &FITSHdrExtension::set_obs_mode,
       "obs_mode", "Observation Mode (PSR, CAL, SEARCH)" );

  add( &FITSHdrExtension::get_obsfreq,
       &FITSHdrExtension::set_obsfreq,
       "obsfreq", "[MHz] Centre frequency for observation" );

  add( &FITSHdrExtension::get_hdrver,
       &FITSHdrExtension::set_hdrver,
       "hdrver", "Header Version" );

  add( &FITSHdrExtension::get_creation_date,
       &FITSHdrExtension::set_creation_date,
       "date", "File Creation Date" );

  add( &FITSHdrExtension::get_coordmode,
       &FITSHdrExtension::set_coordmode,
       "coord_md", "The coordinate mode (EQUAT, GAL, ECLIP, etc )." );

  add( &FITSHdrExtension::get_equinox,
       &FITSHdrExtension::set_equinox,
       "equinox", "Equinox of coords (J2000, B1950)" );

  add( &FITSHdrExtension::get_trk_mode,
       &FITSHdrExtension::set_trk_mode,
       "trk_mode", "Track mode ( TRACK, SCANGC, SCANLAT )" );

  add( &FITSHdrExtension::get_bpa,
       &FITSHdrExtension::set_bpa,
       "bpa", "[deg] beam position angle" );

  add( &FITSHdrExtension::get_bmaj,
       &FITSHdrExtension::set_bmaj,
       "bmaj", "[deg] beam major axis" );

  add( &FITSHdrExtension::get_bmin,
       &FITSHdrExtension::set_bmin,
       "bmin", "[deg] beam minor axis" );

  add( &FITSHdrExtension::get_stt_date,
       &FITSHdrExtension::set_stt_date,
       "stt_date", "Start UT date (YYYY-MM-DD)" );

  add( &FITSHdrExtension::get_stt_time,
       &FITSHdrExtension::set_stt_time,
       "stt_time", "Start UT (hh:mm:ss)" );

  add( &FITSHdrExtension::get_stt_imjd,
       &FITSHdrExtension::set_stt_imjd,
       "stt_imjd", "Start MJD (UTC days) (J - long integer)" );

  add( &FITSHdrExtension::get_stt_smjd,
       &FITSHdrExtension::set_stt_smjd,
       "stt_smjd", "[s] Start time (sec past UTC 00h) (J)" );

  add( &FITSHdrExtension::get_stt_offs,
       &FITSHdrExtension::set_stt_offs,
       "stt_offs", "[s] Start time offset (D)" );

  add( &FITSHdrExtension::get_stt_lst,
       &FITSHdrExtension::set_stt_lst,
       "stt_lst", "Start LST (D)" );

  add( &FITSHdrExtension::get_stt_crd1,
       &FITSHdrExtension::set_stt_crd1,
       "stt_crd1", "Start coord 1 (hh:mm:ss.sss or ddd.ddd)" );

  add( &FITSHdrExtension::get_stt_crd2,
       &FITSHdrExtension::set_stt_crd2,
       "stt_crd2", "Start coord 2 (-dd:mm:ss.sss or -dd.ddd)" );

  add( &FITSHdrExtension::get_stp_crd1,
       &FITSHdrExtension::set_stp_crd1,
       "stp_crd1", "Stop coord 1 (hh:mm:ss.sss or ddd.ddd)" );

  add( &FITSHdrExtension::get_stp_crd2,
       &FITSHdrExtension::set_stp_crd2,
       "stp_crd2", "Stop coord 2 (-dd:mm:ss.sss or -dd.ddd)" );

  add( &FITSHdrExtension::get_ra,
       &FITSHdrExtension::set_ra,
       "ra", "Right ascension (hh:mm:ss.ssss)" );

  add( &FITSHdrExtension::get_dec,
       &FITSHdrExtension::set_dec,
       "dec", "Declination (-dd:mm:ss.sss)" );
}


