



#ifndef BANDPASS_PLOT_H_
#define BANDPASS_PLOT_H_



#include "Pulsar/FrequencyPlot.h"
#include <vector>



namespace Pulsar
{
  class BandpassPlot : public FrequencyPlot
  {
  public:
    BandpassPlot();
    ~BandpassPlot();

  class Interface : public TextInterface::To<BandpassPlot>
    {
    public:
      Interface ( BandpassPlot *s_instance = 0 );
    };

    TextInterface::Parser *get_interface();

    void prepare( const Archive * );
    
    virtual void preprocess( Archive *arch ) {}
    
    void draw( const Archive * );
  private:
    std::vector<double> means;
    std::pair<double,double> means_minmax;
  };
}


#endif

