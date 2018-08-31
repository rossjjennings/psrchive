
/***************************************************************************
 *
 *   Copyright (C) 2004 by Ramesh Karuppusamy
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/*
 *
 * eposio.h - defines a C interface to read EPOS files
 *
 * The structs defined in this header file must be a binary match the EPOS data structure
 * The EPOS files are always multiples of 17000 bytes. 
 *
 * Ramesh Karuppusamy - June 2012
 *
 */

#ifndef __EPOS_IO_H
#define __EPOS_IO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  short int isn; /* scan number */
  short int issn; /* sub-scan number*/
  short int mbltp; /* block number in pulsar data file*/  
  short int mblsc; /* block number in the scan*/
  short int iyr;  /* year of observation */
  short int imon; /* month */
  short int iday; /* day */
  short int ihr; /* hour*/
  short int imin;  /* minute */
  short int isec; /* second */
  short int modtel;  /* telescope mode (idle, observing, stop) */
  int ialpha; /* RA in milliarcs */
  int idelta; /* DEC in milliarcs */ 
  short int dummy0; /* unused*/
  short int J11status; /* J11 status*/
  short int dummy1; /* unused*/
  short int J11istatus; /* J11 internal status*/
  int J11ictr; /* J11 idle counter*/
  short int dummy2[10]; /* unused*/
  int masterref; /* Maser reference clock (Hz)*/
  int ireqper1; /* requested barycentric period*/
  int ireqper2; /* period=ireqper1*ireqper2/maserref*/
  int ireqvel; /* requested doppler velocity correction*/
  int ireqdel; /* last used delay*/
  short int npul; /* number of pulses integrated by PUB (internal)*/
  short int isamp0; /* requested sampling time in microseconds*/
  short int dummy3[4]; /* unused */
  short int J11clkday; /* J11 internal clock - day */
  short int J11clkhr; /* J11 internal clock - hour */
  short int J11clkmin; /* J11 internal clock - minute */
  short int J11clksec; /* J11 internal clock - second */
  short int intclkday; /* internal clock - day */
  short int intclkhr; /*  internal clock - hour */
  short int intclkmin; /* internal clock - minute */
  short int intclksec; /* internal clock - second */
  short int dummy4[4]; /* unused */
  int ical; /* cal duration in microseconds */
  short int dummy5[2]; /* unused */
  unsigned char ver[8];/* program version */
  unsigned char syncstat[8];/* synchroisation status */
  int iactper1; /* actual period*/
  int iactper2; /* current period=iactper1*iactper2/maserref*/
  int iactvel; /* current doppler velocity*/
  int itotdel; /* total accumulated delay*/
  short int isyncday; /*day of last sync - pulse phase is referred to this time */
  short int isynchr; /*hour of last sync (MET)*/
  short int isyncmin; /*minute of last sync */
  short int isyncsec; /*second of last sync (multiple of 10!)*/
  short int intday; /*day of intergration start*/
  short int inthr; /*hour*/
  short int intmin; /*min */
  short int intsec; /*second */
  unsigned char source[24]; /* source name*/
  int baryper_us; /* timapr - barycentric period in microseconds */
  int baryper_ps; /* timapr- barycentric period in picoseconds */
  int nc1; /* counter correction stage 1 */
  int nc2; /* counter correction stage 2 */
  int nc3; /* periods until 0.5 microsecond drift */
  short int dummy6; /*unused */
  int freq_hz; /* this plus next two items - valid for observations made after 01 Nov 2002*/
  int freq_ghz; 
  int feedpos; /* feed rotation in telescope units. rot(deg) = (feedpos - 555)* 0.036*/
  short int dummy7[10];
  int isyncusec; /* microseconds since last sync*/
  short int numsamm; /* content counter*/
  short int nint; /* number of integrations */
  short int isamp; /* sampling time - divide this by 5, as this runs off a 150MHz clock*/
  short int sampsrc; /* sampling source - 0:internal, 1:external */
  short int timeref; /* time reference 0:pulsar 1:sampling period*/
  short int inp[4]; /* no description */

} eposhdr_t;

typedef struct{
  int data0[1024];
  int data1[1024];
  int data2[1024];
  int data3[1024];
  short int checksum;
  short int dummy0[6]; /* no description */
  unsigned char jhspuls[32]; /* used by JHSPULS*/
  unsigned char dummy1[288]; /*empty */
}eposdata_t;

  int check_file(const char *filename, eposhdr_t *eposhdr, int verbose);
  int get_nsubints(const char *filename);
  int get_hdr(const char *filename, eposhdr_t *eposhdr, unsigned subint, int verbose);
  int get_data(const char *filename, eposdata_t *eposdata, unsigned subint, int verbose);

#ifdef __cplusplus
}
#endif

#endif
