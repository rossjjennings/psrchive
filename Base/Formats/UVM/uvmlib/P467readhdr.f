      subroutine P467readhdr(istat)
c=======================================================================
c   Routine to read the header records on the P467 polarization data as
c   now stored on ascii files.  The records are 336 characters long and
c   are prologues to the data records.
c
c   JMR --9 Febr 1995
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
      common/P467baseline/ nbi,nb1s,nb1e,nb2s,nb2e
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,asynfreq,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character*9 source
      character*11 obsvtry
      character wndows*5
      character pulsar*8
      integer day,month,year,hour,min,sec
      obsvtry="Arecibo"
c
      istat=0
      read(41,4) pulsar,day,month,year,hour,min,sec,nscan1,freq,
     .  bwd,tau,taud,nsamps,smint,degsam,bscale,
     .  nbi,nb1s,nb1e,nb2s,nb2e,nsi,ns1s,ns1e,dm,rm
    4 format(//////11x,a8/11x,3(i2,1x)/11x,3(i2,1x)//10x,i20/10x,e20.0/
     .  3(10x,e20.0/),10x,i20/2(10x,e20.0/)//10x,e20.0////////////////,
     .  5(10x,i20/)/3(10x,i20/)////////////////2(10x,f20.0/))
c Define quantitites
      write(source,'("P",a8)') pulsar
      write(*,'(a9)') source
      nscan=nscan1
      obfreq=freq/1.e6
      if(obfreq.eq.800.) obsvtry="NRAO 300-FT"
      abandwd=bwd/1.e6
      admptim=tau*1000.
      apbins=nsamps
      period=smint*360./degsam
      samint=nsamps*smint/period
      scaleI=bscale*1.e6
      nwins=nbi+nsi
      wndows="NPN  "
      nwbins(1)=nb1e-nb1s+1
      nwbins(2)=ns1e-ns1s+1
      nwbins(3)=nb2e-nb2s+1
      call dayyear(month,day,year,idn)
      date=idn*100+year
      startime=hour*3600+min*60+sec
      adm=dm
      arm=rm
c     write(*,*) source,obfreq,samint,apbins,abandwd,admptim,adm,arm
      anumsbc=4
      alensbc=1
      return
   12 istat=1
      return
      end

