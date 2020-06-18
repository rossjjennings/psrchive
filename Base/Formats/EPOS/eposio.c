/***************************************************************************
 *
 *   Copyright (C) 2018 by Ramesh Karuppusamy
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>

#include "eposio.h"

int readepos ( const char *filename, eposhdr_t *eposhdr, eposdata_t *eposdata)
{
  int readwri = 0;

  /* for dealing with filename string */
  unsigned length = 0;

  if (!filename) return -1;

  length = strlen (filename);

  if (readwri > 0)  return 0;

  return 0;

}

int get_nsubints(const char* filename)
{
  // very simple method to get number of subints - mod(17000) of file size
  struct stat st;
  
  stat (filename, &st);

  return st.st_size/17000; 
 
}

int get_master_clock(eposhdr_t *eposhdr)
{
  return eposhdr->masterref;
}

/* check  if the file passed is a valid EPOS file */
/* 1. EPOS master clock is always 30 MHz*/
/* 2. The file is always a multiple of 17000 bytes */

int check_file(const char* filename, eposhdr_t *eposhdr, int verbose)
{
  struct stat st;
  int file_flag = 0;
  FILE *fp;
  int fd;
  int nbytes;
  int c = 0;
 
  fp = fopen(filename, "r");
  fd = fileno(fp);
  fstat(fd, &st);

  if ( (st.st_size % 17000) != 0 )
  {
        fclose(fp);
	if (verbose > 2)
	  fprintf(stderr, "EPOS check_file: File is not a multiple of 17000 bytes.");
        return -1;
  }

  nbytes = fread(eposhdr, 1, sizeof(eposhdr_t), fp);
  fclose(fp);

  if ( nbytes != sizeof(eposhdr_t) ){
        if (verbose > 2)
	  fprintf(stderr, "EPOS check_file: Unable to read header");
	return -1;
  }
  
  int masterref = get_master_clock(eposhdr);

  if ( (masterref != 30000000) && file_flag ) {
        if (verbose > 2)
	  fprintf(stderr, "EPOS check_file: Master clock is not 30 MHz");
	return -1;
  }

#define MAX_SOURCE_STRLEN 24

  c = 0;
  while (c < MAX_SOURCE_STRLEN)
  {
     if (eposhdr->source[c] == '\0')
       break;

     if (!isprint(eposhdr->source[c]))
     {
        if (verbose > 2)
          fprintf(stderr, "EPOS check_file: Source name contains non-printable characters");
        return -1;
     }
  }

  if (c == MAX_SOURCE_STRLEN)
  {
    if (verbose > 2)
      fprintf(stderr, "EPOS check_file: Source name exceeds buffer length");
    return -1;
  }

  if (c == 0)
  {
    if (verbose > 2)
      fprintf(stderr, "EPOS check_file: Source name not set");
    return -1;
  }

  return 1;
}

int get_hdr(const char* filename, eposhdr_t *eposhdr, unsigned isub, int verbose)
{
  FILE *fp;
  int current_offset;  
  int nbytes;

  fp = fopen(filename, "r");
  
  long offset = isub * 17000 ;

  if ( (fseek(fp, offset, SEEK_SET)) == -1 ) {
	fprintf(stderr,"failed to fseek in file %s\n",filename);
	return -1;
  }

  nbytes = fread(eposhdr, 1, sizeof(eposhdr_t), fp);

  if (nbytes < 1) {
	fprintf(stderr,"fread returned %d bytes\n",nbytes);
	return -1;
  }

  fclose(fp);
  
  return 1;

}

int get_data(const char* filename, eposdata_t *eposdata, unsigned isub, int verbose)
{
  FILE *fp;
  int current_offset;
  int nbytes, i;
  // The file consists to two channels, correpsonding to the two polarisations
  int ch0[1024];
  int ch1[1024];

  fp = fopen(filename, "r");

  if ( fp == NULL) {
	fprintf(stderr,"cannot open file %s\n", filename);
	return -1;
  }
  // data starts at 278th byte in each block

  long offset = 278 + isub * 17000 ;

  current_offset = fseek(fp, offset, SEEK_SET);

  if ( verbose > 2 ) fprintf(stderr,"%s: offset is %d bytes\n", __func__, current_offset);

  nbytes = fread(ch0, 1, sizeof(ch0), fp);
  nbytes = fread(ch1, 1, sizeof(ch1), fp);

  if (nbytes < 1) {
    fprintf(stderr,"fread returned %d bytes\n",nbytes);
    return -1;
  }

  // swap bytes - strange data format: swap the 2-byte parts sperately to
  // construct the 4-byte int
  
  for ( i=0; i< 1024; i++){
	eposdata->data0[i] = ((ch0[i] & 0x000000ff) << 16) | ((ch0[i] & 0x0000ff00) << 16) |\
	  ((ch0[i] & 0x00ff0000) >> 16) | ((ch0[i] & 0xff000000) >> 16) ;

	eposdata->data1[i] = ((ch1[i] & 0x000000ff) << 16) | ((ch1[i] & 0x0000ff00) << 16) |\
	  ((ch1[i] & 0x00ff0000) >> 16) | ((ch1[i] & 0xff000000) >> 16) ;
  }
	  

  fclose(fp);

  return 1;

}

