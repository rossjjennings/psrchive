//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotFactory.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/05 21:07:29 $
   $Author: straten $ */

#ifndef __Pulsar_PlotterFactory_h
#define __Pulsar_PlotterFactory_h

#include <string>
#include <vector>

namespace Pulsar {

  class ProfilePlotter;

  //! Provides a text interface to get and set ProfilePlotter attributes
  class PlotterFactory {

  public:

    //! Return a new instance of the named plotter
    ProfilePlotter* construct (std::string name);

    //! Return the number of plotters
    unsigned get_nplot () { return agents.size(); }

    //! Return the name of the ith plotter
    std::string get_name (unsigned i);

    //! Return the name of the ith plotter
    std::string get_description (unsigned i);

  protected:

    class Agent;
    
    std::vector<Agent*> agents;

  public:

    template<class P> class Advocate;

    //! Add a plotter
    void add (Agent* agent) { agents.push_back(agent); }

  };

  class PlotterFactory::Agent
  {
  public:

    Agent (std::string n, std::string d) : name (n), description (d) {}

    //! Return a new instance of Plotter class
    virtual ProfilePlotter* construct () = 0;

    //! Return the name of the Plotter class
    std::string get_name () { return name; }

    //! Return a description of the Plotter class
    std::string get_description () { return description; }

  protected:

    std::string name;
    std::string description;

  };

  template<class P> class PlotterFactory::Advocate : public Agent

  {  
  public:

    //! Construct with name and description
    Advocate (std::string _name, std::string _description)
      : Agent (_name, _description) {}
 
    //! Return a new instance of Plotter class
    ProfilePlotter* construct () { return new P; }

  };

}

#endif
