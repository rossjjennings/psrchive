/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MoreProfiles.h"

    //! Construct with a name
Pulsar::MoreProfiles::MoreProfiles (const char* name)
  : DataExtension (name)
{
}

//! Turns Class::Method() into AdaptableUnaryFunction(Class*)
template<typename ClassPtr, typename Method>
class GeneratorMethod : public std::unary_function<ClassPtr, void>
{
public:

  GeneratorMethod (Method m)
  { method = m; }

  void operator () (ClassPtr ptr) const
  { (ptr->*(this->method))(); }

protected:

  //! Pointer to Method of Class
  Method method;
};

//! Turns Class::Method(Arg) into AdaptableBinaryFunction(Class*,Arg)
template<typename ClassPtr, typename Method, typename Arg>
class UnaryMethod : public std::binary_function<ClassPtr, Arg, void>
{
public:

  UnaryMethod (Method m)
  { method = m; }

  void operator () (ClassPtr ptr, const Arg& arg) const
  { (ptr->*(this->method))(arg); }

protected:

  //! Pointer to Method of Class
  Method method;
};

//! For each element of container C, call method M
template<typename C, typename M>
void foreach (C& container, M method)
{
  for_each (container.begin(), container.end(),
	    GeneratorMethod<typename C::value_type, M>(method));
}

//! For each element of container C, call method M with argument A
template<typename C, typename M, typename A>
void foreach (C& container, M method, const A& a)
{
  UnaryMethod<typename C::value_type, M, A> binary (method);

  for_each (container.begin(), container.end(), std::bind2nd(binary,a));
}

//! Resize the data area
void Pulsar::MoreProfiles::resize (unsigned nbin)
{
  foreach (profile, &Profile::resize, nbin);
}

//! multiplies each bin of the profile by scale
void Pulsar::MoreProfiles::scale (double scale)
{
  foreach (profile, &Profile::scale, scale);
}

//! offsets each bin of the profile by offset
void Pulsar::MoreProfiles::offset (double offset)
{
  foreach (profile, &Profile::offset, offset);
}

//! rotates the profile by phase (in turns)
void Pulsar::MoreProfiles::rotate_phase (double phase)
{
  foreach (profile, &Profile::rotate_phase, phase);
}

//! set all amplitudes to zero
void Pulsar::MoreProfiles::zero ()
{
  foreach (profile, &Profile::zero);
}

//! integrate neighbouring phase bins in profile
void Pulsar::MoreProfiles::bscrunch (unsigned nscrunch)
{
  foreach (profile, &Profile::bscrunch, nscrunch);
}

//! integrate neighbouring phase bins in profile
void Pulsar::MoreProfiles::bscrunch_to_nbin (unsigned nbin)
{
  foreach (profile, &Profile::bscrunch_to_nbin, nbin);
}

//! integrate neighbouring sections of the profile
void Pulsar::MoreProfiles::fold (unsigned nfold)
{
  foreach (profile, &Profile::fold, nfold);
}
