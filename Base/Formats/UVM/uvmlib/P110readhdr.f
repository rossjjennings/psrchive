      subroutine P110readhdr(istat)
c=======================================================================
c   Routine to read the header records on the P110 polarization data as
c   now stored on ascii files.  The records are 400 characters long and
c   occupy their own separate file.  Data are read on lfn 41.
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
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,asynfreq,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character*9 source
      character wndows*5
      character pulsar*8,obsvtry*11
      integer obsdat,obstim
      obsvtry="Arecibo"
c
      istat=0
      read(41,4,err=12) freq,pulsar,obsdat,
     .     obstim,intape,
     .     samintl,nsamps,
     .     
     .     intsamp,bwd,tc,period,dm,rm
    4 format(7x,f6.1,46x,a8,7x,i6/
     .     13x,i6,30x,a8/
     .     11x,f7.4,15x,i4//
     .     i8,5f8.0)
c Define quantitites
      obfreq=freq
      write(source,'(a8," ")') pulsar
      id=obsdat/10000
      im=obsdat/100-id*100
      iy=obsdat-id*10000-im*100
      call dayyear(im,id,iy,idn)
      date=idn*100+iy
      ih=obstim/10000
      im=obstim/100-ih*100
      is=obstim-ih*10000-im*100
      startime=ih*3600+im*60+is
      samint=samintl*nsamps/360.
      apbins=nsamps
      abandwd=bwd
      admptim=tc*1000.
      adm=dm
      arm=rm
      wndows="P    "
      nwins=1
      nwbins(1)=nsamps
      scaleI=1.e6
c     write(*,*) source,obfreq,samint,apbins,abandwd,admptim,adm,arm
      nscan=0
      anumsbc=4
      alensbc=1
      return
   12 istat=1
      return
      end
