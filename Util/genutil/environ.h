/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/environ.h,v $
   $Revision: 1.8 $
   $Date: 2004/03/05 19:42:12 $
   $Author: straten $ */

/*
 * When sending packets back and forth between different architectures,
 * one hopes that they all agree on packet size and the size of each field
 * within the packet.  Using the types typedef'd in this header should help
 * keep things straight.
 * If you are worried about endian, see endian.h.
 *
 * Programmed by Willem van Straten using ideas hacked from YorkU CS dept.
 * and Russell Edwards' tips on environment -D defines
 */

#ifndef __ENVIRON_H
#define __ENVIRON_H

/* this will hopefully cover all the different ways that compilers like
   to define unix */
#ifdef __unix__
#ifndef unix
#define unix
#endif
#endif

#ifdef __unix
#ifndef unix
#define unix
#endif
#endif

/* ********************************************** UNIX */
#ifdef unix 
typedef short          int16;
typedef unsigned short uint16;

typedef float          float32;
typedef double         float64;

/* ********************************************** on DEC alphas */
#ifdef __alpha
typedef int            int32;
typedef unsigned int   uint32;

typedef long           int64;
typedef unsigned long  uint64;

typedef struct {double f1; double f2;} float128;

#define ORDER_LITTLE_ENDIAN 1
#define ORDER_BIG_ENDIAN    0

/* for C++ compiler */
#define NAMESPACE_WORKS 1

/* for parsing a long */
#define I64 "%ld"
#define UI64 "%lu"
#define I32 "%d"

#endif

/* ********************************************** on Silcon Graphics */
#ifdef sgi
typedef int            int32;
typedef unsigned int   uint32;

typedef long           int64;
typedef unsigned long  uint64;

typedef struct {double f1; double f2;} float128;
#define ORDER_LITTLE_ENDIAN 1
#endif

/* ********************************************** on Sun Microsystems */
#ifdef sun
typedef long           int32;
typedef unsigned long  uint32;

typedef long long      int64;
typedef unsigned long long uint64;

typedef long double    float128;
#define ORDER_BIG_ENDIAN 1

/* for parsing a long */
#define I64 "%lld"
#define UI64 "%llu"
#define I32 "%ld"

#endif

#endif  /* ****** end UNIX */

/* ********************************************** LINUX */
#ifdef linux
typedef long           int32;
typedef unsigned long  uint32;

typedef long long int64;
typedef unsigned long long uint64;
typedef struct {double f1; double f2;} float128;

/* for parsing a long */
#define I64 "%Ld"
#define UI64 "%Lu"
#define I32 "%ld"

#define ORDER_LITTLE_ENDIAN 1
#define ORDER_BIG_ENDIAN    0

#endif

/* By default, struct elements should be aligned on double memory addresses */
#define MALIGN_DOUBLE 1

#if defined(linux) || (defined (sun) && defined(__i386))
#undef MALIGN_DOUBLE
#endif

/* For future reference */

#ifdef WINDOWS
typedef short int16;
typedef long  int32;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef float real;

#define REORDER_BYTES
#endif

#ifdef MSDOS
typedef short int16;
typedef long  int32;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef float real;

/*
 * replace malloc with farmalloc((unsigned int))
 * to get around 64k limit
 *
 */
#include <stdlib.h>
#include <malloc.h> /* for Borland C++ 4.0 */

#if 0
#define malloc(x) farmalloc((unsigned long)(x))
#define calloc(x,y) farcalloc((unsigned long)(x),(y))
#define free(x) farfree(x)
#endif /* 0 */

void far *dbg_malloc( unsigned long );
void far *dbg_calloc( unsigned long, unsigned long );
void far *dbg_realloc( void far *, unsigned long );
void dbg_free( void far * );

#define malloc(x) dbg_malloc(x)
#define calloc(x,y) dbg_calloc(x,y)
#define free(x) dbg_free(x)

#ifndef REORDER_BYTES
#define REORDER_BYTES
#endif /* REORDER_BYTES */
#endif /* MSDOS */

#endif /* ENVIRON_H */

