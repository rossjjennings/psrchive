#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Reference.h"

int main (int argc, char** argv)
{ try {

  // Pulsar::Archive::verbose = true;
  // Error::verbose = true;
  // Pulsar::Integration::verbose = true;
  
  Reference::To<Pulsar::Archive> archive1;
  Reference::To<Pulsar::Archive> archive2;
  Reference::To<Pulsar::Archive> diff_Archive;  
  
  
  string filename1;
  string filename2;
 
  if (argc == 1) {
   cerr << "test_Difference <archive1> <archive2>" << endl;
   exit(EXIT_SUCCESS);
  } 

  if (argc>1) {
    filename1 = argv[1];
    cerr << "load archive from " << filename1 << endl;
    archive1 = Pulsar::Archive::load (filename1);
  }
  if (argc>2) {
    filename2 = argv[2];
    cerr << "load archive from " << filename2 << endl;
    archive2 = Pulsar::Archive::load (filename2);
  }

  archive1 -> dedisperse();
  archive1 -> centre();

  archive2 -> dedisperse();
  archive2 -> centre();

  Pulsar::Profile *profile1;
  Pulsar::Profile *profile2;
  
  float max_val1=0;
  float max_val2=0; 

  float * amps1 = NULL;
  float * amps2 = NULL;


  for (int nchan = 0; nchan < archive2->get_nchan(); nchan++) {
      
      profile1 = archive1->get_Profile(0,0,nchan);
      profile2 = archive2->get_Profile(0,0,nchan);
      max_val1 = profile1 -> max(0,profile1->get_nbin());
      max_val2 = profile2 -> max(0,profile2->get_nbin());
      cerr << "Before: " << max_val1 << ":" << max_val2 << endl;
          
      profile2->operator*=(float (max_val1/max_val2)); 
      
      amps1 = profile1->get_amps();
      amps2 = profile2->get_amps();
 
      max_val1 = profile1 -> max(0,profile1->get_nbin());
      max_val2 = profile2 -> max(0,profile2->get_nbin());
      cerr << "During: " << max_val1 << ":" << max_val2 << endl;

     
      for (unsigned ibin=0; ibin<profile1->get_nbin(); ibin++) {
        cout << *amps1 << " " << *amps2;
	*amps1 = ( (double)*amps1 - (double)*amps2 );
        cout << " " << *amps1 << endl;
	
	amps1 ++; amps2 ++;
      }
      // profile1->operator-=(profile2);      
   
      max_val1 = profile1 -> max(0,profile1->get_nbin());
      max_val2 = profile2 -> max(0,profile2->get_nbin());
      cerr << "After: " << max_val1 << ":" << max_val2 << endl;

  } 
  
  archive1 -> unload("difference.ar");
  
}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
catch (string& error) {
  cerr << error << endl;
  return -1;
}
 
 return 0;
}
