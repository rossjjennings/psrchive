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

#include "f77util.h"

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
    float baseval; // asynfreq;
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

  /* return the null-terminated string for char aver[3] */
  inline void uvm_get_version (const uvm_header* hdr, char* txt)
  { f2cstr(hdr->aver, txt, 3); }

  /* return the null-terminated string for char source[9] */
  inline void uvm_get_source (const uvm_header* hdr, char* txt)
  { f2cstr(hdr->source, txt, 9); }

  /* return the null-terminated string for char wndows[5] */
  inline void uvm_get_windows (const uvm_header* hdr, char* txt)
  { f2cstr(hdr->wndows, txt, 5); }

  /* return the null-terminated string for char sver[3] */
  inline void uvm_get_software_version (const uvm_header* hdr, char* txt)
  { f2cstr(hdr->sver, txt, 3); }

  /* return the null-terminated string for char obsvtry[11] */
  inline void uvm_get_observatory (const uvm_header* hdr, char* txt)
  { f2cstr(hdr->obsvtry, txt, 11); }


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

  int uvm_getdata ( int program, uvm_header*, uvm_data* );

  void uvm_dump (const uvm_header*);

#ifdef __cplusplus
}
#endif

#endif
