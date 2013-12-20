      FUNCTION BESRAT(V)                                                
C ----------------------------------------------------------------
C RETURNS BESRAT = A(K) FOR K = ABS(V), WHERE A(K) IS THE EXPECTED
C MODULUS OF THE MEAN VECTOR SUM OF UNIT VECTORS SAMPLED FROM THE
C VON MISES DISTRIBUTION OF DIRECTIONS IN 2D WITH PARAMETER = K.
C A(V) = THE RATIO OF MODIFIED BESSEL FUNCTIONS OF THE FIRST KIND
C OF ORDERS 1 AND 0, I.E., A(V) = I1(V)/I0(V).
C ----------------------------------------------------------------
C
C  ADJUST TO S DECIMAL DIGIT PRECISION BY SETTING DATA CONSTANTS -
C     C1 = (S+9.0-8.0/S)*0.0351
C     C2 = ((S-5.0)**3/180.0+S-5.0)/10.0
C     CX = S*0.5 + 11.0
C  FOR S IN RANGE (5,14).  THUS FOR S = 9.3 :
      DATA C1 /0.613/, C2 /0.475/, CX /15.65/
C
      Y = 0.0
      X = ABS(V)
      IF (X.GT.CX) GO TO 20
C
C  FOR SMALL X, RATIO = X/(2+X*X/(4+X*X/(6+X*X/(8+ ... )))
      N = INT((X+16.0-16.0/(X+C1+0.75))*C1)
      X = X*0.5
      XX = X*X
      DO 10 J=1,N
        Y = XX/(FLOAT(N-J+2)+Y)
   10 CONTINUE
      BESRAT = X/(1.0+Y)
      RETURN
C
C  FOR LARGE X, RATIO = 1-2/(4X-1-1/(4X/3-2-1/(4X/5-2- ... )))
   20 N = INT((68.0/X+1.0)*C2) + 1
      X = X*4.0
      XX = FLOAT(N*2+1)
      DO 30 J=1,N
        Y = XX/((-2.0-Y)*XX+X)
        XX = XX - 2.0
   30 CONTINUE
      BESRAT = 1.0 - 2.0/(X-1.0-Y)
      RETURN
      END
