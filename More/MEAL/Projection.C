#include "Calibration/Projection.h"
#include "Calibration/Composite.h"

//! Default constructor
Calibration::Projection::Projection (Model* _model, Composite* _meta)
{
  model = _model;

  if (_model && _meta)
    _meta->map (this);
}

//! Return the mapped index
unsigned Calibration::Projection::get_imap (unsigned index) const
{
  if (!model)
    throw Error (InvalidRange, "Calibration::Projection::get_imap",
		 "no Model");

  if (model->get_nparam() != imap.size())
    throw Error (InvalidRange, "Calibration::Projection::get_imap",
                 "Model::nparam=%d >= nmap=%d",
		 model->get_nparam(), imap.size());

  if (index >= imap.size())
    throw Error (InvalidRange, "Calibration::Projection::get_imap",
		 "index=%d >= nmap=%d", index, imap.size());

  return imap[index];
}

Calibration::Model* Calibration::Projection::get_Model()
{
  return model;
}

void Calibration::Projection::set_Model (Model* _model)
{
  if (model.ptr() == _model)
    return;

  model = _model;

  imap.resize (0);
  meta = 0;
}
