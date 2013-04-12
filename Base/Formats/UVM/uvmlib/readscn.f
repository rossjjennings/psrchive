      subroutine readscn(convert,istat)
c
c     Routine to read an Arecibo timing scan and return the most
c     important header parameters along with the pulse profile.
c     This version passes all quantities through common blocks,
c     see also findscn. --Joanna Rankin July 1993
c     and writescn. --other small changes May 1994
c
c=======================================================================
c	Glossary of parameters read and passed
c	call	convert	logical flag controls generation of
c					ldata and chidata arrays
c		istat	integer	flag, 0 for no errors, 1 otherwise
c	line 1:	aver	 a3	version, telescope program
c		date	 i5	date (dddyy)
c		startime i5	time, seconds from AST midnight
c		source	 a9	pulsar or source name
c		arecnum	 i4,1x	record number in scan (i.e. freq. chans)
c		nscan	 i7	scan number (dddynnn)
c		aendscan i5	
c		abandwd	 e9.2	bandwidth in MHz
c		aposcrd	 i2	
c		apos1st	 f9.5	R.A. in radians
c		apos2st	 f9.5	Decl in radians
c		aazst	 f9.5	azimuth (rad) at start of scan
c		sver	 a3	"Stokes" analysis program version
c	line2:	za	 f9.5	zenith angle (rad) at start of scan
c		azaend	 f9.5	zenith angle (rad) current or at end of scan
c		aclkrat	 f9.5	clock rate in MHz
c		admptim	 i7	correlator dump time (microseconds)
c		admplen	 i7	=anumsbc X alensbc
c		admpsrd	 i7	
c		ainttim	 i7	
c		aextst	 i7	ext. start flag in avrg prgm; int period in SP
c		obfreq	 f12.5	observing frequency in MHz
c		apoladd	 i2	polarization addition flag = 0 for polarimetry
c		anumsbc	 i2	# subcorrelators used = 4 for polarimetry
c	line3:	alensbc	 i5	number of acf/ccf lags=number of freq. chans.
c		apbins	 i5	number of samples per period
c				   or sampled fraction
c		period	 f18.15	period, I6 accuracy in SP, full acc in aver prgm
c		adm	 f9.2	dispersion measure in pc/cm^3
c		arm	 f9.2	rotation measure in rad-m^2
c		samint	 f9.7	interval between samples in seconds (average)
c				   sampled fraction of period in SP program
c		admptot	 i7	
c		abtwscn	 e11.4,1x
c	line4:	scaleI	 f9.5	scale to convert IQUV to micro Janskys
c		sigmaI	 f9.5	standard deviation of offpulse baseline in I
c		nwins	 i3	0 for average program; # windows in SP program
c		nwbins	 5i3	number of samples in each window
c		baseval  e12.4	baseline value removed
c		wndows	 a5	function of windowns as above (i.e., "I", for
c				   interpulse; "P" for pulse; and "N" for noise)
c		anrecscn i2	
c		scantime f13.1	scan time in microseconds ( =#periods*period)
c		nseq	 i6,1x	integer count of pulses or averages
c		controlword o5	octal word giving settings in analysis program
c=======================================================================
      integer date,startime,arecnum,nscan,aposcrd,admptim,istat,
     .admplen,admpsrd,ainttim,aextst,apoladd,anumsbc,alensbc,
     .apbins,admptot,anrecscn,aendscan,apbinsmax,controlword
      parameter (apbinsmax=2048)
      common/header/ period,samint,scantime,date,startime,arecnum,nscan,
     .aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .admptim,admplen,admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc,
     .alensbc,apbins,adm,arm,abintot,admptot,abtwscn,scaleI,sigmaI,
     .abbm3lo,abbm4lo,baseval,anrecscn,rm,erm,aver,source,controlword,
     .nwins,nwbins(5),wndows,sver,nseq
      common/iquv/ idata(apbinsmax),qdata(apbinsmax),udata(apbinsmax),
     .             vdata(apbinsmax)
      common/lchi/ ldata(apbinsmax),chidata(2*apbinsmax)
      real idata,qdata,udata,vdata,ldata,chidata
      real abandwd,apos1st,apos2st,aazst,za,azaend,aclkrat,
     .obfreq,adm,arm,abintot,abtwscn,scaleI,sigmaI,abbm3lo,
     .abbm4lo,baseval,rm,erm
      real*8 period,samint,scantime
      character*3 aver,sver
      character*9 source,label
      character wndows*5
      logical convert
c
      read(41,101,err=201,end=120) aver,date,startime,source,
     .arecnum,nscan,aendscan,abandwd,aposcrd,apos1st,apos2st,aazst,sver
 101  format(a3,2i5,a9,i4,1x,i7,i5,e9.2,i2,3f9.5,a3)
      read(41,102,err=202,end=300) za,azaend,aclkrat,admptim,admplen,
     .admpsrd,ainttim,aextst,obfreq,apoladd,anumsbc
 102  format(3f9.5,4i7,i7,f12.5,2i2)
      read(41,103,err=203,end=300) alensbc,apbins,period,adm,arm,
     .samint,admptot,abtwscn
 103  format(2i5,f18.15,2f9.2,f9.7,i7,e11.4,1x)
      read(41,104,err=204,end=300) scaleI,sigmaI,nwins,(nwbins(i),i=1,5)
     .,baseval,wndows,anrecscn,scantime,nseq,controlword
 104  format(2f9.5,i3,5i3,e12.5,a5,i2,f13.1,i6,1x,o5)
c
c     read I.
       read(41,106,err=205,end=300) (idata(i),i=1,apbins)
c
c     read Q.
       read(41,106,err=206,end=300) (qdata(i),i=1,apbins)
c
c     read U.
       read(41,106,err=207,end=300) (udata(i),i=1,apbins)
c
c     read V.
       read(41,106,err=208,end=300) (vdata(i),i=1,apbins)
 106  format((8f10.5))
c
c     compute the averaged position angle and linear polarization
      if(convert) then
        do i=1,apbins
          chidata(i)=0.5*atan2(udata(i),qdata(i))*(180./3.14159265)
          ldata(i)=sqrt(qdata(i)**2+udata(i)**2)
        end do
      end if
c
      istat=0
      return
c
 120  istat=-1
      return
c
 201  write(*,*) "Readscan: header line 1 read error",nscan,arecnum
      stop
 202  write(*,*) "Readscan: header line 2 read error",nscan,arecnum
      stop
 203  write(*,*) "Readscan: header line 3 read error",nscan,arecnum
      stop
 204  write(*,*) "Readscan: header line 4 read error",nscan,arecnum
      stop
 205  write(*,*) "Readscan: Stokes I data read error",nscan,arecnum
      stop
 206  write(*,*) "Readscan: Stokes Q data read error",nscan,arecnum
      stop
 207  write(*,*) "Readscan: Stokes U data read error",nscan,arecnum
      stop
 208  write(*,*) "Readscan: Stokes V data read error",nscan,arecnum
      stop
c
 300  istat=2
      stop "Readscan: Unexpected EOF/EOT on input"
      end
