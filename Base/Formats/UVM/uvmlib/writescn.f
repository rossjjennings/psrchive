c-----------------------------------------------------------------------
      subroutine writescn(iband,nprofiles,lfn)
c     ******************************************************************
c     Routine to write an Arecibo timing scan and define the most
c     important header parameters along with the pulse profile.
c     This version passes all quantities through common blocks,
c     see also findscn. --Joanna Rankin May 1994
c
      integer date,startime,arecnum,nscan,aposcrd,admptim,istat,
     .admplen,admpsrd,ainttim,aextst,apoladd,anumsbc,alensbc,
     .apbins,admptot,anrecscn,aendscan,apbinsmax,controlword
      parameter (apbinsmax=2048)
      common/header/ period,samint,scantime,date,startime,arecnum,nscan,
     .aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .admptim,admplen,admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc,
     .alensbc,apbins,adm,arm,abintot,admptot,abtwscn,scaleI,sigmaI,
     .abbm3lo,abbm4lo,baseval,anrecscn,rm,erm,aver,source,controlword,
     .nwins,nwbins(5),wndows,sver,nseq
      common/iquv/ idata(apbinsmax),qdata(apbinsmax),udata(apbinsmax),
     .             vdata(apbinsmax)
      common/lchi/ ldata(apbinsmax),chidata(2*apbinsmax)
      real idata,qdata,udata,vdata,ldata,chidata
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,baseval,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character*9 source
      character wndows*5
c
c
c     Write out 4 header records.
      write(lfn,101) aver,date,startime,source,
     .arecnum,nscan,aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,sver
 101  format(a3,2i5,a9,i4,1x,i7,i5,e9.2,i2,3f9.5,a3)
      write(lfn,102) za,azaend,aclkrat,admptim,admplen,
     .admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc
 102  format(3f9.5,4i7,i7,f12.5,2i2)
      write(lfn,103) alensbc,apbins,period,adm,arm,
     .samint,admptot,timediff
 103  format(2i5,f18.15,2f9.4,f9.7,i7,e11.4,1x)
      iexp=log10(scaleI)
      amant=scaleI/10.**iexp
      write(lfn,104) amant,iexp,sigmaI,nwins,(nwbins(i),i=1,5),
     .baseval,wndows,anrecscn,scantime,nseq,controlword
 104  format(f7.4,'E',i1,f9.4,i3,5i3,e12.4,a5,i2,f13.1,i6,1x,o5)
c
c output I
      write(lfn,"((8f10.5))") (idata(j),j = 1,apbins)
c
c output Q
      write(lfn,"((8f10.5))") (qdata(j),j = 1,apbins)
c
c output U
      write(lfn,"((8f10.5))") (udata(j),j = 1,apbins)
c
c output V
      write(lfn,"((8f10.5))") (vdata(j),j = 1,apbins)
      return
      end
