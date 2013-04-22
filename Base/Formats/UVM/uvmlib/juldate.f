      SUBROUTINE JuLDATE(IC,MONTH,DAY,YEAR,DATE)
C
C     PURPOSE
C     TO CONVERT GREGORIAN DATES TO MODIFIED JULIAN DATES AND
C     MODIFIED JULIAN DATES (MJD = JD - 2440000.) TO GREGORIAN
C
C     FORTRAN USAGE
C     CALL JLDATE(IC,MONTH,DAY,YEAR,DATE)
C     IC   -=0 IF CONVERSION IS GREGORIAN TO JULIAN
C           =1 IF CONVERSION IS JULIAN TO GREGORIAN
C     MONTH- MONTH(INTEGER)
C     DAY  - DAY(INTEGER)
C     YEAR - YEAR(INTEGER)
C     DATE - JULIAN DATE(FLOATING POINT)
C
c     IMPLICIT UNDEFINED(A-Z)
      INTEGER MTHDAY(12)                                                  
      INTEGER DAY,YEAR,IC,ND,MONTH,I
      REAL*8 DATE
      DATA MTHDAY/31,28,31,30,31,30,31,31,30,31,30,31/
      IF(IC)1,1,6
    1 ND=DAY+365.25*YEAR
      IF(YEAR/4*4-YEAR)4,2,4
    2 IF(MONTH.LE.2)ND=ND-1
    4 DO 5 I=2,MONTH
    5 ND=ND+MTHDAY(I-1)
      DATE=ND-718955.5
      RETURN
C  THE NEXT LINE INSERTED OCT 78 TO IMPROVE PRECISION TO A FEW MS
C     DATE=IDINT(DATE)+0.5
    6 ND=DATE+718955.5
      YEAR=ND/365.25
      ND = ND - INT(365.25*YEAR)
      IF(YEAR/4*4-YEAR)10,7,10
7     IF(ND-59)9,8,10
8     MONTH = 2
      DAY = 29
      RETURN
9     ND = ND + 1                                                  
10    DO 11 I = 1,12                                                   
      IF(ND.LE.MTHDAY(I)) GO TO 12                                                 
11    ND = ND - MTHDAY(I)                                                   
      I = 12                                                   
12    MONTH = I                                                   
      DAY = ND                                                   
      RETURN                                                   
      END                                                   
