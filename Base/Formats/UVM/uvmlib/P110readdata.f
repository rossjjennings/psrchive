      subroutine P110readdata(convert,istat)
c=======================================================================
c   Routine to read the pulse records on the P110 polarization data as
c   now stored on ascii files.  The records are 4 x nsamps x f8.3 +16
c   characters long and occupy their own file (i.e, no header prologue).
c   These data have always been ascii, but are now stored on disk files,
c   and thus have no end-of record separators.
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
      common/P110baseline/ nvals,ival,sigmas(25)
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
      character head*16,buffer*8002
      integer dtabytes,fileid
      common /fid/ fileid
c
      istat=0
c     calculate data length and see if pad characters were written
c       to make a record that was 3*n bytes long
      dtabytes=4*8*apbins
      ntbytes=dtabytes+16
      npad=3-(ntbytes-3*(ntbytes/3))
      if(npad.lt.3) dtabytes=dtabytes+npad
      length=dtabytes
      if(apbins.gt.250) stop "P467readdata: data too long"
c     read a record
      call readbytes(head,16,buffer,length,fileid)
      if(length.ne.dtabytes) go to 16
c
c     decode header values
      read(head,'(i8,2i4)') nseq,isatfg1,isatfl2
c     decode input buffer
      read(buffer,'(1000f8.3)') (idata(i),i=1,apbins),
     . (qdata(i),i=1,apbins),(udata(i),i=1,apbins),(vdata(i),i=1,apbins)
c
c     compute the baseline standard deviation
      ndta=0
      sum=0.
      sumsq=0.
      do i=1,5
        sum=sum+idata(i)
        sumsq=sumsq+idata(i)**2
        ndta=ndta+1
      end do
      avg=sum/ndta
      sigma1=sqrt(sumsq/ndta-avg**2)
      ndta=0
      sum=0.
      sumsq=0.
      do i=apbins-4,apbins
        sum=sum+idata(i)
        sumsq=sumsq+idata(i)**2
        ndta=ndta+1
      end do
      avg=sum/ndta
      sigma2=sqrt(sumsq/ndta-avg**2)
      sigma=amin1(sigma1,sigma2)
c     compute the overall sigma from the sinking average
      if(nvals.lt.25) then
        nvals=nvals+1
        ival=ival+1
        sigmas(ival)=sigma
      else if(nvals.eq.25) then
        ival=ival+1
        if(ival.gt.25) ival=1
        sigmas(ival)=sigma
      else
        write(*,*) ival,nvals
        stop "nvals out of range"
      end if
      var=0.
      do i=1,nvals
        var=var+sigmas(i)**2
      end do
      sigmaI=sqrt(var/nvals)
c     write(*,*) nseq,ival,nvals,idata(110),sigma,sigmaI,scaleI
c
c     compute the averaged position angle and linear polarization
      if(convert) then
        do i=1,apbins
          q=qdata(i)
          u=udata(i)
          if(q+u.ne.0.) then
            chidata(i)=0.5*atan2(u,q)*(180./3.14159265)
          else
            chidata(i)=180.
          end if
          ldata(i)=sqrt(qdata(i)**2+udata(i)**2)
        end do
      end if
      return
   16 istat=1
      return
      end
