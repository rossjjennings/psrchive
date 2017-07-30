//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ComplexRVMFit.h

#ifndef __Pulsar_ComplexRVMFit_h
#define __Pulsar_ComplexRVMFit_h

#include "MEAL/Axis.h"
#include "Estimate.h"

#include <complex>

// forward declarations
namespace MEAL
{
  class ComplexRVM;
}

typedef std::pair<double,double> range;

namespace Pulsar
{
  class PolnProfile;

  //! Fit rotating vector model to Stokes Q and U profiles
  class ComplexRVMFit : public Reference::Able
  {
    
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    ComplexRVMFit ();

    //! Add a range of pulse phase containing an orthogonally polarized mode
    /*! Must be called before set_observation */
    void add_opm (const range& radians);

    //! Return true if pulse phase falls within an OPM range
    bool is_opm (double phase_in_radians) const;

    //! Add a range of pulse phase to be included as constraints
    /*! Must be called before set_observation */
    void add_include (const range& radians);

    //! Return true if pulse phase falls within an included range
    bool is_included (double phase_in_radians) const;

    //! Add a range of pulse phase to be excluded from constraints
    /*! Must be called before set_observation */
    void add_exclude (const range& radians);

    //! Return true if pulse phase falls within an excluded range
    bool is_excluded (double phase_in_radians) const;

    //! Set the data to which model will be fit
    void set_observation (const PolnProfile*);
    //! Get the data to which model will be fit
    const PolnProfile* get_observation () const;

    //! Set the threshold below which data are ignored
    void set_threshold (float sigma);
    //! Get the threshold below which data are ignored
    float get_threshold () const;
    
    //! Set the smoothing window used to stabilize first guess
    void set_guess_smooth (unsigned phase_bins);
    //! Get the smoothing window used to stabilize first guess
    unsigned get_guess_smooth () const;
    
    //! Get the model to be fit to the data
    MEAL::ComplexRVM* get_model ();

    //! Fit data to the model
    void solve ();

    void set_chisq_map (bool flag) { chisq_map = flag; }

    void set_range_alpha (const range& r) { range_alpha = r; }
    void set_range_zeta (const range& r) { range_zeta = r; }
    void set_range_beta (const range& r) { range_beta = r; }

    void global_search (unsigned nalpha, unsigned nzeta);

    //! Evaluate the model at the specified pulse longitude (in radians)
    double evaluate (double phi_radians);

    float get_chisq () const { return chisq; }
    unsigned get_nfree () const { return nfree; }

    // get the post-fit residuals of Stokes Q and U
    void get_residuals (std::vector<double>& phases,
			std::vector< std::complex< Estimate<double> > >& res);

    // get the post-fit residuals of position angle
    void get_psi_residuals (std::vector<double>& phases,
			    std::vector< Estimate<double> >& res);

  protected:

    void check_parameters ();

    Reference::To<const PolnProfile> data;
    Reference::To<MEAL::ComplexRVM> model;

    float threshold;

    // ranges of pulse longitude to include in model
    std::vector<range> range_include;
    // ranges of pulse longitude to exclude from model
    std::vector<range> range_exclude;

    MEAL::Axis<unsigned> state;
    std::vector< MEAL::Axis<unsigned>::Value > data_x;
    std::vector< std::complex< Estimate<double> > > data_y;

    // orthogonally polarized mode phase ranges
    std::vector<range> opm;

    range range_alpha;
    range range_beta;
    range range_zeta;

    // length of the first guess ellipse along the sin(alpha) axis
    float guess_alpha;
    // length of the first guess ellipse along the sin(beta) axis
    float guess_beta;

    // number of phase bins in smoothing window used to stabilize first guess
    unsigned guess_smooth;

  private:

    // used by set_observation method to find the maximum in delpsi/delphi
    void find_delpsi_delphi_max ();

    std::vector< std::complex< Estimate<double> > > linear;
    double delpsi_delphi;

    float peak_phase;
    float peak_pa;

    float chisq;
    unsigned nfree;
    bool chisq_map;

    double max_L;
  };
}

#endif

