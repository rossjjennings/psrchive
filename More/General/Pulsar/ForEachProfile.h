//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ForEachProfile.h

#ifndef __Pulsar_ForEachProfile_h
#define __Pulsar_ForEachProfile_h

#include "Pulsar/Archive.h"
#include <string>
#include <vector>

namespace Pulsar {

  class ForEachProfile
  {
    std::string subint;
    std::string chan;
    std::string pol;

  public:

    ForEachProfile (std::vector<std::string>& tokens) { parse(tokens); }
    
    //! Parse any index specifications from an array of string
    /*! Any strings that are successfully parsed are removed from vector */
    void parse (std::vector<std::string>& tokens);

    // call the Profile member function with the specified argument
    template<typename Method, typename Argument>
      void operator () (Archive* archive, Method method, Argument argument)
    {
      std::vector<unsigned> subint_i;
      TextInterface::parse_indeces (subint_i, subint, archive->get_nsubint());

      //std::cerr << "subint size=" << subint_i.size() << std::endl;
      
      std::vector<unsigned> chan_i;
      TextInterface::parse_indeces (chan_i, chan, archive->get_nchan());

      //std::cerr << "chan size=" << chan_i.size() << std::endl;
      
      std::vector<unsigned> pol_i;
      TextInterface::parse_indeces (pol_i, pol, archive->get_npol());

      //std::cerr << "pol size=" << pol_i.size() << std::endl;
      
      for (unsigned isub=0; isub < subint_i.size(); isub++)
	for (unsigned ipol=0; ipol < pol_i.size(); ipol++)
	  for (unsigned ichan=0; ichan < chan_i.size(); ichan++)
	  {
	    Profile* profile = archive->get_Profile (subint_i[isub],
						     pol_i[ipol],
						     chan_i[ichan]);
	    (profile->*method)(argument);
	  }
    }
  };
  
}

#endif
