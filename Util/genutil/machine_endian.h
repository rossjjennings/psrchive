#ifndef ENDIAN_H
#define ENDIAN_H

/* the main byte-swapping function */
#ifdef __cplusplus
extern "C" {
#endif
extern void changeEndian(void *, int);
#ifdef __cplusplus
}
#endif

/* is it Big or Little endian?? */
#ifdef __alpha
#ifndef MACHINE_LITTLE_ENDIAN
#define MACHINE_LITTLE_ENDIAN
#endif
#endif

#ifdef sgi
#undef MACHINE_LITTLE_ENDIAN
#endif

#ifdef sun
#define MACHINE_BIG_ENDIAN
#endif

#ifdef __linux
#define MACHINE_LITTLE_ENDIAN
#endif

/* endian-ness macros */
#ifdef MACHINE_LITTLE_ENDIAN
#define toBigEndian(p,s) 	changeEndian(p,s)
#define toLittleEndian(p,s)	
#define fromBigEndian(p,s)	changeEndian(p,s)
#define fromLittleEndian(p,s)	
#else
#define toBigEndian(p,s)
#define toLittleEndian(p,s)	changeEndian(p,s)
#define fromBigEndian(p,s)		
#define fromLittleEndian(p,s)	changeEndian(p,s)
#endif

#endif
