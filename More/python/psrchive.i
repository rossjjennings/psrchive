%module psrchive
%{
#include "Pulsar/Archive.h"
%}

// Language independent exception handler
%include exception.i       
%include std_string.i

using namespace std;

%exception {
    try {
        $action
    } catch(Error& error) {
        SWIG_exception(SWIG_RuntimeError,error.get_message().c_str());
    } catch(...) {
        SWIG_exception(SWIG_RuntimeError,"Unknown exception");
    }
}

// help memory management
%newobject Pulsar::Archive::new_Archive (const string&);
%newobject Pulsar::Archive::load (const string&);
%newobject Pulsar::Archive::clone () const;
%newobject Pulsar::Archive::extract (const vector<unsigned>& subints) const;
%newobject Pulsar::Archive::total () const;

// does not handle nested classes
%ignore Pulsar::Archive::get_extension(unsigned);
%ignore Pulsar::Archive::get_extension(unsigned) const;
%ignore Pulsar::Archive::add_extension(Extension*);

// does not use the assignment operator
%ignore Pulsar::Archive::operator=(const Archive&);

// does not distinguish between const and non-const overloaded methods
%ignore Pulsar::Archive::get_Profile(unsigned,unsigned,unsigned) const;

// Parse the header file to generate wrappers
%include "Pulsar/Archive.h"

