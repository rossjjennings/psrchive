
      subroutine resopen (r2flun, r2fname)
      integer        r2flun
      character*(*)  r2fname

      open(r2flun,file=r2fname,form='unformatted',status='old')
      rewind r2flun
      end

      subroutine resread (r2flun, mjd, resiturns, resitimes,
     +     phase, freq, weight, error, preres, stat)
      
      integer r2flun
      double precision resiturns,mjd
      double precision phase,preres
      double precision weight, resitimes
      double precision freq,error
      integer stat

      read(r2flun, IOSTAT=stat) mjd, resiturns, resitimes,
     +     phase, freq, weight, error, preres
      end

      subroutine resclose (r2flun)
      integer        r2flun

      close (r2flun)
      end
 

