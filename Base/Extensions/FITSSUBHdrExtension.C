/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/FITSSUBHdrExtension.h"
#include "Pulsar/FITSSUBHdrExtensionTI.h"



using namespace Pulsar;






//Default constructor
FITSSUBHdrExtension::FITSSUBHdrExtension ()
    : Extension( "FITSSUBHdrExtension" )
{
  int_type = "";
  int_unit = "";
  tsamp = -1;
  nbin = -1;
  nbits = -1;
  nch_file = -1;
  nch_strt = -1;
  npol = -1;
  nsblk = -1;
}

// Copy constructor
FITSSUBHdrExtension::FITSSUBHdrExtension (const Pulsar::FITSSUBHdrExtension& extension)
    : Extension( "FITSSUBHdrExtension" )
{
  operator = (extension);
}

// Operator =
const FITSSUBHdrExtension& FITSSUBHdrExtension::operator= (const FITSSUBHdrExtension& extension)
{
  int_type = extension.int_type;
  int_unit = extension.int_unit;
  tsamp = extension.tsamp;
  nbin = extension.nbin;
  nbits = extension.nbits;
  nch_file = extension.nch_file;
  nch_strt = extension.nch_strt;
  npol = extension.npol;
  nsblk = extension.nsblk;

  return *this;
}

// Destructor
FITSSUBHdrExtension::~FITSSUBHdrExtension ()
{}


TextInterface::Parser* FITSSUBHdrExtension::get_interface()
{
  return new FITSSUBHdrExtensionTI( this );
}



