



#ifndef __Cal_Ph_V_Freq_Plot_h
#define __Cal_Ph_V_Freq_Plot_h


#include <Pulsar/Plot.h>
#include <Pulsar/SimplePlot.h>
#include <Pulsar/PlotIndex.h>
#include <vector>


namespace Pulsar
{
  /**
   * Plots the calibrator phase vs frequency.
   **/
  class CalPhVFreqPlot : public SimplePlot
  {
  public:
    CalPhVFreqPlot();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    //! Get the default label for the x axis
    std::string get_xlabel (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! do some preparation ???
    void prepare (const Archive*);

    //! preprocess, overriden to not remove baseline
    virtual void preprocess (Archive* archive) {}

    //! draw the angle as a function of pulse phase
    void draw (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (const PlotIndex& _isubint) { isubint = _isubint; }
    PlotIndex get_subint () const { return isubint; }

    //! Set the polarization to plot (where applicable)
    void set_pol( const PlotIndex & _ipol ) { ipol = _ipol; }
    PlotIndex get_pol() const { return ipol; }

    const void minmaxval( std::vector<float> thearray, float &min, float &max );

    // Text interface to the AnglePlot class
  class Interface : public TextInterface::To<CalPhVFreqPlot>
    {
    public:
      Interface (CalPhVFreqPlot *s_instance = 0);
    };

  private:
    PlotIndex ipol;
    PlotIndex isubint;

    unsigned int npol;
    unsigned int nchan;

    Reference::To<Archive> copy;
    
    std::vector<float> cal, sys, xx;
	
    float mincal;
    float maxcal;
    float minsys;
    float maxsys;
  };

}

#endif // __Cal_Ph_V_Freq_Plot_h


