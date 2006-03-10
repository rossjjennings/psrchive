#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"
#include "iopair.h"

Pulsar::PlotAxis::PlotAxis () :
  range_norm (0.0, 1.0)
{
  label = PlotLabel::unset;
  pgbox_opt = "BCNST";
  alternate = false;
  buf_norm = 0.0;
}

void Pulsar::PlotAxis::get_range (float& min, float& max) const
{
  stretch (range_norm, min, max);

  float space = (max - min) * buf_norm;
  min -= space;
  max += space;
}

//! Add to the options to be passed to pgbox for this axis
void Pulsar::PlotAxis::add_pgbox_opt (char opt)
{
  std::string::size_type found = pgbox_opt.find(opt);
  if (found == std::string::npos)
    pgbox_opt += opt;
}

//! Remove from the options to be passed to pgbox for this axis
void Pulsar::PlotAxis::rem_pgbox_opt (char opt)
{
  std::string::size_type found = pgbox_opt.find(opt);
  if (found != std::string::npos)
    pgbox_opt.erase (found, 1);
}

