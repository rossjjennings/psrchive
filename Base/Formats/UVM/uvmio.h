/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/*
 *
 * uvmio.h - defines a C interface to uvmlib routines
 *
 * The structs defined in this header file must be a binary match to
 * the common blocks defined in libuvm.  This includes:
 *
 * #define UVM_MAXBIN mmm // where: parameter (apbinsmax=2048)
 *
 * Willem van Straten - April 2013
 *
 */

#ifndef __UVMIO_H
#define __UVMIO_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Header block: common/header/ */
  typedef struct
  {
    double period;
    double samint;
    double scantime;
    int date;
    int startime;
    int arecnum;
    int nscan;
    int aendscan;
    float abandwd;
    int aposcrd;
    float apos1st;
    float apos2st;
    float aazst;
    float za;
    float azaend;
    float aclkrat;
    int admptim;
    int admplen;
    int admpsrd;
    int ainttim;
    int aextst;
    float obfreq;
    int apoladd;
    int anumsbc;
    int alensbc;
    int apbins;
    float adm;
    float arm;
    float abintot;
    int admptot;
    float abtwscn;
    float scaleI;
    float sigmaI;
    float abbm3lo;
    float abbm4lo;
    float asynfreq;
    int anrecscn;
    float rm;
    float erm;
    char aver[3];
    char source[9];
    int controlword;
    int nwins;
    int nwbins[5];
    char wndows[5];
    char sver[3];
    int nseq;
    char obsvtry[11];

  } uvm_header;

  /* Maximum number of phase bins: parameter (apbinsmax=2048)) */
  #define UVM_MAXBIN 2048
  #define UVM_NPOL 4
  /* Data block: common/iquv/ */
  typedef struct
  {
    float data [UVM_NPOL][UVM_MAXBIN];
  } uvm_data;


  /* limitation built into getheader */
# define UVM_MAX_FILENAME 80


  /* C wrapper of the Fortran getheader function */
  int uvm_getheader ( const char* filename,
    int* program,  /* return value encodes file format */
    int* newscan,  /* not yet sure what this does */
    uvm_header*);

  int uvm_getdata ( int program, uvm_data* );

  void uvm_dump (const uvm_header*);

#ifdef __cplusplus
}
#endif

#endif
