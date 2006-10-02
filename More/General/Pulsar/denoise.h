/***************************************************************************
 *
 *   Copyright (C) 2004 by Stephen Ord
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_denoise_h
#define __Pulsar_denoise_h

namespace Pulsar {

  class Archive;
  class Profile;

  //! smooth the profile by whacking spectral components
  /* \param fraction one over the fraction of spectrum to keep (8 == 1/8th) */
  void denoise (Profile* profile, int fraction = 8);

  //! applies denoise to every profile in the archive
  void denoise (Archive* archive, int fraction = 8);

}

#endif
