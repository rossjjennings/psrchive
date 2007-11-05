


#ifndef DIGITISER_STATS_H_
#define DIGITISER_STATS_H_


#include "Pulsar/SimplePlot.h"
#include <Pulsar/DigitiserStatistics.h>
#include <vector>



using namespace std;



namespace Pulsar
{
  class DigitiserStatsPlot : public SimplePlot
  {
  public:
    DigitiserStatsPlot();

  class Interface : public TextInterface::To<DigitiserStatsPlot>
    {
    public:
      Interface( DigitiserStatsPlot *s_instance = NULL );
    };

    TextInterface::Parser *get_interface();

    std::string get_xlabel( const Archive * );
    std::string get_ylabel( const Archive * );

    void AdjustSubRange( void );
    bool CheckStats( Reference::To<DigitiserStatistics> ext );

    void prepare( const Archive * );
    void draw( const Archive * );

    void set_subint( int s_subint ) { subint = s_subint; }
    int get_subint() const { return subint; }

    void set_fsub( int s_fsub ) { fsub = s_fsub; }
    int get_fsub() const { return fsub; }

    void set_lsub( int s_lsub ) { lsub = s_lsub; }
    int get_lsub() const { return lsub; }
  private:
    float y_min, y_max;
    vector< vector< vector< float > > > profiles;
    bool valid_archive;

    int subint;
    int fsub, lsub;

    int ncycsub;
    int ndigr;
    int npar;
    int nsub;
  };
}

#endif
