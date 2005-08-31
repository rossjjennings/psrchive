//-*-C++-*-

#ifndef __FTransform_h_
#define __FTransform_h_

#include <string>
#include <vector>

#include <assert.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Reference.h"
#include "psr_cpp.h"

/*!

All outputs of frc1d must have N+2 floats allocated for output

frc1d = forward real->complex 1D
fcc1d = forward complex->complex 1D
bcc1d = backward complex->complex 1D

Call transforms as FUNC(ndat,dest,src);

*/

namespace FTransform {

  typedef int (*fft_call)(unsigned, float*, float*);
  enum norm_type { normal, nfft };

  //! Pointers to the real functions- set by set_library()
  //! Arguments are: (unsigned ndat, float* dest, float* src)
  extern fft_call frc1d;
  extern fft_call fcc1d;
  extern fft_call bcc1d;
  extern fft_call bcr1d; // Data must be hermitian; ndat floats are outputted

  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_frc1d(unsigned ndat, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_fcc1d(unsigned ndat, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_bcc1d(unsigned ndat, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_bcr1d(unsigned ndat, float* srcdest);

  //! Returns currently selected library
  string get_library();
  
  //! Choose to use a different library
  void set_library(string _library);

  //! Returns currently selected normalization
  norm_type get_norm();

  //! Clears out the memory associated with the plans
  void clean_plans();

  //! Whether to optimize or not
  extern bool optimize;

  ////////////////////////////////////////////////////////////////
  //! Users don't need to worry about altering anything down here
  //! i.e. DON'T TOUCH
  //! You may however, wish to explicitly construct a Plan for custom use,
  //! In this case you'll need to use the initialising constructor like:
  //! bla = new FTransform::MKL_Plan(ndat,FTransform::get_ilib(),"frc1d");

  class Plan : public Reference::Able {
  public:
    Plan();
    Plan(unsigned _ndat, unsigned _ilib, string _fft_call);
    virtual ~Plan();
    virtual void init(unsigned _ndat, unsigned _ilib, string _fft_call) = 0;
    void initialise(unsigned _ndat, unsigned _ilib, string _fft_call);

    bool optimized;
    unsigned ndat;
    unsigned ilib;
    string fft_call;
  };

  unsigned get_ilib();
  unsigned get_ilib(string libstring);
  int initialise();

  extern int initialised;

  extern norm_type norm;
  extern string library;
  extern vector<string> valid_libraries;
  extern vector<fft_call> frc1d_calls;
  extern vector<fft_call> fcc1d_calls;
  extern vector<fft_call> bcc1d_calls;
  extern vector<fft_call> bcr1d_calls;
  extern vector<norm_type> norms;
  extern vector<vector<Reference::To<Plan> > > plans;
  extern Plan* last_frc1d_plan;
  extern Plan* last_fcc1d_plan;
  extern Plan* last_bcc1d_plan;
  extern Plan* last_bcr1d_plan;
}

#endif
