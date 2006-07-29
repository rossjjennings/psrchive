//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

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
bcr1d = backward complex->real 1D

fcc2d = forward complex->complex 2D
bcc2d = backward complex->complex 2D

Call transforms as FUNC(ndat,dest,src);

*/

namespace FTransform {

  //! The normalization conventions
  enum norm_type { normal, nfft };

  //! Returns the normalization convention of the currently selected library
  norm_type get_norm ();

  //! Returns the name of the currently selected library
  std::string get_library ();
  
  //! Set the library to the specified name
  void set_library (const std::string& name);

  //! Clears out the memory associated with the plans
  void clean_plans();

  //! Whether to optimize or not
  extern bool optimize;

  /* ////////////////////////////////////////////////////////////////////
     
  One-dimensional FFT library interface
  
  //////////////////////////////////////////////////////////////////// */

  //! Pointer to one-dimensional FFT
  //! A size_t is a 32/64 bit unsigned integer on 32/64 bit machines
  /*! Arguments are: (size_t ndat, float* dest, const float* src) */
  typedef int (*fft_call) (size_t, float*, const float*);

  //! Pointer to the forward real-to-complex FFT
  extern fft_call frc1d;
  //! Pointer to the forward complex-to-complex FFT
  extern fft_call fcc1d;
  //! Pointer to the backward complex-to-complex FFT
  extern fft_call bcc1d;
  //! Pointer to the backward complex-to-real FFT
  /*! Data must be hermitian; ndat floats are output */
  extern fft_call bcr1d;

  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_frc1d(size_t ndat, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_fcc1d(size_t ndat, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_bcc1d(size_t ndat, float* srcdest);
  //! Inplace wrapper-function- performs a memcpy after FFTing
  int inplace_bcr1d(size_t ndat, float* srcdest);


  //! Base class of two-dimensional Fast Fourier Transforms
  class Plan : public Reference::Able {
  public:

    Plan();
    virtual ~Plan ();

    bool optimized;
    std::string call;
    size_t ndat;

  };

  extern Plan* last_frc1d;
  extern Plan* last_fcc1d;
  extern Plan* last_bcc1d;
  extern Plan* last_bcr1d;

  // The whole Agent kababble doesn't work!
  extern std::vector<Reference::To<Plan> > plans_for_clean_plans;

  //! Base class of one-dimensional FFT agents
  class Agent : public Reference::Able {

  public:

    Agent();

    virtual ~Agent();

    //! Name of the transform library
    std::string name;

    //! Forward complex-to-complex one-dimensional FFT
    fft_call fcc1d;

    //! Backward complex-to-complex one-dimensional FFT
    fft_call bcc1d;

    //! Forward real-to-complex one-dimensional FFT
    fft_call frc1d;

    //! Backward complex-to-real one-dimensional FFT
    fft_call bcr1d;

    //! The normalization type
    norm_type norm;

    //! Clean up the plans for this library
    virtual void clean_plans () = 0;

    //! Get the number of available libraries
    static unsigned get_num_libraries () { return libraries.size(); }

  protected:

    //! Install this as the current library
    void install ();

    //! Add a pointer to this instance to the libraries attribute
    void add ();

  private:

    //! List of all libraries
    static std::vector< Reference::To<Agent> > libraries;

    friend void set_library (const std::string& name);
    friend void clean_plans();

  };


  //! Template virtual base class of FFT library agents
  template <class PlanT>
  class PlanAgent : public Agent {

  public:

    //! Default constructor
    PlanAgent (const std::string& name, norm_type norm);

    //! Return an appropriate plan from this library
    PlanT* get_plan (size_t ndat, const std::string& call);

    //! Clean up the plans for this library
    void clean_plans ();

    //! Add an instance of this class to the Agent::libraries attribute
    static void enlist ();

    //! An instance of the agent for use by children
    static typename PlanT::Agent my_agent;

  protected:

    //! The plans managed by the agent for this library
    std::vector< Reference::To<PlanT> > plans;

  };

  //! Clean up the plans for this library
  template<class PlanT>
  void
  PlanAgent<PlanT>::clean_plans ()
  {
    plans.resize (0);
  }


  template<class PlanT> typename PlanT::Agent PlanAgent<PlanT>::my_agent;

  template<class PlanT>
  PlanAgent<PlanT>::PlanAgent (const std::string& _name, norm_type _norm)
  {
    name = _name;
    norm = _norm;

    this->fcc1d = PlanT::fcc1d;
    this->bcc1d = PlanT::bcc1d;
    this->frc1d = PlanT::frc1d;
    this->bcr1d = PlanT::bcr1d;
  }

  template<class PlanT> PlanT* 
  PlanAgent<PlanT>::get_plan (size_t ndat, const std::string& cl)
  {
    //    for (unsigned iplan=0; iplan<plans.size(); iplan++)
    //if (plans[iplan]->ndat == ndat && plans[iplan]->call == cl)
    //return plans[iplan];
    
    //    plans.push_back( new PlanT (ndat, cl) );
    //return plans.back();
    return new PlanT (ndat, cl);
  }

  template<class PlanT>
  void PlanAgent<PlanT>::enlist ()
  {
    PlanAgent<PlanT>* instance = new typename PlanT::Agent;
    instance->add();
  }


  /* ////////////////////////////////////////////////////////////////////
     
  Two-dimensional FFT library interface
  
  //////////////////////////////////////////////////////////////////// */

  //! Pointer to two-dimensional FFT
  typedef void (*fft2_call)(size_t, size_t, float*, const float*);

  extern fft2_call fcc2d;
  extern fft2_call bcc2d;

  //! Base class of two-dimensional Fast Fourier Transforms
  class Plan2 : public Reference::Able {
  public:

    virtual ~Plan2 () { }

    bool optimized;
    std::string call;
    size_t nx;
    size_t ny;

  };

  extern Plan2* last_fcc2d;
  extern Plan2* last_bcc2d;

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
    PlanT* get_plan (size_t nx, size_t ny, const std::string& call);

    //! An instance of the agent for use by children
    static typename PlanT::Agent my_agent;

  protected:

    //! The plans managed by the agent for this library
    std::vector< Reference::To<PlanT> > plans;

  private:

    //! Number of instances of this agent class
    static unsigned instances;

  };

  template<class PlanT> typename PlanT::Agent PlanAgent2<PlanT>::my_agent;

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
  PlanAgent2<PlanT>::get_plan (size_t nx, size_t ny, const std::string& cl)
  {
    for (unsigned iplan=0; iplan<plans.size(); iplan++)
      if (plans[iplan]->nx == nx && plans[iplan]->ny == ny &&
	  plans[iplan]->call == cl)
	return plans[iplan];
    
    plans.push_back( new PlanT (nx, ny, cl) );
    return plans.back();
    
  }


}

//! DEVELOPERS: Use of this macro greatly decreases the margin for error
#define FT_SETUP(PLAN,TYPE) \
  PLAN* plan = dynamic_cast<PLAN*>( last_ ## TYPE ); \
  if (!plan || plan->ndat != nfft || plan->call != #TYPE) \
    last_ ## TYPE = plan = Agent::my_agent.get_plan (nfft, #TYPE)

#define FT_SETUP2(PLAN,TYPE) \
  PLAN* plan = dynamic_cast<PLAN*>( last_ ## TYPE ); \
  if (!plan || plan->nx != nx || plan->ny != ny || plan->call != #TYPE) \
    last_ ## TYPE = plan = Agent::my_agent.get_plan (nx, ny, #TYPE);

#endif

