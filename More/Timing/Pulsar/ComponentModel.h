//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/ComponentModel.h

#ifndef __Pulsar_ComponentModel_h
#define __Pulsar_ComponentModel_h

#include "Pulsar/ProfileShiftEstimator.h"
#include "MEAL/Univariate.h"

#include <map>

namespace MEAL 
{
  class Scalar;
  class ScalarParameter;
  class ScaledVonMises;
  class ScaledVonMisesDeriv;
}

namespace Pulsar
{
  class Profile;
  class Archive;

  //! Models a pulse profile using multiple components
  /*! For now, only the von Mises distribution is used */
  class ComponentModel : public ProfileShiftEstimator
  {
  public:

    //! Default constructor
    ComponentModel ();

    //! Load model from file
    ComponentModel (const std::string& filename);

    //! Get the fraction of the pulse period modelled (in turns)
    double get_gate_duty_cycle() const { return gate; }
    
    //! Set the fraction of the pulse period modelled (in turns)
    virtual void set_gate_duty_cycle (double turns) { gate = turns; }

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return the name of the file from which the component model was loaded
    std::string get_name () const { return filename; }
    
    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Return a copy constructed instance of self
    ComponentModel* clone () const { return new ComponentModel(*this); }

    //! Get the component model
    MEAL::Univariate<MEAL::Scalar>* get_model ();
    
    //! Return the statistical goodness-of-fit
    double get_reduced_chisq () const { return chisq / nfree; }

    //! Return the absolute phase
    double get_absolute_phase () const;
    
    // I/O
    void load (const char *fname);
    void unload (const char *fname) const;

    //! All heights will be treated as log(height)
    void set_log_height (bool flag=true);
    bool get_log_height () const { return log_height; }

    //! Previous best fit model will be first guess on next call to get_shift
    void set_retain_memory (bool flag=true);
    bool get_retain_memory () const { return retain_memory; }

    void set_fix_widths (bool flag=true);
    bool get_fix_widths () const { return fix_widths; }

    void set_fit_primary_first (bool flag=true);
    bool get_fit_primary_first () const { return fit_primary_first; }

    void set_report_absolute_phases (bool flag=true);
    bool get_report_absolute_phases () const { return report_absolute_phases; }

    void set_report_widths (bool flag=true) { report_widths = flag; }
    bool get_report_widths () const { return report_widths; }

    // Manipulation
    /*! \param centre the centre of the component in turns of pulse phase
     *  \param concentration the concentration of the component equal 1 / variance in radians
     *  \param height is the absolute height of the component
     */
    void add_component (double centre, double concentration, double height,
			const char *name);

    void remove_component (unsigned icomp);

    MEAL::ScaledVonMises *get_component(unsigned icomp);

    unsigned get_ncomponents() const;

    //! Roughly align the phases and heights of the components to match the Profile
    void align (const Profile *profile);

    //! Roughly align the Profile to match the phases and heights of the components
    void align_to_model (Profile* profile);

    //! Returns the best fit phase and scale that aligns the model to the Profile
    void get_best_alignment (const Profile* profile, double& phase, double& scale);

    //! Fix the relative phases of the components
    void fix_relative_phases ();

    //! Remove components with small heights
    void set_zap_height_ratio (float r) { zap_height_ratio = r; }
    float get_zap_height_ratio () const { return zap_height_ratio; }

    //! Remove components with large concentrations (narrow widths)
    void set_zap_concentration_ratio (float r) { zap_concentration_ratio = r; }
    float get_zap_concentration_ratio () const { return zap_concentration_ratio; }

    // Fitting

    void set_fit_derivative (bool flag) { fit_derivative = flag; }
    void set_threshold (float t) { threshold = t; }

    void set_infit (unsigned icomponent, unsigned iparam, bool infit);
    void set_infit (const char *fitstring);

    void fit (const Profile *profile);

    float get_chisq () const { return chisq; }
    unsigned get_nfree () const { return nfree; }
    
    // Evaluating
    void evaluate (float *vals, unsigned nvals, int icomp=-1) ;

  protected:

    std::vector< Reference::To<MEAL::ScaledVonMises> > components;

    mutable std::vector< Reference::To<MEAL::ScaledVonMises> > backup;

    // set all infit flags of the specified component to false
    void freeze (unsigned icomponent) const;

    // restore all infit flags of the specified component from backup
    void unfreeze (unsigned icomponent) const;

    std::vector<std::string> component_names;

    mutable std::vector< Reference::To<MEAL::ScaledVonMisesDeriv> > derivative;

    //! comments, indexed by line number
    std::map<unsigned, std::string> comments;
    
    mutable Reference::To< MEAL::ScalarParameter > phase;
    mutable Reference::To< MEAL::Univariate<MEAL::Scalar> > model;

    double gate;
    
    bool fit_derivative;
    float threshold;

    bool log_height;
    bool retain_memory;
    bool fix_widths;
    bool fit_primary_first;
    bool report_absolute_phases;
    bool report_widths;
    
    float zap_height_ratio;
    float zap_concentration_ratio;

    void build () const;
    void check (const char* method, unsigned icomponent) const;

    void clear ();
    void init ();

  private:

    float chisq;
    unsigned nfree;
    std::string filename;
  };
}

#endif
