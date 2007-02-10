/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/normal.h,v $
   $Revision: 1.1 $
   $Date: 2007/02/10 18:36:04 $
   $Author: straten $ */

#ifndef __genutil_normal_H
#define __genutil_normal_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Returns the standard normal probability function */
  double normal_probability (double x);

  /* Returns the standard normal cummulative distribution function */
  double normal_cummulative (double x);

  /* Returns the indefinite integral of the second moment of standard
     normal probability function */
  double normal_moment2 (double x);

#ifdef __cplusplus
           }
#endif

#endif

