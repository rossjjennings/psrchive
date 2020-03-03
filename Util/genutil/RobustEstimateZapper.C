/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "RobustEstimateZapper.h"

RobustEstimateZapper::RobustEstimateZapper ()
{ 
  // by default, seek outliers based on the logarithm of the error bar
  error = true;
  logarithmic =true;

  // WvS: this default was determined by experimentation on one file ...
  // ... it's unexpectedly large ...
  threshold = 20.0;
}


//! Text interface to a RobustEstimateZapper instance
class RobustEstimateZapper::Interface : public TextInterface::To<RobustEstimateZapper>
{
  public:
    Interface (RobustEstimateZapper* = NULL);
};

TextInterface::Parser* RobustEstimateZapper::get_interface() { return new Interface (this); }

RobustEstimateZapper::Interface::Interface (RobustEstimateZapper* a)
{
  if (a)
    set_instance (a);

  add( &RobustEstimateZapper::get_threshold,
       &RobustEstimateZapper::set_threshold,
       "threshold", "Outlier threshold (multiple of MADM)" );

  add( &RobustEstimateZapper::get_error_bar,
       &RobustEstimateZapper::set_error_bar,
       "error", "Flag outliers based on error bar" );

  add( &RobustEstimateZapper::get_log,
       &RobustEstimateZapper::set_log,
       "log", "Compute MADM of natural logarithm" );

}

