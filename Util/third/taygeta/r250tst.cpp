// r250test.cpp

#include <iostream>

#include <r250.hpp>
//#include <rndlcg.hpp>

#define TEST1
// #define TEST2

using namespace std;

int main()
{
	R250 rand( 1 );
//	RandLCG rand( 1 );
	long int val;

#ifdef TEST1
	for (int k = 0; k < 10000; k++)
		val = rand.rani();

	cout << "final value is " << val << '\t';

	long int expect = 267771767;

	if (val != expect) {
	  cout << "error: should be 267771767\n";
	  return -1;
	}

#endif

#ifdef TEST2                 // prints out two columns for a scatter plot
	for (int k = 0; k < 2000; k++)
		cout << rand.rani() << '\t' << rand.rani() << '\n';
#endif

	return 0;
	
}

