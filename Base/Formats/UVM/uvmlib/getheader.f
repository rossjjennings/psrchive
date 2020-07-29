      subroutine getheader(infile,prgm,newscan,convert,istat)
c=======================================================================
c   Routine to detemine which program data is being called for and
c   then call the appropriate routine to interface with the routines
c   written for the P-1260 programs from circa 1992.
c   
c   Joanna Rankin,--February 1995
c=======================================================================
      integer prgm
      common/P110baseline/ nvals,ival,sigmas(25)
      character*80 infile,path,file,temp
      integer fd
      common /fid/ fd
      logical convrt
c
c   Find the length of the infile string
      do i=1,80
        if(infile(i:i).eq." ") go to 4
      end do
      i=81
    4 nstring=i-1
c   Find the path if any
      do i=nstring,1,-1
        if(infile(i:i).eq."/") go to 8
      end do
      i=0
    8 npath=i
      path=infile(1:npath)
      nfile=nstring-npath
      file=infile(npath+1:nstring)

      call sgiinithack
      call sgiprepfd
      open(unit=41,file=infile,status='old',err=100)
      call sgigetfd(fd)

c     write(*,*) path,file
c
c  Check current data
      if(file(1:1).eq."S".and.file(8:8).le."9"
     .           .and.file(8:8).ge."0") then
        prgm=1260
        call findscn(newscan,convert,istat)
        write(*,'("Program:",i5)') prgm
        if(istat.ne.0) go to 99
        return
c  Check P110 program
      else if(file(1:1).eq."H".and.file(8:8).ge."a"
     .           .and.file(17:17).eq."u") then
        prgm=110
        call P110readhdr(istat)
        if(istat.ne.0) go to 99
        close(41)
        if(npath.eq.0) then
          infile="S"//file(2:nfile)
        else
          temp="S"//file(2:nfile)
          infile=path(1:npath)//temp(1:nfile)
        end if
        call sgiprepfd
        open(unit=41,file=infile,status='old')
        call sgigetfd(fd)
        ival=0
        nvals=0
c  Read first record
c        call P110readdata(convert,istat)
        return
c  Check P467 program
      else if(file(1:1).eq."H".and.file(8:8).ge."a"
     .          .and.file(17:17).eq."l") then
        prgm=467
        write(*,'("Program:",i5)') prgm
        call P467readhdr(istat)
        if(istat.ne.0) go to 99
        close(41)
        if(npath.eq.0) then
          infile="S"//file(2:nfile)
        else
          temp="S"//file(2:nfile)
          infile=path(1:npath)//temp(1:nfile)
        end if
        call sgiprepfd
        open(unit=41,file=infile,status='old')
        call sgigetfd(fd)
c  Read first record
c        call P467readdata(convert,istat)
        return
c  Check spulses/WSRT format
      else if(file(1:1).eq."s".or.file(1:1).eq."a") then
        prgm=999
        write(*,'("Program:",i5)') prgm
        call SPULreadhdr(istat)
        if(istat.ne.0) go to 99
c  Read first record
c        call SPULreaddata(convert,istat)
        return
c  Check P868 data
      else if(file(1:1).eq."G".or.file(1:1).eq."A") then
        prgm=868
        write(*,'("Program:",i5)') prgm
        call findscn(newscan,convert,istat)
        if(istat.ne.0) go to 99
        return
      else
c        stop "Getheader: ambiguous file name"
        istat = -1
	return
      end if
      return
   99 write(*,*) "Getheader: header record read error",istat
c      stop
      return
c Go here on file open error
  100 istat=-1
      return
      end
