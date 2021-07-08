//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BackendEstimate.h

#ifndef __Pulsar_BackendEstimate_H
#define __Pulsar_BackendEstimate_H

#include "MEAL/Complex2.h"
#include "MEAL/Mean.h"

#include "MJD.h"

using namespace std;

class setMJD
{
  bool set;
  MJD mjd;

public:
  setMJD () { set = false; }

  operator const MJD& () const { return mjd; }
  
  const setMJD& operator = (const MJD& _mjd)
  { mjd = _mjd; set = true; return *this; }

  friend bool operator < (const setMJD& setmjd, const MJD& mjd)
  { return !setmjd.set || setmjd.mjd < mjd; }

  friend bool operator > (const setMJD& setmjd, const MJD& mjd)
  { return !setmjd.set || setmjd.mjd > mjd; }

  friend bool operator < (const MJD& mjd, const setMJD& setmjd)
  { return !setmjd.set || mjd < setmjd.mjd; }

  friend bool operator > (const MJD& mjd, const setMJD& setmjd)
  { return !setmjd.set || mjd > setmjd.mjd; }

  friend ostream& operator << (ostream& os, const setMJD& setmjd)
  { if (!setmjd.set) os << "unset"; else os << setmjd.mjd; return os; }
};
  
namespace Calibration
{
  //! Manages a single backend and its current best estimate (first guess)
  class BackendEstimate : public Reference::Able
  {

  protected:
    
    //! The backend component
    Reference::To< MEAL::Complex2 > backend;

    //! The mean of the backend component
    Reference::To< MEAL::Mean<MEAL::Complex2> > mean;

    //! The start of the period spanned by this backend solution
    setMJD start_time;

    //! The end of the period spanned by this solution
    setMJD end_time;

    //! The mimimum observation time
    MJD min_time;

    //! The maximum observation time
    MJD max_time;

    float weight;

  public:

    BackendEstimate () { weight = 0.0; }

    //! The index of the SignalPath in which this backend is a component
    unsigned path_index;

    //! Set the response that contains the backend
    virtual void set_response (MEAL::Complex2* xform);

    //! Integrate a calibrator solution
    void integrate (const MEAL::Complex2* xform);

    //! Update the transformation with the current estimate, if possible
    virtual void update ();

    //! Return the path index
    unsigned get_path_index() const { return path_index; }

    void set_start_time (const MJD& epoch) { start_time = epoch; }
    const MJD& get_start_time () const { return start_time; }
    
    void set_end_time (const MJD& epoch) { end_time = epoch; }
    const MJD& get_end_time () const { return end_time; }
    
    //! Return true if this estimate spans the epoch
    bool spans (const MJD& epoch);

    //! update min_time and max_time
    void add_observation_epoch (const MJD& epoch);

    void add_weight (float amount) { weight += amount; }
    float get_weight () const { return weight; }
  };

}

#include "MEAL/ChainRule.h"
#include "MEAL/ProductRule.h"

namespace MEAL
{
  template<class Type, class Base>
  Type* extract (Base* base)
  {
    Type* type = dynamic_cast<Type*>( base );
    if (type)
      return type;
      
    MEAL::ChainRule<Base>* chain;
    chain = dynamic_cast<MEAL::ChainRule<Base>*>( base );
    if (chain)
      return extract<Type> ( chain->get_model() );
  
    MEAL::ProductRule<Base>* product;
    product = dynamic_cast<MEAL::ProductRule<Base>*>( base );
    if (product)
    {
      for (unsigned imodel=0; imodel<product->get_nmodel(); imodel++)
      {
	Type* type = extract<Type> ( product->get_model(imodel) );
	if (type)
	  return type;
      }
    }

    return 0;
  }

  template<class Base>
  void print (std::ostream& os, Base* base, const std::string& indent = "")
  {
    os << indent << (void*) base << " " << base->get_name() << endl;
      
    MEAL::ChainRule<Base>* chain;
    chain = dynamic_cast<MEAL::ChainRule<Base>*>( base );
    if (chain)
    {
      Base* model = chain->get_model();
      print (os, model, indent + " model ");

      for (unsigned i=0; i < model->get_nparam(); i++)
	if (chain->has_constraint (i))
	  print (os, chain->get_constraint(i),
		 indent + " by[" + tostring(i) + "] ");

      return;
    }

    MEAL::ProductRule<Base>* product;
    product = dynamic_cast<MEAL::ProductRule<Base>*>( base );
    if (product)
    {
      for (unsigned imodel=0; imodel<product->get_nmodel(); imodel++)
	print (os, product->get_model(imodel), indent + " ");

      return;
    }

    for (unsigned i=0; i<base->get_nparam(); i++)
      os << indent << " " << i << " " << base->get_param (i)
	 << " " << base->get_infit(i) << endl;
    
  }
}

#endif

