#include <math.h>
#include <cpgplot.h>

#include "CurvePlotter2D3.h"

void Plot2D::CurvePlotter3::where (float& x, float& y, const Cartesian& pt)
{
  x = pt * vx_axis;
  y = pt * vy_axis;
}

void Plot2D::CurvePlotter3::poly (const vector<Cartesian>& pts)
{
  if (pts.size() == 0)
    return;

  float* x = new float [pts.size()];
  float* y = new float [pts.size()];

  for (unsigned ipt=0; ipt<pts.size(); ipt++) {
    x[ipt] = float(pts[ipt] * vx_axis);
    y[ipt] = float(pts[ipt] * vy_axis);
    cerr << ipt << " " << pts[ipt] 
	 << " x=" << x[ipt] << " y=" << y[ipt] << endl;
  }
  // x[0]=y[0]=0;

  if (Plot2D::Volume::verbose)
    cerr << "Plot2D::CurvePlotter3::poly call cpgpoly npt=" 
	 << pts.size() << endl;

  cpgpoly (pts.size(), x, y);

  delete [] x;
  delete [] y;
}

// primitives
void Plot2D::CurvePlotter3::move (const Cartesian& pt)
{
  float x = pt * vx_axis;
  float y = pt * vy_axis;
  cpgmove (x, y);
}

void Plot2D::CurvePlotter3::draw (const Cartesian& pt)
{
  float x = pt * vx_axis;
  float y = pt * vy_axis;
  cpgdraw (x, y);
}

void Plot2D::CurvePlotter3::plot (const Cartesian& pt, int symbol)
{
  if (Plot2D::Volume::verbose)
    cerr << "Plot2D::CurvePlotter3::plot " << pt << endl;

  float x = pt * vx_axis;
  float y = pt * vy_axis;

  if (Plot2D::Volume::verbose)
    cerr << "Plot2D::CurvePlotter3::plot x=" << x << " y=" << y << endl;

  cpgpt1 (x, y, symbol);
}

void Plot2D::CurvePlotter3::text (const Cartesian& pt, const char* text)
{
  if (Plot2D::Volume::verbose)
    cerr << "Plot2D::CurvePlotter3::text " << pt << endl;

  float x = pt * vx_axis;
  float y = pt * vy_axis;
  cpgtext (x, y, text);

}

void Plot2D::CurvePlotter3::arrow (const Cartesian& from, const Cartesian& to)
{
  float fx = from * vx_axis;
  float fy = from * vy_axis;
  float tx = to * vx_axis;
  float ty = to * vy_axis;

  cpgarro (fx, fy, tx, ty);
}

// set camera position - phi and theta in degrees
void Plot2D::CurvePlotter3::set_camera (double theta, double phi)
{
  vx_axis = Cartesian (0, 1, 0);
  vy_axis = Cartesian (0, 0, 1);

  Angle t, p;
  t.setDegrees(theta);
  p.setDegrees(-phi);

  vy_axis.y_rot (p);
  vx_axis.z_rot (t); vy_axis.z_rot (t);
}

void Plot2D::CurvePlotter3::init()
{
  vx_axis = Cartesian (0, 1, 0);
  vy_axis = Cartesian (0, 0, 1);
}

void Plot2D::CurvePlotter3::rangeUpdate ()
{ 
  if (range_valid)
    set_world (range_min, range_max);
  else
    world_set = false;
  Redraw();
}

void Plot2D::CurvePlotter3::painter ()
{
  if (DataManager::verbose) {
     cerr << "Plot2D::CurvePlotter::painter with " << data_set.size()
        << " DataSets." << endl;
     // PlotVolume2D::painter();
  }

  cpgsci (1);

  if (!world_set) {
    cpgbox ("bc",0.0,0,"bc",0.0,0);
    return;
  }

  for (unsigned iplot=0; iplot < data_set.size(); iplot++)
    drawPlot (data_set[iplot]);
}

void Plot2D::CurvePlotter3::drawPlot (DataSet* plot)
{
  if (!plot)
    return;

  unsigned npts = plot->Size();
  
  if (Plot2D::Volume::verbose || DataManager::verbose)
    cerr << "Plot2D::CurvePlotter::drawPlot " << npts << " points" << endl;
  
  if (Plot2D::Volume::verbose || DataManager::verbose) {
    if (plot->getPlotType() == DataSet::Points)
      cerr << "Plot2D::CurvePlotter::drawPlot single points" << endl;
    else if (plot->getPlotType() == DataSet::Line)
      cerr << "Plot2D::CurvePlotter::drawPlot join points" << endl;
    else
      cerr << "Plot2D::CurvePlotter::drawPlot"
	" unsupported DataSet::PlotType" << endl;
  }

  bool lines = plot->getPlotType() != DataSet::Points;
  bool first = true;

  Cartesian error;

  bool xerror = plot->has_xerror();
  bool yerror = plot->has_yerror();
  bool plot_error = (xerror || yerror);

  bool plot_symbols = plot->has_symbol();
  bool plot_labels = plot->has_pt_label();

  float x, y;

  string xlab (" ");
  string ylab (" ");
  string title (" ");

  if (plot->has_xlabel())
    xlab = plot->xlabel();
  if (plot->has_ylabel())
    ylab = plot->ylabel();
  if (plot->has_title())
    title = plot->title();

  cpgsci (1);
  cpglab (xlab.c_str(), ylab.c_str(), title.c_str());

  for (unsigned ipt=0; ipt < npts; ipt++)  {

    const DataPoint& point = (*plot)[ipt];

    x = point.x;
    y = point.y;

    if (lines) {
      if (first)
	move (point);
      else
	draw (point);
      first = false;
      continue;
    }

    if (plot_error) {
      error = plot->Error (ipt);
      move (point - error);
      draw (point + error);
    }
    
    //if (plot_symbols)
    // cpgpt (1, &x, &y, plot->symbol(ipt));
    //else
    this->plot (point, 17);
  }
}


