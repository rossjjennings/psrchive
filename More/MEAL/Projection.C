#include "MEPL/Projection.h"
#include "MEPL/Composite.h"

//! Default constructor
Model::Projection::Projection (Function* _model, Composite* _meta)
{
  model = _model;

  if (_model && _meta)
    _meta->map (this);
}

//! Return the mapped index
unsigned Model::Projection::get_imap (unsigned index) const
{
  if (!model)
    throw Error (InvalidRange, "Model::Projection::get_imap",
		 "no Model");

  if (model->get_nparam() != imap.size())
    throw Error (InvalidRange, "Model::Projection::get_imap",
                 "Model::nparam=%d >= nmap=%d",
		 model->get_nparam(), imap.size());

  if (index >= imap.size())
    throw Error (InvalidRange, "Model::Projection::get_imap",
		 "index=%d >= nmap=%d", index, imap.size());

  return imap[index];
}

Model::Function* Model::Projection::get_Function ()
{
  return model;
}

void Model::Projection::set_Function (Function* _model)
{
  if (model.ptr() == _model)
    return;

  model = _model;

  imap.resize (0);
  meta = 0;
}
