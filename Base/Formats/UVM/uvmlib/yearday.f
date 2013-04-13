      subroutine yearday(month,day,year,date)
c
c     Purpose
c     To convert the day number in a year to month and date
c     JMR -- May 1994
c
      dimension mth(12),mthday(12)
      integer day,year,date
      data mth/31,28,31,30,31,30,31,31,30,31,30,31/
c
      do i=1,12
        mthday(i)=mth(i)
      end do
      if(mod(year,4).eq.0) mthday(2)=29
      nd=date
      do i=1,12
        if(nd.le.mthday(i)) go to 12
        nd=nd-mthday(i)
      end do
      stop "yearday error"
   12 month=i
      day=nd
      return
c
      entry dayyear(month,day,year,date)
      if(month.eq.1) then
        date=day
        return
      else if(month.eq.2) then
        date=31+day
        return
      else
        date=0
        do i=1,month-1
          date=date+mth(i)
        end do
        date=date+day
        if(mod(year,4).eq.0) date=date+1
      end if
      return
      end
