//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/NonlinearlySpaced.h

#ifndef __Pulsar_NonlinearlySpaced_h
#define __Pulsar_NonlinearlySpaced_h

#include "Pulsar/Divided.h"
#include "debug.h"

#include <cmath>

namespace Pulsar {

  template<class C>
  class Integrate<C>::NonlinearlySpaced : public Integrate<C>::Divided
  {

    //! Initialize ranges for the specified parameters
    void initialize (Integrate*, C*);

    //! Return the number of index ranges into which the container is divided
    unsigned get_nrange () { return stop_indeces.size(); }

    void get_range (unsigned irange, unsigned& start, unsigned& stop);

    //! Get the value assigned to the element
    virtual double get_value (const C*, unsigned ielement) = 0;

  protected:

    //! The input frequency channel index at the end of each range
    std::vector<unsigned> stop_indeces;

  };

}

template<class C>
void Pulsar::Integrate<C>::NonlinearlySpaced::initialize (Integrate*, 
							  C* container)
{
  unsigned container_size = this->get_size( container );

  // divide them up
  unsigned output_elements = 0;
  unsigned spacing = 0;
  this->divide (container_size, output_elements, spacing);

  DEBUG("NonlinearlySpaced::initialize container_size=" << container_size << " output_elements=" << output_elements);
  
  if (output_elements == 0)
    return;
  
  stop_indeces.resize (output_elements);
  stop_indeces[output_elements-1] = container_size;

  if (output_elements == 1)
    return;
  
  double value_0 = get_value (container, 0);
  double value_range = get_value (container, container_size - 1) - value_0;
  double value_step = value_range / output_elements;

  DEBUG("NonlinearlySpaced::initialize value_0=" << value_0 << " value_range=" << value_range);
      
  unsigned ielement = 0;
  
  for (unsigned irange=0; irange+1 < output_elements; irange++)
  {
    double stop_value = value_0 + (irange+1) * value_step;

    DEBUG("NonlinearlySpaced::initialize stop_value[" << irange << "]=" << stop_value);

    bool first = true;
    double min_diff = 0;
    
    while (ielement < container_size)
    {
      double value = get_value (container, ielement);
      double diff = std::fabs( value - stop_value );

      DEBUG("NonlinearlySpaced::initialize value[" << ielement << "]=" << value << " diff=" << diff);

      if (first)
	min_diff = diff;
      else if (diff > min_diff)
	break;
      else
	min_diff = diff;

      ielement ++;
      first = false;
    }

    DEBUG("NonlinearlySpaced::initialize stop_index[" << irange << "]=" << ielement);

    stop_indeces[irange] = ielement;
  }
}

template<class C>
void Pulsar::Integrate<C>::NonlinearlySpaced::get_range (unsigned irange, 
							 unsigned& start, 
							 unsigned& stop)
{
  if (irange >= stop_indeces.size())
    throw Error (InvalidParam, 
		 "Pulsar::Integrate::Divide::NonlinearlySpaced::get_range",
		 "irange=%u >= output_elements=%u", irange, stop_indeces.size());

  if (irange > 0)
    start = stop_indeces[irange-1];
  else
    start = 0;

  stop = stop_indeces[irange];
}



#endif
