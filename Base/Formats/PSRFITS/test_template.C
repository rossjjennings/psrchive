#include <iostream>
#include <fitsio.h>

#include "FITSError.h"

void test_template (const char* template_file, bool verbose = true);
void parse_template (const char* template_file, bool verbose = true);


int main (int argc, char** argv) try {

  unsigned nloops = 100;
  
  char* template_file = getenv ("PSRFITSDEFN");
  if (!template_file)  {
    cerr << "test_template: PSRFITSDEFN not defined" << endl;
    return 0;
  }

  cerr << "Parsing " << template_file << endl;
  parse_template (template_file);

  cerr << "Creating " << nloops << " files from template" << endl;
  for (unsigned iloop=0; iloop<nloops; iloop++)
    test_template (template_file, false);

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
    
    
    fits_test_keyword (keyname, &status);
    if (status)
      throw FITSError (status, "parse_template",
		       "fits_test_keyname (%s)", keyname);
    


  }
  
  fclose (fptr);

}


void test_template (const char* template_file, bool verbose)
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

  fits_close_file (fptr, &status);
  if (status)
    throw FITSError (status, "test_template", "fits_close_file");

}
