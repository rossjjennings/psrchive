      subroutine findscn(newscan,convert,istat)
c     ******************************************************************
c     Routine to find the specified scan to process and pass it to the
c     main program.  This version passes all the data though common 
c     blocks.  See also readscn.  --Joanna Rankin July 1993
c     ******************************************************************
      integer date,startime,arecnum,nscan,aposcrd,admptim,istat,
     .admplen,admpsrd,ainttim,aextst,apoladd,anumsbc,alensbc,
     .apbins,admptot,anrecscn,aendscan,apbinsmax
      parameter (apbinsmax=2048)
      common/header/ period,samint,scantime,date,startime,arecnum,nscan,
     .aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .admptim,admplen,admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc,
     .alensbc,apbins,adm,arm,abintot,admptot,abtwscn,scaleI,sigmaI,
     .abbm3lo,abbm4lo,asynfreq,anrecscn,rm,erm,aver,source,controlword,
     .nwins,nwbins(5),wndows,sver,nseq,obsvtry
      common/iquv/ idata(apbinsmax),qdata(apbinsmax),udata(apbinsmax),
     .             vdata(apbinsmax)
      common/lchi/ ldata(apbinsmax),chidata(apbinsmax)
      real idata,qdata,udata,vdata,ldata,chidata
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,asynfreq,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character*5 wndows
      character*9 source
      character*11 obsvtry
      logical test,convert
      obsvtry="Arecibo/UVM"
c
      write(*,100) newscan
 100  format(' Looking for scan: ',i7)
      test=.true.
      do while(test)
        call readscn(convert,istat)
        if(istat.ne.0) then
          write(*,*) 'EOF or ERR on input...'
          stop
        end if
        if(newscan.eq.0) then
          write(*,102) nscan
 102      format(' Found scan: ',i7)
          newscan=nscan
          test=.false.
          go to 1
        end if
        if(newscan.lt.nscan) then
          write(*,101) nscan,newscan
 101      format(' Found scan: ',i7,' > input scan: ',i7)
          stop
        else if(newscan.eq.nscan) then
          write(*,102) nscan
          test=.false.
          go to 1
        else
          continue
        end if
 1      continue
      end do
c
      return
      end
