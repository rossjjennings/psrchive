/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "uvmio.h"
#include "f77util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* *************************************************************************
 *
 * declaration of functions defined in getheader.f and getdata.f
 *
 * ************************************************************************* */

#define F77_getheader F77_FUNC(getheader,GETHEADER)
void F77_getheader (char filename[UVM_MAX_FILENAME],
		    int* prgm, int* newscan, int* convert, int* istat);

#define F77_getdata F77_FUNC(getdata,GETDATA)
void F77_getdata (int* prgm, int* convert, int* istat);


/* *************************************************************************
 *
 * declaration of common blocks defined in uvmlib
 *
 * ************************************************************************* */

#define F77_header F77_FUNC(header,HEADER)
extern uvm_header F77_header;

#define F77_data F77_FUNC(iquv,IQUV)
extern uvm_data F77_data;



/* *************************************************************************
 *
 * the C wrapper to getheader
 *
 * ************************************************************************* */
  
int uvm_getheader ( const char* filename,
		    int* program,
		    int* newscan,
		    uvm_header* hdr )
{
  // Truncated filename will result in crash, so just avoid it
  if (strlen(filename) > UVM_MAX_FILENAME) 
    return -1;

  char f77_filename[UVM_MAX_FILENAME];
  c2fstr (f77_filename, filename, UVM_MAX_FILENAME);

  int convert = 0;  /* don't compute L and P.A. */
  int istat = 0;
  F77_getheader (f77_filename, program, newscan, &convert, &istat);

  if (istat != 0)
    return -1;

  *hdr = F77_header;

  return 0;
}


int uvm_getdata ( int program, uvm_header* hdr, uvm_data* data )
{

  int convert = 0;  /* don't compute L and P.A. */
  int istat = 0;
  F77_getdata ( &program, &convert, &istat);

  if (istat != 0)
    return -1;

  *hdr = F77_header;
  *data = F77_data;
  
  return 0;
}

#if 0

int crwuvm ( const char* filename, int readwri, int recno, int padout,
             uvm_header_line1* line1,
             uvm_header_line2* line2,
             uvm_header_line3* line3,
             uvm_header_line4* line4,
             uvm_header_line5* line5,
             uvm_block_subheader_line1* sub_line1,
             uvm_block_subheader_line2* sub_line2,
             uvm_data_block* data )
{
  /* for dealing with filename string */
  unsigned length = 0;
  char* f77_filename = 0;

  /* for counting blocks */
  unsigned iblock = 0, nblock = 0;

  /* for transposing data */
  unsigned ibin = 0, nbin = 0;
  float* ptr = 0;

  if (!filename)
    return -1;

  /* space-terminate the filename */
  length = strlen (filename);
  f77_filename = strdup (filename);
  f77_filename[length] = ' ';

  if (readwri > 0) {

    F772C(uvm1) = *line1;
    F772C(uvm2) = *line2;
    F772C(uvm3) = *line3;
    F772C(uvm4) = *line4;
    F772C(uvm5) = *line5;
    F772C(uvms1) = *sub_line1;
    F772C(uvms2) = *sub_line2;

    /* fix up the strings */

    /* don't fix the version string, as it uses all eight characters
     * c2fstr (F772C(uvm1).version, line1->version, 8); */

    c2fstr (F772C(uvm1).history, line1->history, 68);

    c2fstr (F772C(uvm2).jname, line2->jname, 12);
    c2fstr (F772C(uvm2).cname, line2->cname, 12);
    c2fstr (F772C(uvm2).catref, line2->catref, 6);
    c2fstr (F772C(uvm2).bibref, line2->bibref, 8);

    c2fstr (F772C(uvm3).telname, line3->telname, 8);

    nblock = line5->npol * line5->nfreq;
    for (iblock = 0; iblock < nblock; iblock++) {
      c2fstr (F772C(uvms1).idfield[iblock], sub_line1->idfield[iblock], 8);
      c2fstr (F772C(uvms1).f0u[iblock], sub_line1->f0u[iblock], 8);
      c2fstr (F772C(uvms1).dfu[iblock], sub_line1->dfu[iblock], 8);
    }

    /* transpose the data */
    ptr = &(F772C(dblk).rawdata[0][0]);
    nbin = line5->nbin;

    fprintf (stderr, "w nblock=%d nbin=%d\n", nblock, nbin);

    for (ibin = 0; ibin < nbin; ibin++) {
      for (iblock = 0; iblock < nblock; iblock++) {
	*ptr = data->rawdata[iblock][ibin];
	ptr ++;
      }
      for (; iblock < UVM_MAXBLK; iblock++) {
	*ptr = 0.0;
	ptr ++;
      }
    }

  }

  F772C(rwuvm) (f77_filename, &readwri, &recno, &padout, length);

  free (f77_filename);

  if (recno == -999)
    return -1;

  if (readwri > 0)
    return 0;

  *line1 = F772C(uvm1);
  *line2 = F772C(uvm2);
  *line3 = F772C(uvm3);
  *line4 = F772C(uvm4);
  *line5 = F772C(uvm5);
  *sub_line1 = F772C(uvms1);
  *sub_line2 = F772C(uvms2);

  /* fix up the strings */

  /* don't fix the version string, as it uses all eight characters
   * f2cstr (F772C(uvm1).version, line1->version, 8); */

  f2cstr (F772C(uvm1).history, line1->history, 68);
  
  f2cstr (F772C(uvm2).jname, line2->jname, 12);
  f2cstr (F772C(uvm2).cname, line2->cname, 12);
  f2cstr (F772C(uvm2).catref, line2->catref, 6);
  f2cstr (F772C(uvm2).bibref, line2->bibref, 8);
  
  f2cstr (F772C(uvm3).telname, line3->telname, 8);
  
  nblock = line5->npol * line5->nfreq;
  for (iblock = 0; iblock < nblock; iblock++) {
    f2cstr (F772C(uvms1).idfield[iblock], sub_line1->idfield[iblock], 8);
    f2cstr (F772C(uvms1).f0u[iblock], sub_line1->f0u[iblock], 8);
    f2cstr (F772C(uvms1).dfu[iblock], sub_line1->dfu[iblock], 8);
  }

  /* transpose the data */
  ptr = &(F772C(dblk).rawdata[0][0]);
  nbin = line5->nbin;

  /* fprintf (stderr, "r nblock=%d nbin=%d\n", nblock, nbin); */

  for (ibin = 0; ibin < nbin; ibin++) {
    for (iblock = 0; iblock < nblock; iblock++) {
      /* fprintf (stderr, "%d %d %f\n", iblock, ibin, *ptr); */
      data->rawdata[iblock][ibin] = *ptr;
      ptr ++;
    }
    ptr += UVM_MAXBLK - nblock;
  }

  /* fprintf (stderr, "crwuvm return\n"); */

  return 0;
}

void uvm_dump (const uvm_header_line1* line1,
               const uvm_header_line2* line2,
               const uvm_header_line3* line3,
               const uvm_header_line4* line4,
               const uvm_header_line5* line5)
{
  fprintf (stderr, "line 5: npol=%d\n", line5->npol);
  fprintf (stderr, "line 5: nfreq=%d\n", line5->nfreq);
  fprintf (stderr, "line 5: nbin=%d\n", line5->nbin);
  fprintf (stderr, "line 5: tbin=%lf\n", line5->tbin);
  fprintf (stderr, "line 5: nint=%d\n", line5->nint);
  fprintf (stderr, "line 5: ncal=%d\n", line5->ncal);
}

int cnuvmrec (const char* filename)
{
  /* for dealing with filename string */
  unsigned i, length = 0;
  char f77_filename [UVM_MAX_FILENAME];

  if (!filename)
    return -1;

  /* space-terminate the filename */
  length = strlen (filename);
  strcpy (f77_filename, filename);
  for (i=length; i<UVM_MAX_FILENAME; i++)
    f77_filename[i] = ' ';

  return F772C(nuvmrec) (f77_filename, length);
}

#endif
