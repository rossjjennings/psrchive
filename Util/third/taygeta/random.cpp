// random.cpp  		A collection of Random number generators

//                      (c) Copyright 1995, Everett F. Carter Jr.
//                      Permission is granted by the author to use
//			this software for any application provided this
//			copyright notice is preserved.

#include <math.h>

#include <random.hpp>
#include <Error.h>

/* uniform [a, b] random variate generator */
Real RUniform::number(const Real a,const Real b)
{
	if ( a > b )
        {
                throw Error(FailedCall, "RUniform::number",
                            "Argument Error: a > b");
	}

	return( a + (b - a) * ranf() );

}

/* random integer generator, uniform */
int IUniform::number(const int i,const int n)
{				/* return an integer in i, i+1, ... n */
	if ( i > n )
        {
                throw Error(FailedCall, "IUniform::number",
		            "Argument Error: i > n");
	}

	return (int)(i + (rani() % (n - i + 1)) );
}


/* negative exponential random variate generator */
Real Expntl::number(const Real x)
{
	return( -x * log( ranf() ) );
}


/* erlang random variate generator */
Real Erlang::number(const Real x,const Real s)
{
	int i, k;
	Real z;

	if ( s > x )
	{
		throw Error(FailedCall, "Erlang::number",
		            "Argument Error: s > x");
	}

	z = x / s;
	k = (int) (z * z);
	for (i = 0, z = 1.0; i < k; i++)
				z *= ranf();
	return( - (x / k) * log( z ) );

}

/* hyperexponential random variate generator */
Real Hyperx::number(const Real x,const Real s)
{
	Real cv, z, p;

	if ( s <= x )
	{
	        throw Error(FailedCall, "Hyperx::number",
		            "Argument Error: s not > x");
	}

	cv = s / x;	z = cv * cv;
	p = 0.5 * (1.0 - sqrt( (z - 1.0) / (z + 1.0) ) );
	z = ( ranf() > p ) ? (x / (1.0 - p) ) : (x / p);

	return( -0.5 * z * log( ranf() ) );
	
}

/* normal random variate generator */
Real Normal::number(const Real x,const Real s)	/* mean x, standard deviation s */
{
	Real v1, v2, w, z1;

	if (use_z2 && z2 != 0.0)		/* use value from previous call */
	{
		z1 = z2;
		z2 = 0.0;
	}
	else
	{
		do {
			v1 = 2.0 * ranf() - 1.0;
			v2 = 2.0 * ranf() - 1.0;
			w = v1 * v1 + v2 * v2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		z1 = v1 * w;
		z2 = v2 * w;
	}

	return( x + z1 * s );
}





