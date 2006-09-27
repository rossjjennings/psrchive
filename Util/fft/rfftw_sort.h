/***************************************************************************
 *
 *   Copyright (C) 1998 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef PSR_RFFTW_SORT_H
#define PSR_RFFTW_SORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Sort into DC, 0, r[1], i[1], ... r[npts/2-1], i[npts/2-1], Nyq, 0 */
int rfftw_sort(int npts, float *src, float *dest);

#ifdef __cplusplus
}
#endif

#endif
