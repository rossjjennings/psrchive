


#ifndef DIGITISER_STATS_H_
#define DIGITISER_STATS_H_


#include "Pulsar/SimplePlot.h"


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

    void prepare( const Archive * );
    void draw( const Archive * );
  };
}

#endif
