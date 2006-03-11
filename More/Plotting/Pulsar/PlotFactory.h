//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFactory.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/11 21:51:48 $
   $Author: straten $ */

#ifndef __Pulsar_PlotFactory_h
#define __Pulsar_PlotFactory_h

#include <string>
#include <vector>

namespace Pulsar {

  class Plot;

  //! Provides a text interface to get and set Plot attributes
  class PlotFactory {

  public:

    //! Default constructor loads up all of the plot types
    PlotFactory ();

    //! Return a new instance of the named plot
    Plot* construct (std::string name);

    //! Return the number of plots
    unsigned get_nplot () { return agents.size(); }

    //! Return the name of the ith plot
    std::string get_name (unsigned i);

    //! Return the name of the ith plot
    std::string get_description (unsigned i);

  protected:

    class Agent;
    
    std::vector<Agent*> agents;

  public:

    template<class P> class Advocate;

    //! Add a plot
    void add (Agent* agent) { agents.push_back(agent); }

  };

  class PlotFactory::Agent
  {
  public:

    Agent (std::string n, std::string d) : name (n), description (d) {}

    //! Return a new instance of Plot class
    virtual Plot* construct () = 0;

    //! Return the name of the Plot class
    std::string get_name () { return name; }

    //! Return a description of the Plot class
    std::string get_description () { return description; }

  protected:

    std::string name;
    std::string description;

  };

  template<class P> class PlotFactory::Advocate : public Agent
  {  
  public:

    //! Construct with name and description
    Advocate (std::string _name, std::string _description)
      : Agent (_name, _description) {}
 
    //! Return a new instance of Plot class
    Plot* construct () { return new P; }

  };

}

#endif
