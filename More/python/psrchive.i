%module psrchive
%{
#define SWIG_FILE_WITH_INIT
#include "numpy/noprefix.h"

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

%init %{
  import_array();
%}

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

%extend Pulsar::Profile
{
    // Allow indexing
    float __getitem__(int i)
    {
        return self->get_amps()[i];
    }
    void __setitem__(int i, float val)
    {
        self->get_amps()[i] = val;
    }
    int __len__()
    {
        return self->get_nbin();
    }

    // Return a numpy array view of the data.
    // This points to the actual Profile data, not a separate copy.
    PyObject *get_amps()
    {
        PyArrayObject *arr;
        float *ptr;
        npy_intp n;
        
        n = self->get_nbin();
        ptr = self->get_amps();
        arr = (PyArrayObject *)                                         \
            PyArray_SimpleNewFromData(1, &n, PyArray_FLOAT, (char *)ptr);
        if (arr == NULL) return NULL;
        arr->flags |= OWN_DATA;
        PyArray_INCREF(arr);
        return (PyObject *)arr;
    }
}

%extend Pulsar::Integration
{
    // Return MJD as double
    // TODO: probably can do this better with typemap?
    double get_epoch() { return self->get_epoch().in_days(); }
}

%extend Pulsar::Archive
{

    // Allow indexing of Archive objects
    Pulsar::Integration *__getitem__(int i)
    {
        return self->get_Integration(i);
    }
    int __len__()
    {
        return self->get_nsubint();
    }

    // String representation of the Archive
    std::string __str__()
    {
        return "PSRCHIVE Archive object: " + self->get_filename();
    }

    // String representation of various enums
    std::string get_type() { return Source2string(self->get_type()); }
    std::string get_state() { return State2string(self->get_state()); }
    std::string get_basis() { return Basis2string(self->get_basis()); }
    std::string get_scale() { return Scale2string(self->get_scale()); }

    // Return a copy of all the data as a numpy array
    PyObject *get_data()
    {
        PyArrayObject *arr;
        npy_intp ndims[4];  // nsubint, npol, nchan, nbin
        int ii, jj, kk;
        
        ndims[0] = self->get_nsubint();
        ndims[1] = self->get_npol();
        ndims[2] = self->get_nchan();
        ndims[3] = self->get_nbin();
        arr = (PyArrayObject *)PyArray_SimpleNew(4, ndims, PyArray_FLOAT);
        for (ii = 0 ; ii < ndims[0] ; ii++)
            for (jj = 0 ; jj < ndims[1] ; jj++)
                for (kk = 0 ; kk < ndims[2] ; kk++)
                    memcpy(arr->data + sizeof(float) * 
                           (ndims[3] * (kk + ndims[2] * (jj + ndims[1] * ii))), 
                           self->get_Profile(ii, jj, kk)->get_amps(),
                           ndims[3]*sizeof(float));
        return (PyObject *)arr;
    }

    // Return a copy of all the weights as a numpy array
    PyObject *get_weights()
    {
        PyArrayObject *arr;
        npy_intp ndims[2];  // nsubint, nchan
        int ii, jj;
        
        ndims[0] = self->get_nsubint();
        ndims[1] = self->get_nchan();
        arr = (PyArrayObject *)PyArray_SimpleNew(2, ndims, PyArray_FLOAT);
        for (ii = 0 ; ii < ndims[0] ; ii++)
            for (jj = 0 ; jj < ndims[1] ; jj++)
                ((float *)arr->data)[ii*ndims[1]+jj] = \
                    self->get_Integration(ii)->get_weight(jj);
        return (PyObject *)arr;
    }
}
