/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __F77UTIL_H
#define __F77UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

void f2cstr (const char* f_str, char* c_str, unsigned length);
void c2fstr (char* f_str, const char* c_str, unsigned length);

#ifdef __cplusplus
	   }
#endif

#endif

