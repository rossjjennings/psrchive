//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_SplineSmooth_h
#define __Pulsar_SplineSmooth_h

#include "ReferenceAble.h"
#include "Estimate.h"

namespace Pulsar {

  //! Base class of penalized splines (p-spline) for smoothing
  /*! Derived classes interface with the SPLINT library */
  class SplineSmooth : public Reference::Able
  {
  public:

    //! Constructor
    SplineSmooth();

    //! Destructor
    ~SplineSmooth();

    //! Set the smoothing factor
    void set_alpha (double _alpha) { alpha = _alpha; }
    double get_alpha () const { return alpha; }

    //! Unload spline to specified filename
    void unload (const std::string&) const;
    //! Load spline from specified filename
    void load (const std::string&);

  protected:
    
    //! evaluate method used by derived types
    double evaluate (const std::vector<double>& xval);

    //! constructor used by derived types
    template<typename T>
    void new_spline (const std::vector<T>& data_x,
		     const std::vector< Estimate<double> >& data_y);

  private:
    
    //! Smoothing factor
    double alpha;

    class Handle;
    Handle* handle;
  };
  
  class SplineSmooth1D : public SplineSmooth
  {
    public:

      void fit (const std::vector< double >& data_x,
                     const std::vector< Estimate<double> >& data_y);

      double evaluate (double x);
  };

  class SplineSmooth2D : public SplineSmooth
  {
    public:

      void fit (const std::vector< std::pair<double,double> >& data_x,
                     const std::vector< Estimate<double> >& data_y);

      double evaluate ( const std::pair<double,double>& );
  };

  //! Determines the spline smoothing factor as in Clark (1977)
  /*! 
    The m-fold cross-validation technique is described in Section 4 of

    R. M. Clark, Non-Parametric Estimation of a Smooth Regression
    Function, Journal of the Royal Statistical Society. Series B
    (Methodological), 1977, Vol. 39, No. 1 (1977), pp. 107-113
    https://www.jstor.org/stable/2984885
  */
  class CrossValidatedSmooth2D
  {
    bool logarithmic;             // smoothing factors on logarithmic scale
    
    unsigned npartition;          // m=40 in Clark (1977)
    double validation_fraction;   // 0.1 in Clark (1977)
    
    SplineSmooth2D* spline;       // the 2-D spline implementation

    double iqr_threshold;         // Tukey's fence used to detect outliers
    double gof_step_threshold;
    
    std::vector<double> gof_tot;
    std::vector<unsigned> gof_count;

    std::string gof_filename;
    std::ofstream* gof_out;

  public:
    
    CrossValidatedSmooth2D ();

    void set_gof_filename (const std::string& name) { gof_filename = name; }
    
    void set_spline (SplineSmooth2D* _spline) { spline = _spline; }
  
    //! Fit spline to data using current configuration
    void fit ( std::vector< std::pair<double,double> >& data_x,
	       std::vector< Estimate<double> >& data_y );

    void remove_iqr_outliers
    ( std::vector< std::pair<double,double> >& x,
      std::vector< Estimate<double> >& y );

    void find_optimal_smoothing_factor
    ( const std::vector< std::pair<double,double> >& dat_x,
      const std::vector< Estimate<double> >& dat_y );
    
    void remove_gof_outliers
    ( std::vector< std::pair<double,double> >& x,
      std::vector< Estimate<double> >& y );

    //! Return the mean goodness-of-fit for the current smoothing
    double get_mean_gof (const std::vector< std::pair<double,double> >& data_x,
			 const std::vector< Estimate<double> >& data_y);

    //! Return the mean goodness-of-fit for the specified smoothing
    double get_mean_gof (double log_10_alpha,
			 const std::vector< std::pair<double,double> >& data_x,
			 const std::vector< Estimate<double> >& data_y);
    
  };

  class BootstrapUncertainty2D
  {
    unsigned nsample;
    SplineSmooth2D* spline;      // the spline implementation
  
  public:
    
    BootstrapUncertainty2D ();

    void set_spline (SplineSmooth2D* _spline) { spline = _spline; }
  
    void get_uncertainty (const std::vector< std::pair<double,double> >& data_x,
			  std::vector< Estimate<double> >& data_y);

  };
  
}

#endif

