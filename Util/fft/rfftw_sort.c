int rfftw_sort(int npts, float * src, float * dest)
{
  int i;
  dest[0] = src[0];
  dest[1] = 0.0;
  for(i=1; i<npts/2; ++i){
    dest[2*i] = src[i];
    dest[2*i+1] = src[npts-i];
  }
  dest[2*(npts/2)] = src[npts/2];
  if(npts%2==0) dest[2*(npts/2)+1] = 0.0;
  else dest[2*(npts/2)+1] = src[npts/2+1];
  return(0);
}

