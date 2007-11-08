/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#ifndef DIGITISER_COUNTS_PLOT_H_
#define DIGITISER_COUNTS_PLOT_H_



#include "Pulsar/SimplePlot.h"
#include <Pulsar/DigitiserCounts.h>
#include <vector>


using namespace std;



namespace Pulsar
{
  class DigitiserCountsPlot : public SimplePlot
  {
  public:
    DigitiserCountsPlot();

    TextInterface::Parser *get_interface();

  class Interface : public TextInterface::To<DigitiserCountsPlot>
    {
    public:
      Interface( DigitiserCountsPlot *s_instance = NULL );
    };
    
    void CheckCounts( const Archive *data );

    void prepare( const Archive *data );
    void draw( const Archive *data );

    std::string get_xlabel( const Archive *data );
    std::string get_ylabel( const Archive *data );

    int get_fsub( void ) const { return fsub; }
    void set_fsub( int s_fsub ) { fsub = s_fsub; }

    int get_lsub( void ) const { return lsub; }
    void set_lsub( int s_lsub ) { lsub = s_lsub; }

    int get_subint( void ) const { return subint; }
    void set_subint( int s_subint ) { subint = s_subint; }

  private:
    vector< vector< float > > adjusted_data;

    float min_count;
    float max_count;
    float y_jump;
    int first_nz;
    int last_nz;
    int subint, fsub, lsub;
    
    bool valid_data;
  };
}



#endif


