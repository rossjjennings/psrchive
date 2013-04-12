      subroutine SPULreadhdr(istat)
c=======================================================================
c   Routine to read the header records on the spulses/WSRT polarimerty 
c   data stored on ascii files.  The header records comprise four lines
c   at the top of each data file.  Data are read on lfn 41.
c
c   JMR --9 Febr 1995
c   JMR --27 Mar 2001 adapted from P110readhdr
c   JMR --1 Jan 2005 attempt to fix MJD conversion
c=======================================================================
      integer date,startime,arecnum,nscan,aposcrd,admptim,istat,
     .admplen,admpsrd,ainttim,aextst,apoladd,anumsbc,alensbc,
     .apbins,admptot,anrecscn,aendscan,apbinsmax,controlword
      parameter (apbinsmax=2048)
      common/header/ period,samint,scantime,date,startime,arecnum,nscan,
     .aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .admptim,admplen,admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc,
     .alensbc,apbins,adm,arm,abintot,admptot,abtwscn,scaleI,sigmaI,
     .abbm3lo,abbm4lo,asynfreq,anrecscn,rm,erm,aver,source,controlword,
     .nwins,nwbins(5),wndows,sver,nseq,obsvtry
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,asynfreq,rm,erm
      real*8 period,samint,scantime,jdate
      character*3 aver,sver
      character source*9,obsvtry*11
      character*12 datum
      character wndows*5
      integer obsdat
c
      istat=0
      ibug=1
      read(41,*,err=12)npulses,apbins,delphi,ntypes
      ibug=2
      read(41,'(a)',err=12) datum
      source=datum(1:9)
      if(datum(1:1).eq." ") source=datum(2:10)
      ibug=3
      read(41,'(a)',err=12) datum
      obsvtry=datum(1:11)
      if(datum(1:1).eq." ") obsvtry=datum(2:12)
      ibug=4
      read(41,*,err=12)nscan,nskip,freq,bwd,period,scantime
c
c     if(ntypes.ne.4) then 
c       write(*,*) "Total power observations only?!?"
        aposcrd=ntypes
c       stop
c     end if
c
c Define quantitites
      obfreq=freq
      obsdat=scantime
      jdate=obsdat+2400000.5-2440000.
c     write(*,*) scantime,jdate
      call juldate(1,im,id,iy,jdate)
c     write(*,*) "SPULreadhdr: ",id,im,iy
      if(iy.lt.2000) then
        iy=iy-1900
      else
        iy=iy-2000
      end if
      if(iy.eq.100) iy=0
      call dayyear(im,id,iy,idn)
      date=idn*100+iy
c     write(*,*) iy,idn,obsdat
      stime=scantime-obsdat
      startime=stime*86400.
c     write(*,*) startime
      samint=(delphi*period/360.)*apbins/period
      abandwd=bwd
      admptim=period*delphi*1.E6/360.
      adm=dm
      arm=rm
      wndows="P    "
      nwins=1
      nwbins(1)=apbins
      scaleI=1000.
c     write(*,*) source,obfreq,samint,apbins,abandwd,admptim,adm,arm
      anumsbc=4
      alensbc=1
      return
   12 istat=ibug
      return
      end
