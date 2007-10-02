/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#ifndef Freq_Vs_SNR_Plot_h_
#define Freq_Vs_SNR_Plot_h_



#include "Pulsar/SimplePlot.h"
#include <Pulsar/Archive.h>
#include <TextInterface.h>



class FreqVsSNRPlot : public Pulsar::SimplePlot
{
public:
  FreqVsSNRPlot();

  void prepare( const Pulsar::Archive * );
  void draw( const Pulsar::Archive * );

  std::string get_xlabel( const Pulsar::Archive * );
  std::string get_ylabel( const Pulsar::Archive * );

class Interface : public TextInterface::To<FreqVsSNRPlot>
  {
  public:
    Interface( FreqVsSNRPlot *s_instance );
  };

  TextInterface::Parser *get_interface( void );
  
  void set_pol( int new_pol ) { pol = new_pol; }
  void set_subint( int new_subint ) { subint = new_subint; }
  int get_pol( void ) const { return pol; }
  int get_subint( void ) const { return subint; }
private:
  int subint;
  int pol;
};




#endif

