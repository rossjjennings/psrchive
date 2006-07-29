/***************************************************************************
 *
 *   Copyright (C) 2005 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_IPP

#include "IPP_Transform.h"
#include "Error.h"

using namespace std;


FTransform::IPP_Plan::~IPP_Plan()
{
  if( pBuffer )
    delete [] pBuffer;
  if( Spec ){
    if( call == "frc1d" || call == "bcr1d" )
      ippsFFTFree_R_32f( (IppsFFTSpec_R_32f*)Spec );
    else
      ippsFFTFree_C_32fc( (IppsFFTSpec_C_32fc*)Spec );
  }
}


FTransform::IPP_Plan::IPP_Plan (size_t nfft, const string& fft_call)
{
#ifdef _DEBUG
  cerr << "FTransform::IPP_Plan nfft=" << nfft
       << " call='" << fft_call << "'" << endl;
#endif

  int order = 0;
  int pSize = 0;

  {
    unsigned doubling = 1;
    while( doubling < nfft ){
      order++;
      doubling *= 2;
    }
    if( doubling != nfft )
      throw Error (InvalidState, "FTransform::IPP_Plan",
		   "nfft=%d is not a power of 2", nfft);
  }    

  if( fft_call == "frc1d" || fft_call == "bcr1d" ) {
    IppStatus ret = ippsFFTInitAlloc_R_32f( (IppsFFTSpec_R_32f**)&Spec, order,
					    IPP_FFT_NODIV_BY_ANY,
					    ippAlgHintFast );
    ret = ippsFFTGetBufSize_R_32f( (IppsFFTSpec_R_32f*)Spec, &pSize );
  }
  else {
    ippsFFTInitAlloc_C_32fc( (IppsFFTSpec_C_32fc**)&Spec, order,
			     IPP_FFT_NODIV_BY_ANY, ippAlgHintFast );
    ippsFFTGetBufSize_C_32fc( (IppsFFTSpec_C_32fc*)Spec, &pSize );
  }

  if( nfft == 131072 ) 
    // KLUDGE!
    pBuffer = new Ipp8u[2*pSize];
  else
    pBuffer = new Ipp8u[pSize];

  ndat = nfft;
  call = fft_call;
  optimized = false;

}

int FTransform::IPP_Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (IPP_Plan, frc1d);

  ///////////////////////////////////////
  // Do the transform
  ippsFFTFwd_RToCCS_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)plan->Spec,
			 plan->pBuffer );
  return 0;
}

int FTransform::IPP_Plan::fcc1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (IPP_Plan, fcc1d);

  ///////////////////////////////////////
  // Do the transform
  ippsFFTFwd_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)plan->Spec,
			plan->pBuffer );
  return 0;
}

int FTransform::IPP_Plan::bcc1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (IPP_Plan, bcc1d);

  ///////////////////////////////////////
  // Do the transform
  ippsFFTInv_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)plan->Spec,
			plan->pBuffer );

  return 0;
}

int FTransform::IPP_Plan::bcr1d (size_t nfft, float* dest, const float* src)
{
  FT_SETUP (IPP_Plan, bcr1d);

  ///////////////////////////////////////
  // Do the transform
  ippsFFTInv_CCSToR_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)plan->Spec,
			 plan->pBuffer );
  return 0;
}

#endif
