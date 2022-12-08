//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten and Fabian Jankowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/DeleteInterpreter.h

#ifndef __Pulsar_DeleteInterpreter_h
#define __Pulsar_DeleteInterpreter_h

#include "Pulsar/InterpreterExtension.h"
#include "Pulsar/Config.h"
#include <vector>

namespace Pulsar {

  class DeleteInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    DeleteInterpreter ();

    //! Destructor
    ~DeleteInterpreter ();

    //! delete the specified channels
    std::string chan (const std::string& args);

    //! delete the specified integrations
    std::string subint (const std::string& args);

    //! delete the specified fraction of the band from the edges
    std::string edge (const std::string& args);

    //! delete the specified ranges of frequencies
    std::string freq (const std::string& args);

    //! delete the specified channels from the CalibratorExtension
    std::string cal (const std::string& args);

    //! delete the specified phase bins
    std::string bin (const std::string& args);

    //! no empty commands
    std::string empty ();

  protected:

    void delete_channels (std::vector<unsigned>& channels);

    static Option<bool> adjust_metadata_while_deleting_channels;
  };

}

#endif
