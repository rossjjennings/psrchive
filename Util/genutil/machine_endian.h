/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/machine_endian.h,v $
   $Revision: 1.7 $
   $Date: 2004/08/31 06:42:58 $
   $Author: redwards $ */

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

#ifdef MACHINE_LITTLE_ENDIAN
#undef MACHINE_LITTLE_ENDIAN
#endif

/* is it Big or Little endian?? */
#ifdef __alpha
#define MACHINE_LITTLE_ENDIAN 1
#endif

#ifdef sgi
#define MACHINE_LITTLE_ENDIAN 0
#endif

#ifdef sun
#ifdef __i386
#define MACHINE_LITTLE_ENDIAN 1
#else
#define MACHINE_LITTLE_ENDIAN 0
#endif
#endif

#if defined(linux) || defined(__FreeBSD__)
#define MACHINE_LITTLE_ENDIAN 1
#endif

#if defined(__APPLE__) && defined(__POWERPC__)
#define MACHINE_LITTLE_ENDIAN 0
#endif

#ifndef MACHINE_LITTLE_ENDIAN
#error Machine architecture not recognized in machine_endian.h
#endif

/* endian-ness macros */
#if MACHINE_LITTLE_ENDIAN

#define toBigEndian(p,s) 	changeEndian(p,s)
#define toLittleEndian(p,s)
#define fromBigEndian(p,s)	changeEndian(p,s)
#define fromLittleEndian(p,s)

/* short forms that save typing */
#define ToBigEndian(p)          changeEndian (&(p), sizeof(p))
#define ToLittleEndian(p)
#define FromBigEndian(p)        changeEndian (&(p), sizeof(p))
#define FromLittleEndian(p)   

/* short forms that work on arrays */
#define N_ToBigEndian(N,p)      array_changeEndian (N, p, sizeof(*(p)))
#define N_ToLittleEndian(N,p)
#define N_FromBigEndian(N,p)    array_changeEndian (N, p, sizeof(*(p)))
#define N_FromLittleEndian(N,p)

#else

#define toBigEndian(p,s)
#define toLittleEndian(p,s)	changeEndian(p,s)
#define fromBigEndian(p,s)		
#define fromLittleEndian(p,s)	changeEndian(p,s)

/* short forms that save typing */
#define ToBigEndian(p)
#define ToLittleEndian(p)       changeEndian (&(p), sizeof(p))
#define FromBigEndian(p)
#define FromLittleEndian(p)     changeEndian (&(p), sizeof(p))

/* short forms that work on arrays */
#define N_ToBigEndian(N,p) 
#define N_ToLittleEndian(N,p)   array_changeEndian (N, p, sizeof(*p))
#define N_FromBigEndian(N,p) 
#define N_FromLittleEndian(N,p) array_changeEndian (N, p, sizeof(*p))

#endif

#endif  /* !def __M_ENDIAN_H */
