/***************************************************************************
 *
 *   Copyright (C) 1998 by redwards Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* keys2fort.c -- redwards 1/12/97
 *
 * program to parse a keyword definition file and produce Fortran
 * arrays containing the information, for use by ephio.f. Produces
 * file keyinfo.com, based on file keys.dat. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fatal errors ... */
#define FATAL_1(s1) {fprintf(stderr, "Fatal: "); fprintf(stderr, s1); exit(1);}
#define FATAL_2(s1,s2) {fprintf(stderr, "Fatal: "); fprintf(stderr, s1,s2); exit(1);}
#define FATAL_3(s1,s2,s3) {fprintf(stderr, "Fatal: "); fprintf(stderr, s1,s2,s3); exit(1);}

int
main(int argc, char** argv)
{
	FILE *fin, *fout;
	char key[1024][64];
	char line[256];
	int type[1024], err[1024];
	int i, nelem, len, maxlen = 0;

	if (argc < 2)
	  FATAL_1("Must specify path to keys.dat");
	fin = fopen(argv[1], "r");
	if (!fin)
		FATAL_1("Couldn't open keys.dat");
	fout = fopen("keyinfo.com", "w");
	if (!fin)
		FATAL_1("Couldn't open keyinfo.com");

	i = 0;
	while (fgets(line,256,fin)!=NULL)
	{
		if ((sscanf(line,"%s", key[i])==1)	/* non empty line */
		   && (key[i][0]!='#'))	/* ignore comments */
		{
			sscanf(line,"%s %d %d", key[i], type+i, err+i);
			i++;
		}
	}
	nelem = i;

	fprintf(fout, 
		"c This file was generated by keys2fort. Do not modify directly\n\n");
	fprintf(fout, "       integer NUM_KEYS\n");
	fprintf(fout, "       parameter (NUM_KEYS = %d)\n\n", nelem);

/* print out "parameters" for each keyword */
	for (i=0; i < nelem; i++)
	{
		fprintf(fout, "       integer EPH_%s\n", key[i]);
		fprintf(fout, "       parameter (EPH_%s = %d)\n", key[i], i+1);
	}

	for (i=0; i < nelem; i++)
	{
		len = strlen(key[i]);
		if (len > maxlen)
			maxlen = len;
	}

	fprintf(fout, "\n\n       integer maxKeyLen /%d/\n", maxlen+2);

	fprintf(fout, "\n\n       character*%d parmNames(NUM_KEYS)\n", maxlen+2);
	for (i=0; i < nelem; i++)
	{
		fprintf(fout, "       data parmNames(%d) /'%s'/\n", i+1, key[i]);
	}
	fprintf(fout, "\n");
	fprintf(fout, "       integer parmTypes(NUM_KEYS)\n");
	for (i=0; i < nelem; i++)
		fprintf(fout, "       data parmTypes(%d) /%d/\n", i+1, type[i]);
	fprintf(fout, "\n");
	fprintf(fout, "       logical parmError(NUM_KEYS)\n");
	for (i=0; i < nelem; i++)
		if (err[i])
			fprintf(fout, "       data parmError(%d) /.true./\n", i+1);
		else
			fprintf(fout, "       data parmError(%d) /.false./\n", i+1);

	fclose(fout);
	fclose(fin);

	return 0;
}

