/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/genutil/machine_endian.h

#ifndef __M_ENDIAN_H
#define __M_ENDIAN_H

/* the main byte-swapping function */
#ifdef __cplusplus
extern "C" {
#endif

void changeEndian (void *p, int size);
void array_changeEndian (int count, void *p, int element_size);

#ifdef __cplusplus
}
#endif

/* MACRO to save you some typing */
#define ChangeEndian(p) changeEndian (&(p), sizeof(p))

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WORDS_BIGENDIAN

#define MACHINE_LITTLE_ENDIAN 0

#define toBigEndian(p,s)           ((void)0)
#define toLittleEndian(p,s)        changeEndian(p,s)
#define fromBigEndian(p,s)         ((void)0)
#define fromLittleEndian(p,s)      changeEndian(p,s)

/* short forms that save typing */
#define ToBigEndian(p)             ((void)0)
#define ToLittleEndian(p)          changeEndian (&(p), sizeof(p))
#define FromBigEndian(p)           ((void)0)
#define FromLittleEndian(p)        changeEndian (&(p), sizeof(p))

/* short forms that work on arrays */
#define N_ToBigEndian(N,p)         ((void)0)
#define N_ToLittleEndian(N,p)      array_changeEndian (N, p, sizeof(*p))
#define N_FromBigEndian(N,p)       ((void)0)
#define N_FromLittleEndian(N,p)    array_changeEndian (N, p, sizeof(*p))

#else

#define MACHINE_LITTLE_ENDIAN 1

#define toBigEndian(p,s)           changeEndian(p,s)
#define toLittleEndian(p,s)        ((void)0)
#define fromBigEndian(p,s)         changeEndian(p,s)
#define fromLittleEndian(p,s)      ((void)0)

/* short forms that save typing */
#define ToBigEndian(p)             changeEndian (&(p), sizeof(p))
#define ToLittleEndian(p)          ((void)0)
#define FromBigEndian(p)           changeEndian (&(p), sizeof(p))
#define FromLittleEndian(p)        ((void)0)

/* short forms that work on arrays */
#define N_ToBigEndian(N,p)         array_changeEndian (N, p, sizeof(*(p)))
#define N_ToLittleEndian(N,p)      ((void)0)
#define N_FromBigEndian(N,p)       array_changeEndian (N, p, sizeof(*(p)))
#define N_FromLittleEndian(N,p)    ((void)0)

#endif

#endif  /* !def __M_ENDIAN_H */
