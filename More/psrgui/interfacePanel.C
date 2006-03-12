#include "interfacePanel.h"

Pulsar::interfacePanel::interfacePanel(QWidget* parent, 
				       TextInterface::Class* ui)
  : QDialog(parent)
{
  ti = ui;

  QVBoxLayout* layout = new QVBoxLayout(this);

  QPushButton* updateMe = new QPushButton("&Update", this);
  layout->addWidget(updateMe);
  QObject::connect(updateMe, SIGNAL(clicked()), this, SLOT(processRequest()));
  updateMe->setDefault(true);

  QString useful1, useful2;
  for (unsigned i = 0; i < ui->get_nattribute(); i++) {
    useful1 = ti->get_name(i);
    useful2 = ti->get_value(useful1.ascii());
    
    parameters.push_back(new QLineEdit(useful1 + " = " + useful2, this));
    layout->addWidget(parameters[i]);
  }
  
  QPushButton* closeMe = new QPushButton("Close", this);
  layout->addWidget(closeMe);
  QObject::connect(closeMe, SIGNAL(clicked()), this, SLOT(accept()));
  closeMe->setDefault(false);

  setSizeGripEnabled(true);
  //setMinimumSize(childrenRect().width(), childrenRect().height());
}

void Pulsar::interfacePanel::processRequest()
{
  for (unsigned i = 0; i < parameters.size(); i++) {
    if (parameters[i]->isModified()) {
      try {
	ti->process((parameters[i]->text().remove(' ')).ascii());
      }
      catch (Error& error) {
	QErrorMessage* em = new QErrorMessage(this);
	QString useful = "Invalid Request: ";
	useful += (error.get_message()).c_str();
	em->message(useful);
      }
    }
  }
}
