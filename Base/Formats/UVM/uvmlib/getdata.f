      subroutine getdata(prgm,convert,istat)
c=======================================================================
c   Routine to detemine which program data is being called for and
c   then call the appropriate routine to interface with the routines
c   written for the P-1260 programs from circa 1992.
c   
c   Joanna Rankin,--February 1995
c=======================================================================
      integer prgm
      character*80 infile,path,file
      logical convert
c  Check current data
      if(prgm.eq.1260) then
        call readscn(convert,istat)
        return
c  Check P110 program
      else if(prgm.eq.110) then
        call P110readdata(convert,istat)
        return
c  Check P467 program
      else if(prgm.eq.467) then
        call P467readdata(convert,istat)
        return
c  Check spulses/WSRT format
      else if(prgm.eq.999) then
        call SPULreaddata(convert,istat)
        return
c  Check P868 data
      else if(prgm.eq.868) then
        call readscn(convert,istat)
        return
      else
        stop "Getdata: illegal call"
      end if
      return
      end
