//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextInterfaceAdapter.h

#ifndef __TextInterfaceAdapter_h
#define __TextInterfaceAdapter_h

namespace TextInterface
{
  //! Base class of Value adapters 
  /*! To avoid the diamond problem and multiple inheritance, 
      the inherited class is a template argument */
  template<class IsA, class HasA = IsA>
  class AdapterBase : public IsA
  {
  public:
    
    AdapterBase (HasA* _value = 0) { value = _value; }

    //! Get the name of the value
    std::string get_name () const
      { return value->get_name(); }

    //! Get the description of the value
    std::string get_description () const
      { return value->get_description(); }

    //! Set the detailed description of the value
    std::string get_detailed_description () const
      { return value->get_detailed_description (); }

    bool matches (const std::string& name) const
      { return value->matches (name); }

    void set_parent (Parser* p)
      { IsA::set_parent(p); value->set_parent(p); }

    void set_modifiers (const std::string& mods) const
      { value->set_modifiers (mods); }

    void reset_modifiers () const
      { value->reset_modifiers(); }

  protected:

    //! The nested value
    Reference::To<HasA> value;

  };


  template<class IsA, class HasA = IsA>
  class ValueAdapter : public AdapterBase<IsA,HasA>
  {
  public:
    
    ValueAdapter (HasA* _value = 0) : AdapterBase<IsA,HasA> (_value) { }

    //! Get the value of the value
    std::string get_value () const 
      { return this->value->get_value (); }

    //! Set the value of the value
    void set_value (const std::string& val)
      { this->value->set_value (val); }
  };

  //! Can be used as an Adapter (IsA != HasA) or a Decorator (IsA == HasA)
  template<class IsA, class HasA = IsA>
  class AttributeAdapter 
    : public AdapterBase< Attribute<IsA>, Attribute<HasA> >
  {
  public:
    
    //! Set the description of the attribute
    void set_description (const std::string& description)
      { this->value->set_description (description); }

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description)
      { this->value->set_detailed_description (description); }
  };

}

#endif
