//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/fft/median_smooth.h,v $
   $Revision: 1.1 $
   $Date: 2004/09/02 03:04:04 $
   $Author: redwards $*/

#ifndef __fft_smooth_h
#define __fft_smooth_h

#include <algorithm>

namespace fft {

  extern bool verbose;

  template <class T> 
  void median_smooth (vector<T>& data, unsigned ndim, unsigned wsize) {

    vector<typename T::type> temp (data.size());
    for (unsigned idim=0; idim<ndim; idim++) {
      for (unsigned idat=0; idat<data.size(); idat++)
	temp[idat] = data[idat][idim];
      median_smooth (temp, wsize);
      for (unsigned idat=0; idat<data.size(); idat++)
	data[idat][idim] = temp[idat];
    }

  }

  template <class T> 
    void median_smooth (vector<T>& data, unsigned wsize) {

    if (data.size() < 4)
      return;

    if (wsize < 3)
      return;
    
    if (data.size() < wsize + 1)
      return;

    // make it an odd number
    if (wsize%2 == 0)
      wsize ++;

    unsigned ipt = 0, jpt = 0;

    unsigned middle = wsize/2;
    vector<T> window (wsize);

    unsigned rsize = data.size() - wsize + 1;
    vector<T> result (data.size());

    unsigned truncated = 0;

    if (verbose)
      cerr <<"fft::median_filter window="<< wsize <<"/"<< data.size() <<endl;

    // deal with leading edge
    for (ipt=0; ipt < middle; ipt++) {
      truncated = middle + 1 + ipt;
      for (jpt=0; jpt < truncated; jpt++)
	window[jpt] = data[jpt];
      sort (window.begin(), window.begin()+truncated);
      result[ipt] = window[truncated/2];
    }

    for (ipt=0; ipt < rsize; ipt++) {
      for (jpt=0; jpt<wsize; jpt++)
	window[jpt] = data[ipt+jpt];
      sort (window.begin(), window.end());
      result[ipt+middle] = window[middle];
    }

    // deal with trailing edge
    for (ipt=rsize; ipt < data.size()-middle; ipt++) {
      truncated = data.size() - ipt;
      for (jpt=0; jpt < truncated; jpt++)
	window[jpt] = data[ipt+jpt];
      sort (window.begin(), window.begin()+truncated);
      result[ipt+middle] = window[truncated/2];
    }

    for (ipt=0; ipt < data.size(); ipt++)
      data[ipt] = result[ipt];
 
  } // end median_filter

} // end namespace fft

#endif // ! _fft_smooth_h
