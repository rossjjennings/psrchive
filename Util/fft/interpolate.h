
namespace fft {

  template <class T>
    void interpolate (vector<T>& out, vector<T>& in) {
    
    vector<float> dom1 (in.size());
    vector<float> dom2 (in.size());

    unsigned ndim = T::ndim;

    unsigned ipt, idim;

    // for each dimension of the type T, perform an interpolation
    for (idim=0; idim<ndim; idim++) {

      for (ipt=0; ipt < in.size(); ipt++)
	dom1[ipt] = in[ipt][idim];

      fft::frc1d (in.size(), dom2.begin(), dom1.begin());

      dom1.resize (out.size()*2);

      // copy the transform from DC up
      for (ipt=0; ipt < dom2.size(); ipt++)
	dom1[ipt] = dom2[ipt];

      unsigned dom1i = dom1.size()-2;
      unsigned npt2 = dom2.size()/2;

      // copy the conjugate of the transform backwards, excluding DC
      for (ipt=1; ipt < npt2; ipt++) {
	dom1[dom1i]   = dom2[ipt*2];     // Re
	dom1[dom1i+1] = -dom2[ipt*2+1];  // Im
	dom1i -= 2;
      }

      // zero pad the rest
      unsigned stop = dom1.size()-dom2.size()+2;
      for (ipt=dom2.size(); ipt<stop; ipt++)
	dom1[ipt] = 0;

      dom2.resize (out.size()*2);

      fft::bcc1d (out.size(), dom2.begin(), dom1.begin());

      float factor = in.size();

      for (ipt=0; ipt < out.size(); ipt++)
	out[ipt][idim] = dom2[ipt*2];

    } // end for each dimension

  } // end of interpolate

} // end of namespace fft
