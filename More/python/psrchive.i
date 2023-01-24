%module psrchive

%rename(Archive_Extension) Pulsar::Archive::Extension;
%rename(Integration_Extension) Pulsar::Integration::Extension;

%{
#define SWIG_FILE_WITH_INIT
#include "numpy/noprefix.h"

#include "Reference.h"
#include "Pulsar/IntegrationManager.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/ProfileAmps.h"
#include "Pulsar/Profile.h"

#include "Pulsar/Pointing.h"
#include "Pulsar/ITRFExtension.h"
#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/TextParameters.h"
#include "load_factory.h"

#include "Pulsar/Dispersion.h"
#include "Pulsar/IntegrationBarycentre.h"

#include "Pulsar/Interpreter.h"

#include "Pulsar/ShiftEstimator.h"
#include "Pulsar/ArrivalTime.h"

#include "Pulsar/ProfileShiftFit.h"
#include "Pulsar/Append.h"
#include "Pulsar/TimeAppend.h"
#include "Pulsar/FrequencyAppend.h"
#include "Pulsar/PatchTime.h"
#include "Pulsar/PatchFrequency.h"
#include "Pulsar/Contemporaneity.h"
#include "Pulsar/Predictor.h"
#include "polyco.h"
#include "toa.h"

#include "Pulsar/ManualPolnCalibrator.h"

#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/BackendCorrection.h"
#include "Pulsar/FrontendCorrection.h"

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/PeakCumulative.h"
#include "Pulsar/PeakConsecutive.h"

#include "Pulsar/ArchiveStatistic.h"

#if HAVE_CFITSIO
#include <fitsio.h>
#endif

#if HAVE_GSL
#include "Pulsar/WaveletSmooth.h"
#endif

// For some reason SWIG is not picking up the namespace for the emitted
// code, hence this kluge allowing an unqualified reference to Phase
using Pulsar::Phase;
using Pulsar::Predictor;

%}

#ifdef HAVE_CONFIG_H
%include <config.h>
#endif
 
// Language independent exception handler
%include exception.i       
%include std_string.i

%include std_vector.i
namespace std {
  %template(StringVector) vector<string>;
}

using namespace std;

%exception {
    try {
        $action
    } catch(Error& error) {
        // Deal with out-of-range errors
        if (error.get_code()==InvalidRange)
            SWIG_exception(SWIG_IndexError, error.get_message().c_str());
        else
            SWIG_exception(SWIG_RuntimeError,error.get_message().c_str());
    } catch(...) {
        SWIG_exception(SWIG_RuntimeError,"Unknown exception");
    }
}

%init %{
  import_array();
%}

// Declare functions that return a newly created object
// (Helps memory management)
%newobject Pulsar::Archive::new_Archive;
%newobject Pulsar::Archive::load;
%newobject Pulsar::Archive::clone;
%newobject Pulsar::Archive::extract;
%newobject Pulsar::Archive::total;
%newobject Pulsar::Integration::clone;
%newobject Pulsar::Integration::total;
%newobject Pulsar::Profile::clone;
%newobject Pulsar::Predictor::clone;

// Track any pointers handed off to python with a global list
// of Reference::To objects.  Prevents the C++ routines from
// prematurely destroying objects by effectively making python
// variables act like Reference::To pointers.
%feature("ref")   Reference::Able "pointer_tracker_add($this);"
%feature("unref") Reference::Able "pointer_tracker_remove($this);"
%header %{
std::vector< Reference::To<Reference::Able> > _pointer_tracker;
void pointer_tracker_add(Reference::Able *ptr) {
    _pointer_tracker.push_back(ptr);
}
void pointer_tracker_remove(Reference::Able *ptr) {
    std::vector< Reference::To<Reference::Able> >::iterator it;
    for (it=_pointer_tracker.begin(); it<_pointer_tracker.end(); it++) 
        if ((*it).ptr() == ptr) {
            _pointer_tracker.erase(it);
            break;
        }
}
%}

%ignore Pulsar::FrontendCorrection::new_Extension() const;

// These return Jones<double> instances which swig 4.x currently
// seems to have issues wrapping.  If needed from python a better
// way would be to typemap Jones<double> to 2-by-2 numpy arrays
// (or some similar approach).
// https://sourceforge.net/p/psrchive/bugs/471/
%ignore Pulsar::FrontendCorrection::get_transformation(unsigned);
%ignore Pulsar::FrontendCorrection::get_basis();
%ignore Pulsar::FrontendCorrection::get_projection(unsigned);

// Also does not use the assignment operator
%ignore Pulsar::Archive::operator=(const Archive&);
%ignore Pulsar::Integration::operator=(const Integration&);
%ignore Pulsar::Profile::operator=(const Profile&);
%ignore Pulsar::IntegrationManager::operator=(const IntegrationManager&);

// Also does not distinguish between const and non-const overloaded methods
%ignore Pulsar::Archive::get_Profile(unsigned,unsigned,unsigned) const;
%ignore Pulsar::Archive::expert() const;
%ignore Pulsar::Integration::get_Profile(unsigned,unsigned) const;
%ignore Pulsar::Integration::new_PolnProfile(unsigned) const;
%ignore Pulsar::IntegrationManager::get_Integration(unsigned) const;
%ignore Pulsar::IntegrationManager::get_last_Integration() const;
%ignore Pulsar::IntegrationManager::get_first_Integration() const;
%ignore Pulsar::ProfileAmps::get_amps() const;

// Stokes class not wrapped yet, so ignore it
%ignore Pulsar::Integration::get_Stokes(unsigned,unsigned) const;

// Same for Option
%ignore Pulsar::Profile::rotate_in_phase_domain;
%ignore Pulsar::Profile::transition_duty_cycle;
%ignore Pulsar::Profile::default_duty_cycle;
%ignore Pulsar::FrequencyAppend::weight_strategy;
%ignore Pulsar::FrequencyAppend::force_new_predictor;

// Also Functor
%ignore Pulsar::Profile::peak_edges_strategy;
%ignore Pulsar::Profile::baseline_strategy;
%ignore Pulsar::Profile::onpulse_strategy;
%ignore Pulsar::Profile::snr_strategy;

// Also Contemporaneity
%ignore Pulsar::PatchTime::set_contemporaneity_policy(Contemporaneity*);

// This conflicted with std_vector for some reason
%ignore Pulsar::ManualPolnCalibrator::match;

// Return psrchive's Estimate class as a Python tuple
%typemap(out) Estimate<double> {
    PyTupleObject *res = (PyTupleObject *)PyTuple_New(2);
    PyTuple_SetItem((PyObject *)res, 0, PyFloat_FromDouble($1.get_value()));
    PyTuple_SetItem((PyObject *)res, 1, PyFloat_FromDouble($1.get_variance()));
    $result = (PyObject *)res;
}
%typemap(out) Estimate<float> = Estimate<double>;

// Convert various enums to/from string
%define %map_enum(TYPE)
%typemap(out) Signal:: ## TYPE {
    $result = PyString_FromString( TYPE ## 2string($1).c_str());
}
%typemap(in) Signal:: ## TYPE %{
    try {
        // String handling changed between python 2 and 3.
        // This approach should be OK for 2.7 and 3.3+
#if PY_VERSION_HEX >= 0x03030000
        const char *typestr = PyUnicode_AsUTF8($input);
#else
        const char *typestr = PyString_AsString($input);
#endif
        if (typestr!=NULL) {
            $1 = Signal::string2 ## TYPE (typestr);
        }
    } catch (Error &error) {
        SWIG_exception(SWIG_RuntimeError,error.get_message().c_str());
    } 
%}
%enddef
%map_enum(State)
%map_enum(Basis)
%map_enum(Scale)
%map_enum(Source)

// return long doubles as  numpy scalars
// implementation left in case wanted later
//%typemap(out) (long double) {
//  npy_intp size[1];
//  size[0] = 1;
//  PyArrayObject *rval;
//  rval = (PyArrayObject *)PyArray_SimpleNew(0, size, NPY_LONGDOUBLE);
//  *((long double *)(rval->data)) = $1;
//  $result = (PyObject *) rval;
//}

// return long doubles as Python floats
%typemap(out) (long double) {
  $result = PyFloat_FromDouble(double($1));
}

// read Python floats as long doubles when necessary
%typemap(in) (long double) {
  $1 = (long double)(PyFloat_AsDouble($input));
}

// For some reason SWIG thinks polyco is abstract even though it's not.
// This forces it to reconsider.
%feature("notabstract") polyco;

// Header files included here will be wrapped
%include "ReferenceAble.h"
%include "Pulsar/Container.h"
%include "Pulsar/IntegrationManager.h"
%include "Pulsar/Archive.h"
%include "Pulsar/Integration.h"
%include "Pulsar/ProfileAmps.h"
%include "Pulsar/Profile.h"
%include "Pulsar/Parameters.h"
%include "Pulsar/TextParameters.h"
%include "Pulsar/ArrivalTime.h"
%include "Pulsar/ProfileShiftFit.h"

%include "Pulsar/BackendCorrection.h"
%include "Pulsar/FrontendCorrection.h"

#if HAVE_GSL
%include "Pulsar/WaveletSmooth.h"
#endif

%include "Pulsar/Append.h"
%include "Pulsar/TimeAppend.h"
%include "Pulsar/FrequencyAppend.h"
%include "Pulsar/PatchTime.h"
%include "Pulsar/PatchFrequency.h"
%include "Pulsar/PeakCumulative.h"
%include "Pulsar/PeakConsecutive.h"
%include "Pulsar/ITRFExtension.h"
%include "Pulsar/ManualPolnCalibrator.h"
%include "Angle.h"
%include "sky_coord.h"
%include "MJD.h"
%include "Phase.h"
%include "Pulsar/Predictor.h"
%include "polyco.h"

// Some useful free functions 

#if HAVE_CFITSIO
%inline %{

// Least I/O intensive way to grab observation time
double get_tobs(const char* filename) {
    int status=0,colnum=0;
    long numrows=0;
    double tobs=0;
    fitsfile* fp;
    fits_open_file(&fp, filename, READONLY, &status);
    fits_movnam_hdu(fp, BINARY_TBL, "SUBINT", 0, &status);
    fits_get_colnum (fp, CASEINSEN, "TSUBINT", &colnum, &status);
    fits_get_num_rows(fp, &numrows, &status);
    double tsubs[numrows];
    fits_read_col(fp, TDOUBLE, colnum, 1, 1, numrows, NULL, tsubs, NULL, &status);
    if (status == 0)
      while (numrows >= 0)
        tobs += tsubs[--numrows];
    fits_close_file(fp, &status);
    return tobs;
}
%}
#endif

// Python-specific extensions to the classes:

%extend MJD
{
    // SWIG doesn't like the way the operators are currently defined
    // in the MJD class.  We could change MJD definition but will
    // try this for now.
    MJD operator + (const MJD & right) { return operator + (*self,right); }
    MJD operator - (const MJD & right) { return operator - (*self,right); }
    MJD operator + (double right) { return operator + (*self,right); }
    MJD operator - (double right) { return operator - (*self,right); }

    std::string __str__()
    {
        return "PSRCHIVE MJD: " + self->printall();
    }

}

%extend Pulsar::PatchTime
{
  void set_contemporaneity_policy(std::string policy_name)
  {

    if (policy_name=="time") 
    {
      self->set_contemporaneity_policy(new Pulsar::Contemporaneity::AtEarth);
    }

    if (policy_name=="phase")
    {
      self->set_contemporaneity_policy(new Pulsar::Contemporaneity::AtPulsar);
    }

  }
}

%extend Pulsar::Phase
{
    // see MJD extension
    Pulsar::Phase operator + (const Pulsar::Phase & right) { return operator + (*self,right); }
    Pulsar::Phase operator - (const Pulsar::Phase & right) { return operator - (*self,right); }
    Pulsar::Phase operator + (double right) { return operator + (*self,right); }
    Pulsar::Phase operator - (double right) { return operator - (*self,right); }

  long intturns() {
    return self->intturns();
  }

}

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
        arr = (PyArrayObject *) \
              PyArray_SimpleNewFromData(1, &n, PyArray_FLOAT, (char *)ptr);
        if (arr == NULL) return NULL;
        PyArray_INCREF(arr);
        return (PyObject *)arr;
    }
}

%extend Pulsar::Integration
{
    // Interface to Pointing
    double get_telescope_zenith() {
        Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
        if (p==NULL) return 0.0;
        return p->get_telescope_zenith().getDegrees();
    }

    double get_telescope_azimuth() {
        Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
        if (p==NULL) return 0.0;
        return p->get_telescope_azimuth().getDegrees();
    }

    double get_parallactic_angle() {
        Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
        if (p==NULL) return 0.0;
        p->update(self);
        return p->get_parallactic_angle().getDegrees();
    }

   double get_position_angle() {
       Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
       if (p==NULL) return 0.0;
       p->update(self);
       return p->get_position_angle().getDegrees();
    }

    // Return Galactic latitude and longitude. Pulsar::Pointing is
    // inherited by Pulsar::Integration, hence we cannot call it
    // while in archive object.
    double get_galactic_latitude() {
        Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
        if (p==NULL) return 0.0;
        return p->get_galactic_latitude().getDegrees();
    }

    double get_galactic_longitude() {
        Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
        if (p==NULL) return 0.0;
        return p->get_galactic_longitude().getDegrees();
    }

    // Return LST in decimal hours.
    double get_local_sidereal_time() {
        Pulsar::Pointing *p = self->get<Pulsar::Pointing>();
        if (p==NULL) return 0.0;
        return p->get_local_sidereal_time() / 3600.0;
    }

    void set_verbose() {
        self->verbose = 1;
    }

    // Interface to Barycentre
    double get_doppler_factor() {
        Pulsar::IntegrationBarycentre bary;
        bary.set_Integration(self);
        return bary.get_Doppler();
    }

    // rotate is protected.. kinda annoying
    void rotate_time(double time) {
        self->expert()->rotate(time);
    }

    void _rotate_phase_swig(double phase) {
        self->expert()->rotate_phase(phase);
    }

    %pythoncode %{
def rotate_phase(self,phase): return self._rotate_phase_swig(phase)
%}

    void combine(Pulsar::Integration* subint) {
      self->expert()->combine(subint);
    }

    // Return baseline_stats as numpy arrays
    PyObject *baseline_stats() {

        // Call C++ routine
        std::vector< std::vector< Estimate<double> > > mean;
        std::vector< std::vector<double> > var;
        self->baseline_stats(&mean, &var);
        npy_intp size[2]; // Chan and pol
        size[0] = mean.size();
        size[1] = mean[0].size();

        // Pack values into new numpy arrays
        PyArrayObject *npy_mean, *npy_var;
        npy_mean = (PyArrayObject *)PyArray_SimpleNew(2, size, PyArray_DOUBLE);
        npy_var  = (PyArrayObject *)PyArray_SimpleNew(2, size, PyArray_DOUBLE);
        for (int ii=0; ii<size[0]; ii++) 
            for (int jj=0; jj<size[1]; jj++) {
                ((double *)npy_mean->data)[ii*size[1]+jj] = mean[ii][jj].get_value();
                ((double *)npy_var->data)[ii*size[1]+jj] = var[ii][jj];
            }

        // Pack arrays into tuple for output
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(2);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)npy_mean);
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)npy_var);
        return (PyObject *)result;
    }

    // Return cal levels as numpy arrays
    PyObject *cal_levels() {

        // Call C++ routine for values
        std::vector< std::vector< Estimate<double> > > hi, lo;
        self->cal_levels(hi, lo);
        npy_intp dims[2]; // Chan and pol
        dims[0] = hi.size();
        dims[1] = hi[0].size();

        // Create, fill numpy arrays
        PyArrayObject *hi_arr, *lo_arr, *sig_hi_arr, *sig_lo_arr;
        hi_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims, PyArray_DOUBLE);
        lo_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims, PyArray_DOUBLE);
        sig_hi_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims,
            PyArray_DOUBLE);
        sig_lo_arr = (PyArrayObject *)PyArray_SimpleNew(2, dims,
            PyArray_DOUBLE);
        for (int ii=0; ii<dims[0]; ii++) {
            for (int jj=0; jj<dims[1]; jj++) {
                ((double *)hi_arr->data)[ii*dims[1]+jj] =
                    hi[ii][jj].get_value();
                ((double *)lo_arr->data)[ii*dims[1]+jj] =
                    lo[ii][jj].get_value();
                ((double *)sig_hi_arr->data)[ii*dims[1]+jj] =
                    sqrt(hi[ii][jj].get_variance());
                ((double *)sig_lo_arr->data)[ii*dims[1]+jj] =
                    sqrt(lo[ii][jj].get_variance());
            }
        }

        // Pack arrays into tuple
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(4);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)hi_arr);
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)lo_arr);
        PyTuple_SetItem((PyObject *)result, 2, (PyObject *)sig_hi_arr);
        PyTuple_SetItem((PyObject *)result, 3, (PyObject *)sig_lo_arr);
        return (PyObject *)result;
    }

    // Return frequency table of the integration as numpy array
    PyObject *get_frequencies()
    {
        int ii;
        PyArrayObject *arr;
        npy_intp ndim[1];
        ndim[0] = self->get_nchan();
        arr = (PyArrayObject *)PyArray_SimpleNew(1, ndim, PyArray_DOUBLE);
        for (ii = 0; ii < ndim[0]; ii++) {
            ((double *)arr->data)[ii] = self->get_Profile(0, ii)->get_centre_frequency();
        }
        return (PyObject *)arr;
    }

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

    // Get recvr name
    std::string get_receiver_name()
    {
        Pulsar::Receiver *r = self->get<Pulsar::Receiver>();
        if (r==NULL) { return "none"; }
        return r->get_name();
    }

    // Get backend name
    std::string get_backend_name()
    {
        Pulsar::Backend *r = self->get<Pulsar::Backend>();
        if (r==NULL) { return "none"; }
        return r->get_name();
    }

    // Get backend delay
    double get_backend_delay()
    {
        Pulsar::Backend *b = self->get<Pulsar::Backend>();
        if (b==NULL) { return 0.0; }
        return b->get_delay();
    }

    // Return telescope ITRF position as tuple.
    // If ITRF coordinates are not present in the data then the position
    // will be returned as "undefined".
    PyObject *get_ant_xyz() {
    double itrf_x, itrf_y, itrf_z;
    Pulsar::ITRFExtension *p = self->get<Pulsar::ITRFExtension>();
    if (p==NULL) {
        PyObject *result = (PyObject *)PyString_FromString("undefined");
        return (PyObject *)result;
    } else {
        itrf_x = p->get_ant_x();
        itrf_y = p->get_ant_y();
        itrf_z = p->get_ant_z();
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(3);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)PyFloat_FromDouble(itrf_x));
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)PyFloat_FromDouble(itrf_y));
        PyTuple_SetItem((PyObject *)result, 2, (PyObject *)PyFloat_FromDouble(itrf_z));
        return (PyObject *)result;
    }
}

    // Allow timing model to be updated via eph filename
    void set_ephemeris(std::string eph_file)
    {
        Pulsar::Parameters *new_eph;
        new_eph = factory<Pulsar::Parameters> (eph_file);
        self->set_ephemeris(new_eph);
        // TODO: update DM...
    }

    void dededisperse()
    {
        Pulsar::Dispersion correction;
        correction.revert(self);
    }

    // Interface with the internal command interpreter
    std::string execute(std::string command)
    {
        static Pulsar::Interpreter *psrsh = NULL;
        if (!psrsh) psrsh = standard_shell();
        psrsh->set(self);
        return psrsh->parse(command);
    }

    // Return frequency table of the archive as numpy array
    PyObject *get_frequencies()
    {
        int ii;
        PyArrayObject *arr;
        npy_intp ndim[1];

        ndim[0] = self->get_nchan();
        arr = (PyArrayObject *)PyArray_SimpleNew(1, ndim, PyArray_DOUBLE);
        for (ii = 0; ii < ndim[0]; ii++) {
            ((double *)arr->data)[ii] = self->get_Profile(0, 0, ii)->get_centre_frequency();
        }
        return (PyObject *)arr;
    }

    // Return frequency table of the archive as 2-D numpy array
    PyObject *get_frequency_table()
    {
        int ii, jj;
        PyArrayObject *arr;
        npy_intp ndims[2];  // nsubint, nchan

        ndims[0] = self->get_nsubint();
        ndims[1] = self->get_nchan();
        arr = (PyArrayObject *)PyArray_SimpleNew(2, ndims, PyArray_DOUBLE);
        for (ii = 0; ii < ndims[0]; ii++) {
            for (jj = 0; jj < ndims[1]; jj++) {
                ((double *)arr->data)[ii*ndims[1]+jj] = \
                    self->get_Profile(ii, 0, jj)->get_centre_frequency();
            }
        }
        return (PyObject *)arr;
    }

    // Return mjd table of the archive as numpy array
    PyObject *get_mjds()
    {
        int ii;
        PyArrayObject *arr;
        npy_intp ndim[1];

        ndim[0] = self->get_nsubint();
        arr = (PyArrayObject *)PyArray_SimpleNew(1, ndim, PyArray_DOUBLE);
        for (ii = 0; ii < ndim[0]; ii++) {
            ((double *)arr->data)[ii] = self->get_Integration(ii)->get_epoch().in_days();
        }
        return (PyObject *)arr;
    }

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

    // Return statistic as a numpy array
    PyObject *get_statistic(std::string statname)
    {
        PyArrayObject *arr;
        npy_intp ndims[3];  // nsubint, npol, nchan
        int ii, jj, kk;

        Reference::To<Pulsar::ArchiveStatistic> stat = Pulsar::ArchiveStatistic::factory(statname);
        stat->set_Archive(self);

        ndims[0] = self->get_nsubint();
        ndims[1] = self->get_npol();
        ndims[2] = self->get_nchan();
        arr = (PyArrayObject *)PyArray_SimpleNew(3, ndims, PyArray_FLOAT);
        for (ii = 0 ; ii < ndims[0] ; ii++)
            for (jj = 0 ; jj < ndims[1] ; jj++)
                for (kk = 0 ; kk < ndims[2] ; kk++)
                {
                    stat->set_subint(ii);
                    stat->set_pol(jj);
                    stat->set_chan(kk);
              
                    ((float *)arr->data)[ii*ndims[1]*ndims[2]+jj*ndims[2]+kk] = \
                        stat->get();
                }
        return (PyObject *)arr;
    }
    // Return a copy of the predictor
    Pulsar::Predictor* get_predictor() {
      return self->get_model()->clone();
    }
}

%extend Pulsar::PeakCumulative
{
    PyObject *get_indeces() {

        // Call C++ routine for values
        int hi, lo;
        self->get_indeces(hi, lo);

        // Pack arrays into tuple
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(2);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)PyInt_FromLong((long)hi));
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)PyInt_FromLong((long)lo));
        return (PyObject *)result;
    }
}

%extend Pulsar::PeakConsecutive
{
    PyObject *get_indeces() {

        // Call C++ routine for values
        int hi, lo;
        self->get_indeces(hi, lo);

        // Pack arrays into tuple
        PyTupleObject *result = (PyTupleObject *)PyTuple_New(2);
        PyTuple_SetItem((PyObject *)result, 0, (PyObject *)PyInt_FromLong((long)hi));
        PyTuple_SetItem((PyObject *)result, 1, (PyObject *)PyInt_FromLong((long)lo));
        return (PyObject *)result;
    }
}

%extend Pulsar::ArrivalTime
{
    // Allow use of the 'pat -A' strings directly
    void set_shift_estimator(std::string type) {
        self->set_shift_estimator(Pulsar::ShiftEstimator::factory(type));
    }

    // Allow use of 'pat -e' type options
    void shift_estimator_config(std::string config) {
        Reference::To<TextInterface::Parser> parser;
        parser = self->get_shift_estimator()->get_interface();
        parser->process(config);
    }

    // returns TOAs as a tuple of strings in python
    std::vector<std::string> get_toas() {
        std::vector<Tempo::toa> toas;
        self->get_toas(toas);
        std::vector<std::string> result;
        for (int i=0; i<toas.size(); i++) {
            char toatmp[2048];
            toas[i].unload(toatmp);
            result.push_back(toatmp);
        }
        return result;
    }

    // get phase shift from toa matching (pat -R like)
    PyObject *get_phase_shifts() {
        PyArrayObject *arr;
        std::vector<Tempo::toa> toas;
        self->get_toas(toas);
        npy_intp size[1];
        size[0] = toas.size();

        arr = (PyArrayObject *)PyArray_SimpleNew(1, size, PyArray_DOUBLE);
        for (int i=0; i<toas.size(); i++) {
            ((double*)arr->data)[i] = toas[i].get_phase_shift();
        }
        return (PyObject*) arr;
    }


}
