#include "qapplication.h"

#include "PulsarGUI.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QString filename;
  if (app.argc() > 1) {
    filename = app.argv()[1];
  }

  Pulsar::PulsarGUI* pg = new Pulsar::PulsarGUI(&app, filename);
  app.setMainWidget(pg);
  pg -> show();
  
  return app.exec();

}
