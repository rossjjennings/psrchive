


      subroutine smooth_mw(pr,nbin,maxwidth, rmsp,kwmax,snrmax,smmax)
c******************************************************************
c
c  convolves profile pr(nbin) with a boxcar of width kw.  it returns
c    the width kwmax which gave the highest s/n ratio snrmax, and the
c    corresponding pulse amplitude smmax.
c
c RTE 17 Feb 00, parameterized maximum boxcar witdth
c
c******************************************************************
c

      integer nbin,kwmax, maxwidth
      real*4 pr(*),rmsp,snrmax,smmax
c
      integer ksm,j,k,kw,nn,ja,jj
      real*4 s,wrk(2048),al,an,sn,smax
c
      snrmax=0.
      if (nbin>1024) stop 'smooth.f Increase dim of wrk in smooth_mw'
c---------------------------------------
c  remove baseline
      ksm=nbin/2.5+0.5
      smax=1.e30
      do 10 j = 1,nbin
        s=0.0
        do 20 k = 1,ksm
          s = s + pr(mod(j+k-1,nbin)+1)
   20   continue
        if(s.lt.smax) smax=s
   10 continue
      smax=smax/ksm
      do 30 j = 1,nbin
        pr(j) = pr(j) - smax
   30 continue
c--------------------------------------
c
c
      do 40 nn=1,1000
        kw=2**(nn-1)
        if (kw.gt.maxwidth) goto 70
        if(kw.gt.nbin/2) return
	s=0.0
	do 50 k=1,kw
	  s=s+pr(k)
	  wrk(k)=pr(k)
   50   continue
	ja=0
	smax=s
	do 60 j=2,nbin
	  ja=ja+1
	  if(ja.gt.kw) ja=ja-kw
	  al=wrk(ja)
	  jj=j+kw-1
	  if(jj.gt.nbin)jj=jj-nbin
	  an=pr(jj)
	  s=s+an-al
	  wrk(ja)=an
	  if(s.gt.smax) smax=s
   60   continue

        sn=smax/(rmsp*sqrt(kw*(1.+float(kw)/nbin)))
        if(sn.gt.snrmax) then
          snrmax=sn
          kwmax=kw
          smmax=smax/kw
        endif
   40 continue

 70   end




C rte old interface
      subroutine smooth(pr,nbin, rmsp,kwmax,snrmax,smmax)
      integer nbin,kwmax, maxwidth
      real*4 pr(*),rmsp,snrmax,smmax
      
      call smooth_mw(pr,nbin,32, rmsp,kwmax,snrmax,smmax)
      end



