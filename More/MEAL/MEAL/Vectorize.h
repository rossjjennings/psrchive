//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* psrchive/More/MEAL/MEAL/Vectorize.h */

#ifndef __Vectorize_H
#define __Vectorize_H

#include "MEAL/ScalarVector.h"
#include "MEAL/Convert.h"
#include "Error.h"

template<class Container, class Element = double> struct ScalarMapping;

namespace MEAL {

  //! Converts a higher dimensional function into a ScalarVector
  template<class T, class Map = ScalarMapping<typename T::Result> >
  class Vectorize : public Convert<T,ScalarVector>
  {
  public:

    typedef typename T::Result Result;

    //! Default contructor
    Vectorize (T* function);

    //! Return the name of the class
    std::string get_name () const
    { return "Vectorize<" + this->get_model()->get_name() + ">"; }

    //! Return the dimension of the vector
    unsigned size () const;

  private:

    //! Evaluate the function and decompose it
    void calculate (double& result, std::vector<double>* gradient);

    //! The order of scalar elements in T
    Map map;
  };

  template<class T>
  Vectorize<T>* vectorize (T* function) { return new Vectorize<T>(function); }
}


//! This recursive template completely unrolls multi-dimensional objects
template<class T, class Element> struct ScalarMapping
{
  typedef DatumTraits< typename DatumTraits<T>::element_type > SubTraits;

  static inline unsigned ndim ()
  {
    return DatumTraits<T>::ndim() * SubTraits::ndim();
  }

  static inline Element element (const T& t, unsigned idim)
  { 
    const unsigned index = idim / SubTraits::ndim();
    const unsigned sub_index = idim % SubTraits::ndim();
    return SubTraits::element( DatumTraits<T>::element(t, index), sub_index );
  }
};

//! Ends the recursion as long as the most nested type is double
template<>
struct ScalarMapping<double>
{
  static inline unsigned ndim () { return 1; }
  static inline double element (const double& x, unsigned idim) { return x; }
};

template<class T, class M>
MEAL::Vectorize<T,M>::Vectorize (T* function)
{
  this->set_model (function);
}

template<class T, class M>
void MEAL::Vectorize<T,M>::calculate (double& result,
				      std::vector<double>* gradient)
{
  unsigned index = this->get_index();

  if (index >= size())
    throw Error (InvalidState,
		 "MEAL::Vectorize<" + std::string(T::Name) + ">::calculate",
		 "index=%u >= size=%u", index, size());

  Result m_result;
  std::vector<Result> m_gradient;
  std::vector<Result>* m_gradptr = &m_gradient;
  if (!gradient)
    m_gradptr = 0;

  if (this->get_verbose())
    std::cerr << get_name() + "::calculate call evaluate" << std::endl;

  m_result = this->get_model()->evaluate (m_gradptr);

  //ScalarMapping<Result> map;

  if (this->get_verbose())
    std::cerr << get_name() + "::calculate index=" << this->get_index() << std::endl;

  result = map.element( m_result, index );

  if (!gradient)
    return;

  if (this->get_verbose())
    std::cerr << get_name() + "::calculate map gradient" << std::endl;

  gradient->resize( m_gradient.size() );
  for (unsigned i=0; i<m_gradient.size(); i++)
    (*gradient)[i] = map.element( m_gradient[i], index );

  if (this->get_verbose())
    std::cerr << get_name() + "::calculate return" << std::endl;
}

//! Return the dimension of the vector
template<class T, class M>
unsigned MEAL::Vectorize<T,M>::size () const
{
  return map.ndim(); // ScalarMapping<Result>::ndim();
}

#endif
