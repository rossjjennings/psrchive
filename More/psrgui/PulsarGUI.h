#include "qapplication.h"
#include "qmainwindow.h"
#include "qwhatsthis.h"
#include "qradiobutton.h"
#include "qbuttongroup.h"
#include "qerrormessage.h"
#include "qfiledialog.h"
#include "qpopupmenu.h"
#include "qvbox.h"
#include "qmenubar.h"
#include "qtoolbar.h"
#include "qaction.h"

#include "Pulsar/Plot.h"
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Archive.h"

#include "plotItem.h"
#include "interfacePanel.h"
#include "Error.h"

#include "cpgplot.h"

#include <iostream>

namespace Pulsar {
  
  class PulsarGUI : public QMainWindow
    {
      Q_OBJECT

      public:

        // Basic constructor
        PulsarGUI(QApplication* qa, QString& filename);

	void readFile(QString& filename);
      
      protected slots:

	void fileOpen();
        void fileQuit();

	void confFrame();
	void confGraph();

	void plotGraph();

      protected:

	Reference::To<Pulsar::Archive> arch;

	QButtonGroup* psrButtons;

        QPopupMenu* fileMenu;
	QPopupMenu* confMenu;
  
	QToolBar* psrTools;

	QApplication* myApp;

      private:
  
    };
}
