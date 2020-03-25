/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "RobustEstimateZapper.h"

RobustEstimateZapper::RobustEstimateZapper ()
{ 
  // seek outliers based on the base 10 logarithm of the error bar
  error = true;
  logarithmic = true;

  /* do not scale threshold by MADM
     when combined with logarithmic, this makes threshold an upper limit
     on the order of magnitude difference from the median */
  scale = false;

  // allow error bars up to 10x greater than the median error bar
  threshold_max = 1.0;

  // smaller error bars are ok
  threshold_min = 0.0;

}


//! Text interface to a RobustEstimateZapper instance
class RobustEstimateZapper::Interface
 : public TextInterface::To<RobustEstimateZapper>
{
  public:
    Interface (RobustEstimateZapper* = NULL);
};

TextInterface::Parser* RobustEstimateZapper::get_interface()
{
  return new Interface (this);
}

RobustEstimateZapper::Interface::Interface (RobustEstimateZapper* a)
{
  if (a)
    set_instance (a);

  add( &RobustEstimateZapper::get_cutoff_threshold,
       &RobustEstimateZapper::set_cutoff_threshold,
       "cutoff", "Outlier threshold (maximum offset from median)" );

  add( &RobustEstimateZapper::get_cutoff_threshold_min,
       &RobustEstimateZapper::set_cutoff_threshold_min,
       "cutmin", "Minimum extreme threshold (lower offset from median)" );

  add( &RobustEstimateZapper::get_cutoff_threshold_max,
       &RobustEstimateZapper::set_cutoff_threshold_max,
       "cutmax", "Maximum extreme threshold (upper offset from median)" );

  add( &RobustEstimateZapper::get_scale,
       &RobustEstimateZapper::set_scale,
       "scale", "Scale threshold by MADM" );

  add( &RobustEstimateZapper::get_error_bar,
       &RobustEstimateZapper::set_error_bar,
       "error", "Flag outliers based on error bar" );

  add( &RobustEstimateZapper::get_log,
       &RobustEstimateZapper::set_log,
       "log", "Take base 10 logarithm of data" );
}

