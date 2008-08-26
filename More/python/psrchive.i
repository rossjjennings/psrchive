%module psrchive
%{
#include "Pulsar/IntegrationManager.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProfileAmps.h"
#include "Pulsar/Profile.h"
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

%newobject Pulsar::Integration::clone () const;
%newobject Pulsar::Integration::total () const;

%newobject Pulsar::Profile::clone () const;

// does not handle nested classes
%ignore Pulsar::Archive::get_extension(unsigned);
%ignore Pulsar::Archive::get_extension(unsigned) const;
%ignore Pulsar::Archive::add_extension(Extension*);

%ignore Pulsar::Integration::get_extension(unsigned);
%ignore Pulsar::Integration::get_extension(unsigned) const;
%ignore Pulsar::Integration::add_extension(Extension*);

// does not use the assignment operator
%ignore Pulsar::Archive::operator=(const Archive&);
%ignore Pulsar::Integration::operator=(const Integration&);
%ignore Pulsar::Profile::operator=(const Profile&);
%ignore Pulsar::IntegrationManager::operator=(const IntegrationManager&);

// does not distinguish between const and non-const overloaded methods
%ignore Pulsar::Archive::get_Profile(unsigned,unsigned,unsigned) const;
%ignore Pulsar::Archive::expert() const;
%ignore Pulsar::Integration::get_Profile(unsigned,unsigned) const;
%ignore Pulsar::Integration::new_PolnProfile(unsigned) const;
%ignore Pulsar::IntegrationManager::get_Integration(unsigned) const;
%ignore Pulsar::IntegrationManager::get_last_Integration() const;
%ignore Pulsar::IntegrationManager::get_first_Integration() const;
%ignore Pulsar::ProfileAmps::get_amps() const;

// Ignore Stokes class for now
%ignore Pulsar::Integration::get_Stokes(unsigned,unsigned) const;

// Also ignore Option
%ignore Pulsar::Profile::rotate_in_phase_domain;
%ignore Pulsar::Profile::transition_duty_cycle;
%ignore Pulsar::Profile::default_duty_cycle;

// Parse the header file to generate wrappers
%include "Pulsar/IntegrationManager.h"
%include "Pulsar/Archive.h"
%include "Pulsar/Integration.h"
%include "Pulsar/ProfileAmps.h"
%include "Pulsar/Profile.h"

