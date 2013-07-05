//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Cast.h

#ifndef __MEAL_Cast_H
#define __MEAL_Cast_H

namespace MEAL {

  //! Converts an evaluable model to one of another type

  template<class To, class From>
  class Cast : public To
  {

  public:

    typedef typename To::Result Result;

    //! Default constructor
    Cast (From* _from)
    {
      from = _from;  
      this->copy_parameter_policy  (from);
    }

    std::string get_name () const
    { return std::string("Cast")
	+ "<To=" + To::Name + ",From=" + From::Name + ">"; }

  protected:

    //! Convert
    void calculate (Result& result, std::vector<Result>* gradient)
    {
      std::vector<typename From::Result> from_grad;
      std::vector<typename From::Result>* from_gradptr = &from_grad;
      if (!gradient)
	from_gradptr = 0;

      typename From::Result from_result = from->evaluate (from_gradptr);

      result = static_cast<Result> (from_result);

      if (!gradient)
	return;

      gradient->resize( from_grad.size() );
      for (unsigned igrad=0; igrad<from_grad.size(); igrad++)
	(*gradient)[igrad] = static_cast<Result> (from_grad[igrad]);
    }

    //! The model to be cast to a different type
    Reference::To<From> from;

  };

  template<class To, class From>
  To* cast (From* from)
  { return new Cast<To, From> (from); }

  template<class To, class From>
  To* cast (Reference::To<From>& from)
  { return new Cast<To, From> (from.get()); }

}

#endif

