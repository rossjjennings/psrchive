      subroutine P467readdata(convert,istat)
c=======================================================================
c   Routine to read the pulse records on the P110 polarization data as
c   now stored on ascii files.  The records are 4 x nsamps x f8.3 +16
c   characters long and occupy their own file (i.e, no header prologue).
c   These data have always been ascii, but are now stored on disk files.
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
     .nwins,nwbins(5),wndows,sver,nseq
      common/P467baseline/ nbi,nb1s,nb1e,nb2s,nb2e
      common/iquv/ idata(apbinsmax),qdata(apbinsmax),udata(apbinsmax),
     .             vdata(apbinsmax)
      common/lchi/ ldata(apbinsmax),chidata(2*apbinsmax)
      real idata,qdata,udata,vdata,ldata,chidata
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,asynfreq,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character*9 source
      character wndows*5
      logical convert
      character head*48,buffer*16384
      integer*4 header(12),data(4096)
      equivalence (head,header),(buffer,data)
      integer dtabytes,fileid
      common /fid/ fileid
c
      istat=0
c     calculate data length and see if pad characters were written
c       to make a record that was 3*n bytes long
      dtabytes=4*4*apbins
      ntbytes=dtabytes+48
      npad=3-(ntbytes-3*(ntbytes/3))
      if(npad.lt.3) dtabytes=dtabytes+npad
      length=dtabytes
      if(apbins.gt.apbinsmax) stop "P467readdata: data too long"
c     read a record
      call readbytes(head,48,data,length,fileid)
      nseq=header(1)
      if(length.ne.dtabytes) go to 16
c
      do i=1,apbins
        idata(i)=data(i)
        qdata(i)=data(apbins+i)
        udata(i)=data(2*apbins+i)
        vdata(i)=data(3*apbins+i)
      end do
c
c     compute the baseline standard deviation
      ndta=0
      sum=0.
      sumsq=0.
      do i=nb1s,nb1e
        sum=sum+idata(i)
        sumsq=sumsq+idata(i)**2
        ndta=ndta+1
      end do
c     ndta1=ndta
c     sum1=sum
c     sumsq1=sumsq
      if(nbi.eq.2) then
        do i=nb2s,nb2e
          sum=sum+idata(i)
          sumsq=sumsq+idata(i)**2
          ndta=ndta+1
        end do
      end if
c     ndta2=ndta-ndta1
c     sum2=sum-sum1
c     sumsq2=sumsq-sumsq1
c     avg1=sum1/ndta1
c     sigmaI1=sqrt(sumsq1/ndta1-avg**2)
c     avg2=sum2/ndta2
c     sigmaI2=sqrt(sumsq2/ndta2-avg**2)
c     write(*,*) avg1,sigmaI1,avg2,sigmaI2
      avg=sum/ndta
      sigmaI=sqrt(sumsq/ndta-avg**2)
c
c     compute the averaged position angle and linear polarization
      if(convert) then
        do i=1,apbins
          chidata(i)=0.5*atan2(udata(i),qdata(i))*(180./3.14159265)
          ldata(i)=sqrt(qdata(i)**2+udata(i)**2)
        end do
      end if
c     write(*,*) nseq,idata(1),idata(64),sigmaI,apbins
      return
   16 istat=1
      return
      end
