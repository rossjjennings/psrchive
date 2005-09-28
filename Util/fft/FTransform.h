//-*-C++-*-

#ifndef __FTransform_h_
#define __FTransform_h_

#include "Reference.h"

#include <string>
#include <vector>

/*!

All outputs of frc1d must have N+2 floats allocated for output

frc1d = forward real->complex 1D
fcc1d = forward complex->complex 1D
bcc1d = backward complex->complex 1D

fcc2d = forward complex->complex 2D
bcc2d = backward complex->complex 2D

Call transforms as FUNC(ndat,dest,src);

*/

namespace FTransform {

  //! Pointer to one-dimensional FFT
  typedef int (*fft_call)(unsigned, float*, const float*);

  enum norm_type { normal, nfft };

  //! Pointers to the real functions- set by set_library()
  //! Arguments are: (unsigned ndat, float* dest, const float* src)
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
  std::string get_library ();
  
  //! Choose to use a different library
  void set_library (const std::string& _library);

  //! Returns currently selected normalization
  norm_type get_norm ();

  //! Clears out the memory associated with the plans
  void clean_plans();

  //! Whether to optimize or not
  extern bool optimize;

  //! Virtual base class of one-dimensional Fast Fourier Transforms
  class Plan : public Reference::Able {
  public:

    Plan();

    Plan(unsigned _ndat, unsigned _ilib, const std::string& _fft_call);

    virtual ~Plan();

    virtual void init (unsigned _ndat, unsigned _ilib,
		       const std::string& _fft_call) = 0;

    void initialise (unsigned _ndat, unsigned _ilib,
		     const std::string& _fft_call);

    bool optimized;
    unsigned ndat;
    unsigned ilib;
    std::string fft_call;
  };

  unsigned get_ilib();
  unsigned get_ilib(const std::string& libstring);
  int initialise();

  extern int initialised;

  extern norm_type norm;
  extern std::string library;
  extern std::vector<std::string> valid_libraries;
  extern std::vector<fft_call> frc1d_calls;
  extern std::vector<fft_call> fcc1d_calls;
  extern std::vector<fft_call> bcc1d_calls;
  extern std::vector<fft_call> bcr1d_calls;
  extern std::vector<norm_type> norms;
  extern std::vector<std::vector<Reference::To<Plan> > > plans;
  extern Plan* last_frc1d_plan;
  extern Plan* last_fcc1d_plan;
  extern Plan* last_bcc1d_plan;
  extern Plan* last_bcr1d_plan;


  //! Pointer to two-dimensional FFT
  typedef void (*fft2_call)(unsigned, unsigned, float*, const float*);

  extern fft2_call fcc2d;
  extern fft2_call bcc2d;

  //! Base class of two-dimensional Fast Fourier Transforms
  class Plan2 : public Reference::Able {
  public:

    virtual ~Plan2 () { }

    bool optimized;
    std::string call;
    unsigned nx;
    unsigned ny;

  };

  extern Plan2* last_fcc2d_plan;
  extern Plan2* last_bcc2d_plan;

  //! Base class of two-dimensional FFT agents
  class Agent2 : public Reference::Able {

  public:

    //! Name of the transform library
    std::string name;

    //! Forward complex-to-complex two-dimensional FFT
    fft2_call fcc2d;

    //! Backward complex-to-complex two-dimensional FFT
    fft2_call bcc2d;

    //! The normalization type
    norm_type norm;

    //! List of all libraries
    static std::vector< Reference::To<Agent2> > libraries;
  };


  //! Template virtual base class of FFT library agents
  template <class PlanT>
  class PlanAgent2 : public Agent2 {

  public:

    //! Default constructor
    PlanAgent2 (const std::string& name, norm_type norm);

    //! Return an appropriate plan from this library
    PlanT* get_plan (unsigned nx, unsigned ny, const std::string& call);

  protected:

    //! The plans managed by the agent for this library
    std::vector< Reference::To<PlanT> > plans;

  private:

    //! Number of instances of this agent class
    static unsigned instances;

  };

  template<class PlanT> unsigned PlanAgent2<PlanT>::instances = 0;

  template<class PlanT>
  PlanAgent2<PlanT>::PlanAgent2 (const std::string& _name, norm_type _norm)
  {
    name = _name;
    norm = _norm;

    this->fcc2d = PlanT::fcc2d;
    this->bcc2d = PlanT::bcc2d;

    if (!FTransform::fcc2d)
      FTransform::fcc2d = PlanT::fcc2d;

    if (!FTransform::bcc2d)
      FTransform::bcc2d = PlanT::bcc2d;

    if (instances==0)
      Agent2::libraries.push_back (this);

    instances ++;
  }

  template<class PlanT> PlanT* 
  PlanAgent2<PlanT>::get_plan (unsigned nx, unsigned ny, const std::string& cl)
  {
    for (unsigned iplan=0; iplan<plans.size(); iplan++)
      if (plans[iplan]->nx == nx && plans[iplan]->ny == ny &&
	  plans[iplan]->call == cl)
	return plans[iplan];
    
    plans.push_back( new PlanT (nx, ny, cl) );
    return plans.back();
    
  }


}

#endif
