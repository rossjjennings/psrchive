#ifndef __TextInterface_h
#define __TextInterface_h

#include "ReferenceAble.h"
#include "tostring.h"

namespace TextInterface {

  template<class Class>
  class Attribute {

  public:
    
    //! Get the name of the attribute
    virtual std::string get_name () const = 0;

    //! Return true if the name argument matches
    virtual bool matches (const std::string& name) const { return true; }

    //! Get the value of the attribute
    virtual std::string get_value (const Class* ptr) const = 0;

    //! Set the value of the attribute
    virtual void set_value (Class* ptr, const std::string& value) = 0;

  };

  template<class Class, class Type, class Set, class Get>
  class AttributeSetGet : public Attribute<Class> {

  public:

    //! Constructor
    AttributeSetGet (const std::string& _name, Set _set, Get _get)
      { name = _name; set_method = _set; get_method = _get; }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the value of the attribute
    std::string get_value (const Class* ptr) const
      { return tostring( (ptr->*get_method) () ); }

    //! Set the value of the attribute
    void set_value (Class* ptr, const std::string& value)
      { (ptr->*set_method) (fromstring<Type>(value)); }

  protected:

    //! The name of the attribute
    std::string name;

    //! The set method
    Set set_method;

    //! The get method
    Get get_method;

  };

  template<class Class, class Type>
  class Allocator {

  public:
    template<class Set, class Get>
    Attribute<Class>* new_Attribute (const std::string& n, Set s, Get g)
    { return new AttributeSetGet<Class,Type,Set,Get> (n, s, g); }

  };

}


#endif
