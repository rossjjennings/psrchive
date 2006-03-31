//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ColdPlasma.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/31 23:01:57 $
   $Author: straten $ */

#ifndef __Pulsar_ColdPlasma_h
#define __Pulsar_ColdPlasma_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"
#include "Physical.h"

namespace Pulsar {

  class Integration;
  class Archive;

  //! Corrects dielectric effects in cold plasma
  /*! Corrections are performed with respect to a reference frequency.
    By default, this is the centre frequency of the Integration;
    however, it is possible to correct with respect to an arbitrary
    frequency or wavelength (including zero wavelength = infinite
    frequency).
  */
  template<class Corrector, class History>
  class ColdPlasma : public Transformation<Integration> {

  public:

    //! Default constructor
    ColdPlasma () { name = "ColdPlasma"; }

    //! Derived classes must return the measure to be passed to the Corrector
    virtual double correction_measure (Integration*) = 0;

    //! Derived classes must define the identity
    virtual typename Corrector::Return get_identity () = 0;

    //! Derived classes must define how to apply the correction
    virtual void apply (Integration*, unsigned channel) = 0;

    //! Execute the correction for an entire Pulsar::Archive
    virtual void execute (Archive*);

    //! The default Faraday rotation correction
    void transform (Integration*);

    //! Set up internal variables before execution
    /* \post reference_frequency = Integration::get_centre_frequency. */
    void setup (Integration*);

    //! Execute the correction
    /* \post All data will be corrected to the reference frequency */
    void execute (Integration*);

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength
    double get_reference_wavelength () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the correction measure
    void set_measure (double measure);
    //! Get the correction measure
    double get_measure () const;

    //! Set the correction due to a change in reference wavelength
    void set_delta (const typename Corrector::Return& d) { delta = d; }
    //! Get the correction due to a change in reference wavelength
    typename Corrector::Return get_delta () const { return delta; }

  protected:

    friend class Integration;

    //! Execute the correction on the selected range
    void range (Integration*, unsigned ichan, unsigned jchan);

    //! The dielectric effect corrector
    Corrector corrector;

    //! The correction due to a change in reference wavelength
    typename Corrector::Return delta;

    //! The name to be used in verbose messages
    std::string name;

    //! The name of the correction measure
    std::string val;
  };

}


template<class C, class H>
void Pulsar::ColdPlasma<C,H>::setup (Integration* data)
{
  set_reference_frequency( data->get_centre_frequency() );
  set_measure( correction_measure(data) );
}
 
template<class C, class H>
void Pulsar::ColdPlasma<C,H>::transform (Integration* data) try
{
  setup (data);
  execute (data);
}
catch (Error& error) {
  throw error += "Pulsar::" + name + "::transform";
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::execute (Archive* arch)
{
  for (unsigned i=0; i<arch->get_nsubint(); i++)
    execute( arch->get_Integration(i) );
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::set_reference_frequency (double MHz)
{
  corrector.set_reference_frequency (MHz);
}

template<class C, class H>
double Pulsar::ColdPlasma<C,H>::get_reference_frequency () const
{
  return corrector.get_reference_frequency ();
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::set_reference_wavelength (double metres)
{
  corrector.set_reference_wavelength (metres);
}

template<class C, class H>
double Pulsar::ColdPlasma<C,H>::get_reference_wavelength () const
{
  return corrector.get_reference_wavelength ();
}

template<class C, class H>
void Pulsar::ColdPlasma<C,H>::set_measure (double measure)
{
  corrector.set_measure (measure);
}

template<class C, class H>
double Pulsar::ColdPlasma<C,H>::get_measure () const
{
  return corrector.get_measure ();
}

template<class C, class History>
void Pulsar::ColdPlasma<C,History>::execute (Integration* data) try
{
  double measure = get_measure();
  History* corrected = data->template get<History>();
 
  if ( corrected ) {

    double corrected_measure = corrected->get_measure();
    double lambda = corrected->get_reference_wavelength();

    if (corrected_measure == measure && lambda == get_reference_wavelength()) {
      if (Integration::verbose)
	cerr << "Pulsar::"+name+"::execute data are corrected" << endl;
      return;
    }

    // calculate the rotation arising from the new centre frequency, if any
    corrector.set_wavelength( lambda );
    delta = corrector.evaluate();

    // set the effective rotation measure to the difference
    set_measure( measure - corrected_measure );

  }
  else
    delta = get_identity();

  if (Integration::verbose)
    cerr << "Pulsar::"+name+"::execute"
      " effective "+val+"=" << get_measure() <<
      " reference wavelength=" << get_reference_wavelength() << endl;

  range (data, 0, data->get_nchan());

  // restore the original rotation measure
  set_measure( measure );

  if (!corrected) {
    corrected = new History;
    data->add_extension( corrected );
  }

  corrected->set_measure( measure );
  corrected->set_reference_wavelength( get_reference_wavelength() );

}
catch (Error& error) {
  throw error += "Pulsar::"+name+"::execute";
}


/*! This worker method corrects Faraday rotation without asking many
  questions.

   \param ichan the first channel to be corrected
   \param kchan one more than the last channel to be corrected

   \pre the measure and reference_wavelength attributes will
   have been set prior to calling this method

   \pre the delta attribute will have been properly set or reset
*/
template<class C, class H>
void Pulsar::ColdPlasma<C,H>::range (Integration* data,
				     unsigned ichan, unsigned kchan) try
{

  if (Integration::verbose)
    cerr << "Pulsar::"+name+"::range "+val+"=" << get_measure()
	 << " lambda_0=" << get_reference_wavelength() << " m" 
         << " delta=" << delta << endl;

  if (get_measure() == 0 && delta == get_identity())
    return;

  if (ichan >= data->get_nchan())
    throw Error (InvalidRange, "Pulsar::"+name+"::range",
                 "start chan=%d >= nchan=%d", ichan, data->get_nchan());

  if (kchan > data->get_nchan())
    throw Error (InvalidRange, "Pulsar::"+name+"::range",
                 "end chan=%d > nchan=%d", kchan, data->get_nchan());

  for (unsigned jchan=ichan; jchan < kchan; jchan++) {
    corrector.set_frequency( data->get_centre_frequency (jchan) );
    apply (data, jchan);
  }

}
catch (Error& error) {
  throw error += "Pulsar::"+name+"::range";
}

#endif
