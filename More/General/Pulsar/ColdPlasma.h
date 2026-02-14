//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ColdPlasma.h

#ifndef __Pulsar_ColdPlasma_h
#define __Pulsar_ColdPlasma_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Physical.h"

namespace Pulsar {

  //! Corrects dielectric effects in cold plasma
  /*! Corrections are performed with respect to a reference frequency.
    By default, this is the centre frequency of the Integration;
    however, it is possible to correct with respect to an arbitrary
    frequency or wavelength (including zero wavelength = infinite
    frequency).

    The main point to this template base class is to standardize
    dispersion and Faraday rotation corrections.  The data to be
    corrected may have already been corrected once before, with a
    different dispersion/rotation measure and/or refererence
    wavelength.  This class ensures that past corrections are treated
    properly, through use of a History class (an Integration::Extension).

  */
  template<class Calculator, class History>
  class ColdPlasma : public Transformation<Integration> {

  public:

    /*! For dispersion, Type = double; for Faraday rotation, Type = Jones<double> */
    typedef typename Calculator::Return Type;

    //! Default constructor
    ColdPlasma () { name = "ColdPlasma"; }

    //! Return the measure to be corrected with respect to centre frequency
    /*! As returned by psredit -c dm or rm */
    virtual double get_relative_measure (const Integration*) const = 0;

    //! Return true if the relative measure has been corrected with respect to centre frequency
    /*! As returned by psredit -c dmc or rmc */
    virtual bool get_relative_corrected (const Integration*) const = 0;
    
    //! Return the measure to be corrected with respect to infinite frequency
    /*! As returned by psredit -c int:aux:dm or int:aux:rm */
    virtual double get_absolute_measure (const Integration*) const = 0;

    //! Return true if the relative measure has been corrected with respect to centre frequency
    /*! As returned by psredit -c aux:dmc or aux:rmc */
    virtual bool get_absolute_corrected (const Integration*) const = 0;

    //! Derived classes must define the identity
    /*! For dispersion, returns 0; for Faraday rotation, returns the identity matrix */
    virtual Type get_identity () const = 0;

    //! Derived classes must define the operator
    /*! For dispersion, this performs scalar addition; for Faraday rotation, matrix multiplication */
    virtual void combine (Type& result, const Type& add) const = 0;

    //! Derived classes must define how to apply the correction
    virtual void apply (Integration*, unsigned channel, Type to_be_corrected) = 0;

    //! Execute the correction for an entire Archive
    virtual void execute (Archive*);

    //! Just do the correction (off the books)
    void just_do_it (Archive*);

    //! Revert the correction for an entire Archive
    virtual void revert (Archive*);

    //! The default correction
    void transform (Integration*);

    //! Set up internal variables before execution
    /*! \post reference_frequency = Integration::get_centre_frequency */
    void setup (const Integration*);

    //! update internal variables before execution
    /*! \post measure -= corrected_measure; and delta is initialized */
    virtual void update (const Integration*);

    //! Calls setup then update
    virtual void set (const Integration* data)
    { setup (data); update (data); }

    //! Set the frequency for which the correction will be computed
    virtual void set_Profile (const Profile* data)
    { set_frequency( data->get_centre_frequency () ); }

    //! Set the frequency for which the correction will be computed
    virtual void set_frequency (double frequency)
    { relative.set_frequency(frequency); absolute.set_frequency(frequency); }

    //! Execute the correction for the current get_reference_frequency and get_measure
    /* \post All data will be corrected to the reference frequency 
    
      Calls update and range, then updates the history. */
    void execute1 (Integration*);

    //! Undo the relative correction
    /* \post All corrections with respect to the reference frequency will be inverted */
    void revert_relative (Integration*);

    //! Correct the selected range according to effective_measure
    void correct (Integration*, unsigned start_chan, unsigned end_chan, double freq);

    //! Correct the second argument as the first argument was corrected
    void match (const Integration* reference, Integration* to_be_corrected);

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength in metres
    double get_reference_wavelength () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the correction due to a change in reference wavelength
    void set_delta (const Type& d) { delta = d; }

    //! Get the correction due to a change in reference wavelength
    Type get_delta () const { return delta; }

  protected:

    friend class Integration;

    //! Execute the correction on the selected range
    void range (Integration*, unsigned start_chan, unsigned end_chan);

    //! Computes the effect to be corrected with respect to reference frequency
    Calculator relative;

    //! update the relative transformation based on past correction
    virtual void update_relative (const Integration*);

    //! Computes the effect to be corrected with respect to infinite frequency
    Calculator absolute;

    //! update the absolute transformation based on past correction
    virtual void update_absolute (const Integration*);

    //! The correction due to a change in reference wavelength
    Type delta;

    //! The name to be used in verbose messages
    std::string name;

    //! The name of the correction measure
    std::string val;
  };
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::setup (const Integration* data)
{
  set_reference_frequency( data->get_centre_frequency() );
  relative.set_measure( get_relative_measure(data) );

  if (Integration::verbose)
    std::cerr << "Pulsar::" + name + "::setup lambda=" 
	      << get_reference_wavelength() << " measure=" << relative.get_measure()
	      << std::endl;
}
 
template<class C, class H>
void Pulsar::ColdPlasma<C,H>::transform (Integration* data) try
{
  setup (data);
  execute1 (data);
}
catch (Error& error)
{
  throw error += "Pulsar::" + name + "::transform";
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::execute (Archive* arch)
{
  for (unsigned i=0; i<arch->get_nsubint(); i++)
    execute1( arch->get_Integration(i) );
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::revert (Archive* arch)
{
  for (unsigned i=0; i<arch->get_nsubint(); i++)
    revert_relative( arch->get_Integration(i) );
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::just_do_it (Archive* arch)
{
  delta = get_identity ();

  for (unsigned i=0; i<arch->get_nsubint(); i++)
    range (arch->get_Integration(i), 0, arch->get_nchan());
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::set_reference_frequency (double MHz)
{
  relative.set_reference_frequency (MHz);
}

template<class C, class H>
double Pulsar::ColdPlasma<C,H>::get_reference_frequency () const
{
  return relative.get_reference_frequency ();
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::set_reference_wavelength (double metres)
{
  relative.set_reference_wavelength (metres);
}

template<class C, class H>
double Pulsar::ColdPlasma<C,H>::get_reference_wavelength () const
{
  return relative.get_reference_wavelength ();
}

template<class Calculator, class History>
void Pulsar::ColdPlasma<Calculator,History>::update (const Integration* data)
try
{
  update_relative(data);
  update_absolute(data);
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::update";
}

template<class Calculator, class History>
void Pulsar::ColdPlasma<Calculator,History>::update_relative (const Integration* data) try
{
  double relative_measure = get_relative_measure (data);

  if (Integration::verbose)
    std::cerr << "Pulsar::" + name + "::update_relative measure=" << relative_measure << std::endl;

  relative.set_measure(relative_measure);

  delta = get_identity();

  if (get_relative_corrected(data))
  {
    double corrected_measure = relative_measure;

    auto history = data->template get<History>();
    if (history && history->get_relative()->get_corrected())
    {
      corrected_measure = history->get_relative()->get_measure();
      double lambda = history->get_relative()->get_reference_wavelength();

      if (Integration::verbose)
        std::cerr << "Pulsar::" + name + "::update_relative corrected"
                    " measure=" << corrected_measure << " lambda=" << lambda << std::endl;

      // calculate the correction due to the new centre frequency, if any
      relative.set_wavelength( lambda );
      delta = relative.evaluate();
    }

    // remaining correction is due to change in corrected measure, if any
    relative.set_measure(relative_measure - corrected_measure);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::update_relative";
}

template<class Calculator, class History>
void Pulsar::ColdPlasma<Calculator,History>::update_absolute (const Integration* data) try
{
  double absolute_measure = get_absolute_measure (data);

  if (Integration::verbose)
    std::cerr << "Pulsar::" + name + "::update_absolute measure=" << absolute_measure << std::endl;

  absolute.set_measure(absolute_measure);
  absolute.set_reference_wavelength(0.0);

  if (get_absolute_corrected(data))
  {
    double corrected_measure = absolute_measure;

    auto history = data->template get<History>();
    if (history && history->get_absolute()->get_corrected())
    {
      corrected_measure = history->get_absolute()->get_measure();
    }

    if (Integration::verbose)
      std::cerr << "Pulsar::" + name + "::update_absolute corrected"
                  " measure=" << corrected_measure << std::endl;

    // remaining correction is due to change in corrected measure, if any
    absolute.set_measure(absolute_measure - corrected_measure);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::update_absolute";
}

template<class C, class History>
void Pulsar::ColdPlasma<C,History>::execute1 (Integration* data) try
{
  update (data);

  if (Integration::verbose)
    std::cerr << "Pulsar::"+name+"::execute1"
      " relative "+val+"=" << relative.get_measure() <<
      " reference wavelength=" << get_reference_wavelength() << std::endl;

  range (data, 0, data->get_nchan());

  double relative_measure = get_relative_measure (data);
  History* history = data->template getadd<History>();
  history->get_relative()->set_corrected( true );
  history->get_relative()->set_measure( relative_measure );
  history->get_relative()->set_reference_wavelength( get_reference_wavelength() );

  double absolute_measure = get_absolute_measure (data);
  if (absolute_measure)
  {
    History* history = data->template getadd<History>();
    history->get_absolute()->set_corrected( true );
    history->get_absolute()->set_measure( absolute_measure );
    history->get_absolute()->set_reference_wavelength( 0.0 );
  }
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::execute1";
}

template<class C, class History>
void Pulsar::ColdPlasma<C,History>::revert_relative (Integration* data) try
{
  auto history = data->template get<History>();
  if (!history)
    throw Error (InvalidState, "Pulsar::" + name + "::revert_relative", "no correction history");

  if (!history->get_relative()->get_corrected())
  {
    if (Integration::verbose)
      std::cerr << "Pulsar::" + name + "::revert_relative not corrected" << std::endl;
    return;
  }

  relative.set_measure( -history->get_relative()->get_measure() );
  set_reference_wavelength( history->get_relative()->get_reference_wavelength() );
  set_delta( get_identity() );

  if (Integration::verbose)
    std::cerr << "Pulsar::"+name+"::revert_relative"
      " relative "+val+"=" << relative.get_measure() <<
      " reference wavelength=" << get_reference_wavelength() << std::endl;

  range (data, 0, data->get_nchan());

  // this should remove the history
  delete history;
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::revert_relative";
}

//! Correct the second argument as the first argument was corrected
template<class C, class History>
void Pulsar::ColdPlasma<C,History>::match (const Integration* reference, Integration* to_correct)
{
  const History* history = reference->template get<History>();
  if (!history)
    throw Error (InvalidState, "Pulsar::" + name + "::match",
		 "reference has no correction history");

  relative.set_measure( history->get_relative()->get_measure() );
  set_reference_wavelength( history->get_relative()->get_reference_wavelength() );
 
  execute1( to_correct );
}

/*! This worker method performs the correction on a specified range
    of frequency channels

    \param start_chan the first channel to be corrected
    \param end_chan one more than the last channel to be corrected

    \pre the measure and reference_wavelength attributes will
    have been set prior to calling this method

    \pre the delta attribute will have been properly set or reset
*/
template<class C, class H>
void Pulsar::ColdPlasma<C,H>::range (Integration* data, unsigned start_chan, unsigned end_chan) try
{
  if (Integration::verbose)
    std::cerr << "Pulsar::"+name+"::range "+val
        << " relative=" << relative.get_measure()
        << " absolute=" << absolute.get_measure()
        << " lambda_0=" << get_reference_wavelength() << " m" 
        << " delta=" << delta << std::endl;

  if (relative.get_measure() == 0 && absolute.get_measure() == 0 && delta == get_identity())
  {
    if (Integration::verbose)
      std::cerr << "Pulsar::"+name+"::range nothing to correct" << std::endl;
    return;
  }

  if (start_chan >= data->get_nchan())
    throw Error (InvalidRange, "Pulsar::"+name+"::range",
                 "start chan=%d >= nchan=%d", start_chan, data->get_nchan());

  if (end_chan > data->get_nchan())
    throw Error (InvalidRange, "Pulsar::"+name+"::range",
                 "end chan=%d > nchan=%d", end_chan, data->get_nchan());

  for (unsigned ichan=start_chan; ichan < end_chan; ichan++)
  {
    set_frequency( data->get_centre_frequency (ichan) );

    Type result = delta;
    combine(result, relative.evaluate());
    combine(result, absolute.evaluate());

    if (Profile::verbose)
      std::cerr << "Pulsar::"+name+"::range ichan=" << ichan
                << "\n\t relative=" << relative.evaluate()
                << "\n\t absolute=" << absolute.evaluate() << std::endl;

    apply (data, ichan, result);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::range";
}


/*! Calls setup, update, and range, but does not modify history

   \param start_chan the first channel to be corrected
   \param end_chan one greater than the last channel to be corrected
   \param reference_frequency the reference frequency
*/
template<class C, class H>
void Pulsar::ColdPlasma<C,H>::correct (Integration* subint,
				       unsigned start_chan, unsigned end_chan, 
				       double reference_frequency) 
try
{
  setup(subint);
  set_reference_frequency( reference_frequency );
  update(subint);

  if (Integration::verbose)
    std::cerr << "Pulsar::" + name + "::correct"
                 " reference freq=" << reference_frequency << 
                 " relative measure=" << relative.get_measure() <<
                 " absolute measure=" << absolute.get_measure() << std::endl;

  range (subint, start_chan, end_chan);
}
catch (Error& error)
{
  throw error += "Pulsar::"+name+"::correct";
}


#endif
