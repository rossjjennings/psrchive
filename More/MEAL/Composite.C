#include "MEPL/Composite.h"
#include "MEPL/Constant.h"
#include "Error.h"

//! Default constructor
Model::Composite::Composite ()
{
  nparameters = 0;
  current_model = 0;
  current_index = 0;
}

//! Return the number of parameters
unsigned Model::Composite::get_nparam () const
{
  return nparameters;
}

//! Return the name of the specified parameter
string Model::Composite::get_param_name (unsigned index) const
{
  return get_Function(index)->get_param_name (index);
}

//! Return the value of the specified parameter
double Model::Composite::get_param (unsigned index) const
{
  return get_Function(index)->get_param (index);
}

//! Set the value of the specified parameter
void Model::Composite::set_param (unsigned index, double value)
{
  get_Function(index)->set_param (index, value);
}

//! Return the variance of the specified parameter
double Model::Composite::get_variance (unsigned index) const
{
  return get_Function(index)->get_variance (index);
}

//! Set the variance of the specified parameter
void Model::Composite::set_variance (unsigned index, double variance)
{
  get_Function(index)->set_variance (index, variance);
}

//! Return true if parameter at index is to be fitted
bool Model::Composite::get_infit (unsigned index) const
{
  return get_Function(index)->get_infit (index);
}

//! Set flag for parameter at index to be fitted
void Model::Composite::set_infit (unsigned index, bool flag)
{
  get_Function(index)->set_infit (index, flag);
}


//! The most common abscissa type needs a simple interface
void Model::Composite::set_argument (unsigned dimension, Argument* axis)
{
  for (unsigned imodel=0; imodel < models.size(); imodel++)  {
    reference_check (imodel, "set_axis");
    models[imodel] -> set_argument (dimension, axis);
  }
}

// ///////////////////////////////////////////////////////////////////
//
// Add and access parameters
//
// ///////////////////////////////////////////////////////////////////

string Model::Composite::class_name() const
{
  return "Model::Composite[" + get_name() + "]::";
}

//! Get the number of Models
unsigned Model::Composite::get_nmodel () const
{
  return models.size ();
}


void Model::Composite::map (Projection* modelmap, bool signal_changes)
{
#ifdef _DEBUG
  cerr << class_name() + "map (Projection* = " << modelmap << ")" << endl;
#endif

  if (!modelmap)
    throw Error (InvalidState, class_name() + "map", "null Projection");

  bool already_mapped = false;
  
  if (modelmap->meta) {

    if (modelmap->meta != this)
      throw Error (InvalidState, class_name() + "map",
                   "Projection already mapped into another Composite");

    unsigned imap = find_Projection (modelmap);
    if (imap < maps.size())
      already_mapped = true;
    else
      throw Error (InvalidState, class_name() + "map",
		   "Projection partially mapped into this");
      
  }
  
  try {

    Function* model = modelmap->get_Function();
    modelmap->imap.resize(0);
    add_component (model, modelmap->imap);

    if (modelmap->imap.size() != model->get_nparam())
      throw Error (InvalidState, class_name() + "map",
		   "map size=%d != nparam=%d",
		   modelmap->imap.size(), model->get_nparam());

    if (very_verbose) {
      cerr << class_name() + "map Model maps into" << endl;
      for (unsigned i=0; i<modelmap->imap.size(); i++)
	cerr << "   " << i << ":" << modelmap->imap[i] << endl;
    }

    model->changed.connect (this, &Composite::attribute_changed);

    if (!already_mapped) {
      if (very_verbose) cerr << class_name() + "map new Projection" << endl;
      maps.push_back (modelmap);
      modelmap->meta = this;
    }
    
    if (very_verbose) 
      cerr << class_name() + "map send changed ParameterCount" << endl;

    changed.send (ParameterCount);

    if (very_verbose) 
      cerr << class_name() + "map set_evaluation_changed" << endl;

    set_evaluation_changed ();

  }
  catch (Error& error) {
    throw error += class_name() + "map";
  }

}



//! Map the Model indeces
void Model::Composite::add_component (Function* model,
					    vector<unsigned>& imap)
{
  if (!model)
    return;

  if (very_verbose)
    cerr << class_name() + "add_component [" <<model->get_name()<< "]" << endl;

  Constant* constant = dynamic_cast<Constant*>(model);
  if (constant) {
    if (very_verbose)
      cerr << class_name() + "add_component Constant" << endl;
    return;
  }

  Composite* meta = dynamic_cast<Composite*>(model);

  if (meta) {

    if (meta == this)
      throw Error (InvalidState, class_name() + "add_component",
		   "cannot map Composite into self");

    if (very_verbose)
      cerr << class_name() + "add_component Composite" << endl;

    unsigned nmodel = meta->get_nmodel();
    for (unsigned imodel=0; imodel<nmodel; imodel++)
      add_component (meta->models[imodel], imap);

  }
  else {
    
    if (very_verbose)
      cerr << class_name() + "add_component nmodel=" << models.size() << endl;

    unsigned iparam = 0;
    unsigned imodel = 0;

    for(; imodel < models.size(); imodel++)  {
      
      reference_check (imodel, "add_component");
      
      if (models[imodel].ptr() == model)
        break;
      
      iparam += models[imodel]->get_nparam();

    }

    unsigned nparam = model->get_nparam();

    // add the mapped indeces (works for both cases: new model or old model)
    for (unsigned jparam=0; jparam < nparam; jparam++)
      imap.push_back(iparam + jparam);

    // the model exists in the current list
    if (imodel < models.size())
      return;

    if (very_verbose)
      cerr << class_name() + "add_component add new Model" << endl;

    // add the new model
    nparameters += model->get_nparam();
    models.push_back (model);

  }

}

void Model::Composite::unmap (Projection* modelmap, bool signal_changes)
{
  if (!(modelmap->meta))
    throw Error (InvalidParam, class_name() + "unmap",
		 "Undefined projection");
  
  if ((modelmap->meta).get() != this)
    throw Error (InvalidParam, class_name() + "unmap",
		 "Projection not mapped into this");
  
  // erase the mapping
  unsigned imap = find_Projection (modelmap);
  if (imap == maps.size()) {
    if (very_verbose)
      cerr << class_name() + "unmap Projection not found" << endl;
    return;
  }
  maps.erase (maps.begin()+imap);

  // remove the component
  Function* model = modelmap->get_Function();
  remove_component (model);

  // disconnect the callback
  model->changed.disconnect (this, &Composite::attribute_changed);

  // flag the mapping as unmanaged
  modelmap->meta = 0;

  // remap the remaining mappings
  remap (signal_changes);
}


//! Remove a Model from the list
void Model::Composite::remove_component (Function* model)
{
  if (very_verbose)
    cerr << class_name() + "remove_component" << endl;

  Constant* constant = dynamic_cast<Constant*>(model);
  if (constant) {
    if (very_verbose)
      cerr << class_name() + "remove_component no need to remove Constant"
	   << endl;
    return;
  }

  Composite* meta = dynamic_cast<Composite*>(model);

  if (meta) {

    if (very_verbose)
      cerr << class_name() + "remove_component remove Composite" << endl;

    unsigned nmodel = meta->get_nmodel();
    for (unsigned imodel=0; imodel<nmodel; imodel++)
      remove_component (meta->models[imodel]);

  }
  else {
    
    unsigned imodel = find_Function(model);

    if (very_verbose)
      cerr << class_name() + "remove_component imodel=" << imodel << endl;

    if (imodel >= models.size())
      throw Error (InvalidRange, class_name() + "remove_component",
		   "imodel=%d > nmodel=%d", imodel, models.size());
  
    reference_check (imodel, "remove_component");

    nparameters -= model->get_nparam();
    models.erase (models.begin()+imodel);
  
  }

}

//! Recount the number of parameters
void Model::Composite::remap (bool signal_changes)
{ 
  if (very_verbose)
    cerr << class_name() << "remap remap Projection instances" << endl;

  try {
    for (unsigned imap=0; imap < maps.size(); imap++)
      map (maps[imap], false);
  }
  catch (Error& error) {
    throw error += class_name() + "remap";
  }

  if (very_verbose)
    cerr << class_name() << "remap recount parameters" << endl;

  nparameters = 0;
  for (unsigned imodel=0; imodel < models.size(); imodel++)  {
    reference_check (imodel, "remap");
    nparameters += models[imodel]->get_nparam();
  }

  if (very_verbose)
    cerr << class_name() << "remap send changed ParameterCount" << endl;
  
  changed.send (ParameterCount);
  
  if (very_verbose)
    cerr << class_name() << "remap set_evaluation_changed" << endl;
  
  set_evaluation_changed ();

  if (very_verbose)
    cerr << class_name() << "remap exit" << endl;
}

void Model::Composite::attribute_changed (Attribute attribute) 
{
  if (very_verbose)
    cerr << class_name() << "attribute_changed" << endl;

  if (attribute == ParameterCount) {
    if (very_verbose)
      cerr << class_name() << "attribute_changed remap" << endl;
    remap ();
  }

  if (attribute == Evaluation) {
    if (very_verbose)
      cerr << class_name() << "attribute_changed set_evaluation_changed"
	   << endl;
    set_evaluation_changed ();
  } 

}

//! Get the const Model that corresponds to the given index
const Model::Function*
Model::Composite::get_Function (unsigned& index) const
{
  return const_cast<Composite*>(this)->get_Function (index);
}

//! Get the Model that corresponds to the given index
Model::Function* Model::Composite::get_Function (unsigned& index)
{
  unsigned imodel = current_model;
  
  if (index < current_index) {
    current_index = 0;
    imodel = 0;
  }
  else
    index -= current_index;
  
  while (imodel < models.size())  {
    
    reference_check (imodel, "get_Function");

    unsigned nparam = models[imodel]->get_nparam();
    
    if (index < nparam) {
      current_model = imodel;
      return models[imodel];
    }
    
    index -= nparam;
    current_index += nparam;
    imodel ++;
  }

  throw Error (InvalidRange, class_name() + "get_Function",
	       "index=%d > nparam=%d", index, get_nparam());
}


//! Check the the reference to the specified model is still valid
void Model::Composite::reference_check (unsigned i, char* method) const
{
  if (!models[i])
    throw Error (InvalidState, class_name() + method, 
		 "model[%d] is an invalid reference", i);
}


//! Return the index for the specified model
unsigned Model::Composite::find_Function (Function* model) const
{
  if (very_verbose) cerr << class_name() + "find_Function nmodel="
			 << models.size() << endl;

  for (unsigned imodel=0; imodel < models.size(); imodel++)  {
    reference_check (imodel, "find_Function");
    if ((models[imodel])) {
      if ((models[imodel]).get() == model)
	return imodel;
    }
  }
  return models.size();
}

//! Return the index for the specified map
unsigned Model::Composite::find_Projection (Projection* modelmap) const
{
  if (very_verbose) cerr << class_name() + "find_Projection nmap="
			 << maps.size() << endl;

  for (unsigned imap=0; imap < maps.size(); imap++) {
    if ((maps[imap]))
      if ((maps[imap]).get() == modelmap)
	return imap;
  }
  
  return maps.size();
}
