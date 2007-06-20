//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FITSSUBHdrExtension.h,v $
   $Revision: 1.1 $
   $Date: 2007/06/20 03:04:53 $
   $Author: nopeer $ */

#ifndef __Pulsar_FITSSUBHdrExtension_h
#define __Pulsar_FITSSUBHdrExtension_h

#include "Pulsar/Archive.h"
#include <TextInterface.h>



namespace Pulsar
{
  class FITSSUBHdrExtension : public Archive::Extension
  {
  public:
    //Default constructor
    FITSSUBHdrExtension ();

    // Copy constructor
    FITSSUBHdrExtension (const Pulsar::FITSSUBHdrExtension& extension);

    // Operator =
    const FITSSUBHdrExtension& operator= (const FITSSUBHdrExtension& extension);

    // Destructor
    ~FITSSUBHdrExtension ();

    //! Clone method
    FITSSUBHdrExtension* clone () const { return new FITSSUBHdrExtension( *this ); }

    //! Get the text interface
    Reference::To< TextInterface::Class > get_text_interface();

    std::string get_int_type( void ) const { return int_type; }
    std::string get_int_unit( void ) const { return int_unit; }
    double get_tsamp( void ) const { return tsamp; }
    int get_nbin( void ) const { return nbin; }
    int get_nbits( void ) const { return nbits; }
    int get_nch_file( void ) const { return nch_file; }
    int get_nch_strt( void ) const { return nch_strt; }
    int get_npol( void ) const { return npol; }
    int get_nsblk( void ) const { return nsblk; }

    void set_int_type( std::string s_int_type )  { int_type = s_int_type; }
    void set_int_unit( std::string s_int_unit )  { int_unit = s_int_unit; }
    void set_tsamp( double s_tsamp )  { tsamp = s_tsamp; }
    void set_nbin( int s_nbin )  { nbin = s_nbin; }
    void set_nbits( int s_nbits )  { nbits = s_nbits; }
    void set_nch_file( int s_nch_file )  { nch_file = s_nch_file; }
    void set_nch_strt( int s_nch_strt )  { nch_strt = s_nch_strt; }
    void set_npol( int s_npol )  { npol = s_npol; }
    void set_nsblk( int s_nsblk )  { nsblk = s_nsblk; }

  private:
    std::string int_type;
    std::string int_unit;
    double tsamp;
    int nbin;
    int nbits;
    int nch_file;
    int nch_strt;
    int npol;
    int nsblk;
  };
}

#endif
