
#include "rhythm.h"

#ifdef Rhythm2D
#include "qxmp_manager.h"
#include "CurvePlotter2D.h"
#endif

#ifdef Rhythm3D
#include "qgl_PlotManager.h"
#include "CurvePlotter3D.h"
#endif

void Rhythm::initializePlot ()
{
#ifdef Rhythm2D

  if (vverbose)
    cerr << "Rhythm::initializePlot new qxmp_manager" << endl;
  qxmp_manager* manager2 = new qxmp_manager (this, 800, 700);

  if (vverbose)
    cerr << "Rhythm::initializePlot new Plot2D::CurvePlotter" << endl;
  Plot2D::CurvePlotter* plot2;
  plot2 = new Plot2D::CurvePlotter ( Cartesian (0.05,0.09),
				     Cartesian (0.98,0.98) );

  manager2->manage (*plot2);

  if (vverbose)
    cerr << "Rhythm::initializePlot push back managers" << endl;
  plot_manager.push_back ( manager2 );
  data_manager.push_back ( plot2 );

#endif

#ifdef Rhythm3D

  if (vverbose)
    cerr << "Rhythm::initializePlot new qgl_Manager" << endl;
  Plot3D::qgl_Manager* manager3 = new qgl_Manager (this);
  manager3 -> resize ( 550, 350 );

  if (vverbose)
    cerr << "Rhythm::initializePlot new Plot3D::CurvePlotter" << endl;
  Plot3D::CurvePlotter* plot3;
  plot3 = new Plot3D::CurvePlotter ( Cartesian (-1.0,-1.0,-1.0),
				     Cartesian (1.0,1.0,1.0) );

  manager3 -> manage ( plot3 );

  if (vverbose)
    cerr << "Rhythm::initializePlot push back managers" << endl;
  plot_manager.push_back ( manager3 );
  data_manager.push_back ( plot3 );

#endif

  if (plot_manager.size() != data_manager.size()) {
    string error ("Rhythm::initializePlot unequal manager/data pairs");
    cerr << error;
    throw error;
  }

  if (plot_manager.size() == 0)
    cerr << "Rhythm::initializePlot no plotting capabilities compiled" << endl;
}

