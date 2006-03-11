#include "Pulsar/PlotAxis.h"
#include "Pulsar/PlotLabel.h"
#include "iopair.h"

Pulsar::PlotAxis::PlotAxis ()
{
  label = PlotLabel::unset;
  pgbox_opt = "BCNST";
  alternate = false;
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

