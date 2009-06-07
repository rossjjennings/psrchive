/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "OwnStream.h"
#include <iostream>

//! Constructor
OwnStream::OwnStream ()
  : cout (std::cout.rdbuf()), cerr (std::cerr.rdbuf())
{
}

//! Constructor
OwnStream::OwnStream (const OwnStream&)
  : cout (std::cout.rdbuf()), cerr (std::cerr.rdbuf())
{
}

//! Set verbosity ostream
void OwnStream::set_cerr (std::ostream& os) const
{
  cerr.rdbuf( os.rdbuf() );
}
