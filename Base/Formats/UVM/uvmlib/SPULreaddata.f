      subroutine SPULreaddata(convert,istat)
c=======================================================================
c   Routine to read the pulse records in the spulses/WSRT format as
c   now stored on ascii files.  These data have either been generated 
c   using various "extraction" programs or computed from PuMa output.  
c
c   JMR --9 Febr 1995
c   JMR --27 March 2001 converted from P467readdata
c=======================================================================
      integer date,startime,arecnum,nscan,aposcrd,admptim,istat,
     .admplen,admpsrd,ainttim,aextst,apoladd,anumsbc,alensbc,
     .apbins,admptot,anrecscn,aendscan,apbinsmax,controlword
      parameter (apbinsmax=2048)
      common/header/ period,samint,scantime,date,startime,arecnum,nscan,
     .aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .admptim,admplen,admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc,
     .alensbc,apbins,adm,arm,abintot,admptot,abtwscn,scaleI,sigmaI,
     .abbm3lo,abbm4lo,baseval,anrecscn,rm,erm,aver,source,controlword,
     .nwins,nwbins(5),wndows,sver,nseq,obsvtry
      common/P467baseline/ nbi,nb1s,nb1e,nb2s,nb2e
      common/iquv/ idata(apbinsmax),qdata(apbinsmax),udata(apbinsmax),
     .             vdata(apbinsmax)
      common/lchi/ ldata(apbinsmax),chidata(2*apbinsmax)
      real idata,qdata,udata,vdata,ldata,chidata
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,baseval,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character source*9,obsvtry*11
      character wndows*5
      logical convert
c
      istat=0
c     write(*,*) "ntypes ",aposcrd
      if(aposcrd.eq.4) then
        read(41,*,err=20,end=16)
     .     (idata(i),qdata(i),udata(i),vdata(i),i=1,apbins),
     .     sigmaI,baseval,seq
      else if(aposcrd.eq.1) then
         read(41,*,err=24,end=16)
     .     (idata(i),i=1,apbins),sigmaI,baseval,seq
      else
        write(*,*) "Undefined ntypes"
        stop
      end if
      nseq=seq
c
c     compute the averaged position angle and linear polarization
      if(convert) then
        do i=1,apbins
          chidata(i)=0.5*atan2(udata(i),qdata(i))*(180./3.14159265)
          ldata(i)=sqrt(qdata(i)**2+udata(i)**2)
        end do
      end if
c     write(*,*) nseq,idata(1),idata(64),sigmaI,apbins,istat
      return
   16 istat=5
      return
   20 write(*,*) "SPULreaddata: 4-Stokes data read error"
      stop
   24 write(*,*) "SPULreaddata: 1-Stokes data read error"
      stop
      end
