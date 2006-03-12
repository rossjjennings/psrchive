#include "PulsarGUI.h"

Pulsar::PulsarGUI::PulsarGUI(QApplication* qa, QString& filename) 
  : QMainWindow(0, 0, WDestructiveClose)
{
  myApp = qa;

  // First, define all user Actions
  QAction* fileOpenAction = new QAction("Open File", "&Open", CTRL+Key_O, 
					this, "Open");

  connect(fileOpenAction, SIGNAL(activated()), this, SLOT(fileOpen()));

  QAction* fileQuitAction = new QAction("Quit", "&Quit", CTRL+Key_Q, 
					this, "Quit");

  connect(fileQuitAction, SIGNAL(activated()), this, SLOT(fileQuit()));

  QAction* confFrameAction = new QAction("Frame", "&Frame", CTRL+Key_F, 
					this, "Frame");

  connect(confFrameAction, SIGNAL(activated()), this, SLOT(confFrame()));

  QAction* confGraphAction = new QAction("Plot", "&Plot", CTRL+Key_P, 
					this, "Plot");

  connect(confGraphAction, SIGNAL(activated()), this, SLOT(confGraph()));

  // Look after the placement of Widgets
  QVBox* layout = new QVBox(this);
  setCentralWidget(layout);

  // Construct a radio button group to hold the plot options
  psrButtons = new QButtonGroup(1, Qt::Horizontal, 
				"Pulsar Plot Methods", layout);
  psrButtons->setRadioButtonExclusive(true);

  // Define some useful main menus
  fileMenu = new QPopupMenu(this);
  menuBar()->insertItem("&File", fileMenu);

  confMenu = new QPopupMenu(this);
  menuBar()->insertItem("&Configure", confMenu);

  // Define a useful shortcuts toolbar
  psrTools = new QToolBar("Pulsar Tools", this);
  QWhatsThis::whatsThisButton(psrTools);

  // Connect the Actions to the toolbar and menus
  fileOpenAction->addTo(psrTools);
  fileOpenAction->addTo(fileMenu);
  fileQuitAction->addTo(fileMenu);

  confFrameAction->addTo(confMenu);
  confGraphAction->addTo(confMenu);

  
  // Construct available plot styles
  static PlotFactory factory;

  plotItem* Button1 = new plotItem(psrButtons, factory.construct("flux"),
				   "Flux Profile", 
				   "Plots the total intensity amplitudes "
				   "as a function of pulse phase.");
  
  plotItem* Button2 = new plotItem(psrButtons, factory.construct("stokes"),
				   "Stokes Profile", 
				   "Plots the total intensity, total linear "
				   "and total circular pulse profiles as a "
				   "function of phase, on the same axes.");

  plotItem* Button3 = new plotItem(psrButtons, factory.construct("freq"),
				   "Frequency vs Phase", 
				   "Plots an image of intensity in each "
				   "frequency band as a function of pulse "
				   "phase.");

  plotItem* Button4 = new plotItem(psrButtons, factory.construct("time"),
				   "Time vs Phase", 
				   "Plots an image of intensity in each "
				   "sub-integration as a function of "
				   "pulse phase.");
  psrButtons->adjustSize();

  // Allow the user to open a pgplot window and draw the active plot
  QPushButton* launcher = new QPushButton("Draw Plot", layout);
  QObject::connect(launcher, SIGNAL(clicked()), this, SLOT(plotGraph()));

  // Load a file if a name was given on the command line
  if (filename.isEmpty()) {
    arch = 0;
  }
  else {
    readFile(filename);
  }
}

void Pulsar::PulsarGUI::fileOpen()
{
  QString filename = QFileDialog::getOpenFileName("./", "*.*", this);
  if (!filename.isEmpty()) {
    readFile(filename);
  }
}

void Pulsar::PulsarGUI::readFile(QString& filename)
{
  try {
    arch = Pulsar::Archive::load(filename);
  }
  catch (Error& error) {
    QErrorMessage* em = new QErrorMessage(this);
    QString useful = "Failed to load Archive: ";
    useful += (error.get_message()).c_str();
    em->message(useful);
  }
}

void Pulsar::PulsarGUI::fileQuit()
{
  myApp->exit();
}

void Pulsar::PulsarGUI::confFrame()
{
  plotItem* pi = 0;
  TextInterface::Class* ui = 0;

  pi = dynamic_cast<plotItem*> (psrButtons->selected());

  if (pi) {
    ui = pi->getPlot()->get_frame_interface();
  }
  
  if (ui) {
    interfacePanel* panl = new interfacePanel(this, ui);
    panl->show();
  }
}

void Pulsar::PulsarGUI::confGraph()
{
  plotItem* pi = 0;
  TextInterface::Class* ui = 0;

  pi = dynamic_cast<plotItem*> (psrButtons->selected());

  if (pi) {
    ui = pi->getPlot()->get_interface();
  }

  if (ui) {
    interfacePanel* panl = new interfacePanel(this, ui);
    panl->show();
  }
}

void Pulsar::PulsarGUI::plotGraph()
{
  plotItem* pi = 0;
  pi = dynamic_cast<plotItem*> (psrButtons->selected());

  if (pi) {
    cpgopen("/xs");
    cpgsvp(0.1,0.9,0.1,0.9);
    pi->getPlot()->plot(arch);
    cpgclos();
  }
}
