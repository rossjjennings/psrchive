/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>
#include <string>

#include <fitsio.h>
#include "genutil.h"
#include "FITSError.h"

using namespace std;

void test_template (const char* template_file, bool populate = true, 
		    bool verbose = true);
void parse_template (const char* template_file, bool verbose = true);

int main (int argc, char** argv) try {

  unsigned nloops = 100;

  string filename;

  char* srcdir = getenv ("srcdir");
  if (srcdir)
    filename = string(srcdir) + "/";
  
  filename += "psrheader.fits";

  cerr << "Parsing " << filename << endl;
  parse_template (filename.c_str());

  cerr << "Creating " << nloops << " files from template" << endl;
  for (unsigned iloop=0; iloop<nloops; iloop++)
    test_template (filename.c_str(), true, false);

  cerr << "Test passed" << endl;
  return 0;

}
catch (Error& error) {
  
  cerr << error << endl;
  return -1;
  
}

void check_length (const char* str, const char* name)
{
  if (strlen(str) >= FLEN_CARD)
    throw Error (InvalidState, "check_length",
		 "%s length=%d >= FLEN_CARD=%d\n\n%s",
		 name, strlen(str), FLEN_CARD, str);
}

void parse_template (const char* template_file, bool verbose)
{

  FILE* fptr = fopen (template_file, "r");
  if (!fptr)
    throw Error (FailedSys, "parse_template", "fopen(%s)", template_file);

  char templt [FLEN_CARD];
  char card   [FLEN_CARD];

  int status = 0;
  
  while (fgets (templt, FLEN_CARD*2, fptr)) {

    // CFITSIO User's Reference Guide
    // 11.1 Detailed Template Line Format

    /* "Any template line that begins with the pound '#' character is
       ignored by the template parser and may be use to insert
       comments into the template file itself." */

    if (templt[0] == '#')
      continue;

    char* newline = strchr (templt, '\n');
    if (newline)
      *newline = '\0';

    int keytype = 0;
    fits_parse_template (templt, card, &keytype, &status);
    if (status)
      throw FITSError (status, "parse_template",
		       "fits_parse_template (%s)", templt);      

    check_length (templt, "card");

    // cerr << "T:" << templt << endl << "C:" << card << endl;
    
    fits_test_record (card, &status);
    if (status)
      throw FITSError (status, "parse_template",
		       "fits_test_record (%s)", card);      
    
    
    char keyname[FLEN_CARD];
    int keylength = 0;
    fits_get_keyname (card, keyname, &keylength, &status);
    if (status)
      throw FITSError (status, "parse_template",
		       "fits_get_keyname (%s)", card);      
    
    if (keyname[strlen(keyname)-1]=='#')
      keyname[strlen(keyname)-1] = '\0'; // ignore auto-indexing marks

    fits_test_keyword (keyname, &status);
    if (status)
      throw FITSError (status, "parse_template",
		       "fits_test_keyname (%s)", keyname);
    


  }
  
  fclose (fptr);

}


void test_template (const char* template_file, bool populate, bool verbose)
{
  char* filename = "!test_template.psrfits";

  if (verbose)
    cerr << "test_template call fits_create_template (" << filename << ", " 
	 << template_file << ")" << endl;

  fitsfile* fptr = 0;
  int status = 0;

  fits_create_file (&fptr, filename, &status);
  if (status)
    throw FITSError (status, "test_template", "fits_create_file");

  fits_execute_template (fptr, const_cast<char*>(template_file), &status);
  if (status)
    throw FITSError (status, "test_template",
		     "fits_execute_template (%s)", template_file);

  if (populate)
  {
    fits_movabs_hdu (fptr, 1, 0, &status);
    if (status)
      throw FITSError (status, "test_template",
		       "fits_moveabs_hdu");
    // populate some tables
    fits_movnam_hdu (fptr, BINARY_TBL, "SUBINT", 0, &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_movnam_hdu SUBINT");
    int npol = 2, nbin=3, nchan=4;
    int16 data[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
    int colnum = 0;
    fits_get_colnum (fptr, CASEINSEN, "DATA", &colnum, &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_get_colnum DATA");
    fits_modify_vector_len (fptr, colnum, nchan*npol*nbin, &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_modify_vector_len DATA");
    fits_write_col (fptr, TSHORT, colnum, 1, 1, npol*nbin*nchan, 
		    data, &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_write_col DATA");
    fits_write_col (fptr, TSHORT, colnum, 2, 1, npol*nbin*nchan, 
		    data, &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_write_col DATA");
    fits_write_col (fptr, TSHORT, colnum, 3, 1, npol*nbin*nchan, 
		    data, &status); // 3 subints
    if (status != 0)
      throw FITSError (status, "test_template", "fits_write_col DATA");

#if 1
    // and a tempo2 predictor bit of text
    char *predictor="ChebyModelSet 1 segments\n"
      "ChebyModel BEGIN\n"
      "PSRNAME J1906+0746\n"
      "SITENAME pks\n"
      "TIME_RANGE 52999.73100000000000000000000000021 52999.77100000000000000000000000024\n"
      "FREQ_RANGE 600 700\n"
      "DISPERSION_CONSTANT -6272008.470365761865252891275788158\n"
      "NCOEFF_TIME 4\n"
      "NCOEFF_FREQ 3\n"
      "COEFFS 277860797.9339920026837258594028233 -5.220161217568439556184814296811367e-07 3.007171084412236395612484491876061e-08\n"
      "COEFFS 23987.68472248130594267099310411194 6.495268167732337239776991618520845e-07 -3.741726696168595133440611759046334e-08\n"
      "COEFFS 0.0008448324536128677846111604043291928 -1.715585520448988683531246072839005e-08 9.882946101307895601123045482021159e-10\n"
      "COEFFS -1.488687694655296405712529028194112e-05 -3.397481491206875835762687894958933e-10 1.957116344031319920601399137776413e-11\n"
      "ChebyModel END\n";
    fits_movnam_hdu (fptr, BINARY_TBL, "T2PREDICTOR", 0, &status);
    
    if (status != 0) 
      throw FITSError (status, "test_template", 
		       "fits_movnam_hdu T2PREDICTOR");
    fits_get_colnum (fptr, CASEINSEN, "PREDICTOR", &colnum, &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_get_colnum PREDICTOR");
    fits_modify_vector_len (fptr, colnum, strlen(predictor), &status);
    if (status != 0)
      throw FITSError (status, "test_template", "fits_modify_vector_len PREDICTOR"); 
    fits_write_col (fptr, TSTRING, colnum, 1, 1, 1, 
		    &predictor, &status);  
    if (status != 0)
      throw FITSError (status, "test_template", "fits_write_col PREDICTOR");
#endif
  }

  fits_close_file (fptr, &status);
  if (status)
    throw FITSError (status, "test_template", "fits_close_file");

}
