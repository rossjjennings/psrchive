#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_IPP

#include "Error.h"

#include "IPP_Transform.h"

int FTransform::ipp_initialise(){
  FTransform::frc1d_calls.push_back( &ipp_frc1d );
  FTransform::fcc1d_calls.push_back( &ipp_fcc1d );
  FTransform::bcc1d_calls.push_back( &ipp_bcc1d );

  FTransform::norms.push_back( FTransform::nfft );
  FTransform::valid_libraries.push_back( "IPP" );

  if( FTransform::library==string() ){
    FTransform::library = "IPP";
    FTransform::frc1d = &ipp_frc1d;
    FTransform::fcc1d = &ipp_fcc1d;
    FTransform::bcc1d = &ipp_bcc1d;
    FTransform::norm = FTransform::norms.back();
  }

  return 0;
}

FTransform::IPP_Plan::~IPP_Plan(){
  if( pBuffer )
    delete [] pBuffer;
  if( Spec ){
    if( fft_call == "frc1d" )
      ippsFFTFree_R_32f( (IppsFFTSpec_R_32f*)Spec );
    else
      ippsFFTFree_C_32fc( (IppsFFTSpec_C_32fc*)Spec );
  }
}

FTransform::IPP_Plan::IPP_Plan() : FTransform::Plan() {
  pBuffer = 0;
  Spec = 0;
}

FTransform::IPP_Plan::IPP_Plan(unsigned _ndat, unsigned _ilib, string _fft_call)
  : FTransform::Plan(_ndat,_ilib,_fft_call)
{
  init(ndat,ilib,fft_call);
}

void FTransform::IPP_Plan::init(unsigned _ndat, unsigned _ilib, string _fft_call){
  initialise(_ndat,_ilib,_fft_call);

  int order = 0;
  int pSize = 0;

  unsigned doubling = 1;
  while( doubling < ndat ){
    order++;
    doubling *= 2;
  }
  if( doubling != ndat )
    throw Error(InvalidState,"FTransform::IPP_Plan()::IPP_Plan()",
		"Your ndat (%d) was not a power of 2",ndat);

  if( fft_call == "frc1d" ){
    ippsFFTInitAlloc_R_32f( (IppsFFTSpec_R_32f**)&Spec, order,
			    IPP_FFT_NODIV_BY_ANY, ippAlgHintFast);
    ippsFFTGetBufSize_R_32f( (IppsFFTSpec_R_32f*)Spec, &pSize);
  }
  else{
    ippsFFTInitAlloc_C_32fc( (IppsFFTSpec_C_32fc**)&Spec, order,
			    IPP_FFT_NODIV_BY_ANY, ippAlgHintFast);
    ippsFFTGetBufSize_C_32fc( (IppsFFTSpec_C_32fc*)Spec, &pSize);
  }

  pBuffer = new Ipp8u[pSize];
}

int FTransform::ipp_frc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = FTransform::get_ilib("IPP");
  IPP_Plan* plan = 0;

  if( FTransform::last_frc1d_plan && 
      FTransform::last_frc1d_plan->ilib==ilib &&
      FTransform::last_frc1d_plan->ndat == ndat )
    plan = (IPP_Plan*)FTransform::last_frc1d_plan;

  if( !plan ){
    for( unsigned iplan=0; iplan<FTransform::plans[ilib].size(); iplan++){
      if( FTransform::plans[ilib][iplan]->ndat == ndat && 
	  FTransform::plans[ilib][iplan]->fft_call == "frc1d"){
	plan = (IPP_Plan*)FTransform::plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan )
    plan = new IPP_Plan(ndat,ilib,"frc1d");

  ///////////////////////////////////////
  // Do the transform
  ippsFFTFwd_RToCCS_32f( (const Ipp32f*)src, (Ipp32f*)dest,
			 (const IppsFFTSpec_R_32f*)plan->Spec,
			 plan->pBuffer );

  return 0;
}

int FTransform::ipp_fcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = FTransform::get_ilib("IPP");
  IPP_Plan* plan = 0;

  if( FTransform::last_fcc1d_plan && 
      FTransform::last_fcc1d_plan->ilib==ilib &&
      FTransform::last_fcc1d_plan->ndat == ndat )
    plan = (IPP_Plan*)FTransform::last_fcc1d_plan;

  if( !plan ){
    for( unsigned iplan=0; iplan<FTransform::plans[ilib].size(); iplan++){
      if( FTransform::plans[ilib][iplan]->ndat == ndat && 
	  FTransform::plans[ilib][iplan]->fft_call == "fcc1d"){
	plan = (IPP_Plan*)FTransform::plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan ){
    plan = new IPP_Plan(ndat,ilib,"fcc1d");
    FTransform::plans[ilib].push_back( plan );
  }

  ///////////////////////////////////////
  // Do the transform
  ippsFFTFwd_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)plan->Spec,
			plan->pBuffer );

  return 0;
}

int FTransform::ipp_bcc1d(unsigned ndat, float* dest, float* src){
  ///////////////////////////////////////
  // Set up the plan
  static unsigned ilib = FTransform::get_ilib("IPP");
  IPP_Plan* plan = 0;

  if( FTransform::last_bcc1d_plan && 
      FTransform::last_bcc1d_plan->ilib==ilib &&
      FTransform::last_bcc1d_plan->ndat == ndat )
    plan = (IPP_Plan*)FTransform::last_bcc1d_plan;

  if( !plan ){
    for( unsigned iplan=0; iplan<FTransform::plans[ilib].size(); iplan++){
      if( FTransform::plans[ilib][iplan]->ndat == ndat && 
	  FTransform::plans[ilib][iplan]->fft_call == "bcc1d"){
	plan = (IPP_Plan*)FTransform::plans[ilib][iplan].ptr();
	break;
      }
    }
  }

  if( !plan ){
    plan = new IPP_Plan(ndat,ilib,"bcc1d");
    FTransform::plans[ilib].push_back( plan );
  }

  ///////////////////////////////////////
  // Do the transform
  ippsFFTInv_CToC_32fc( (const Ipp32fc*)src, (Ipp32fc*)dest,
			(const IppsFFTSpec_C_32fc*)plan->Spec,
			plan->pBuffer );

  return 0;
}

#endif
