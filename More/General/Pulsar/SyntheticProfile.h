#ifndef __Synthetic_Profile_h
#define __Synthetic_Profile_h

#include <iostream>
#include <vector>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/box-muller.h"
#include "Pulsar/Calculator.h"

class SyntheticProfile {
  
 public:
  
  static bool verbose; 

  bool constructed;
  bool built_from_template;
  
  void clear();

  void add_noise    ();
  void basic_init   ();
  void generate     ();
  void integrate    ();
  void integrate    (int numtimes);
  void build        (Pulsar::Profile* _template = NULL);

  void set_height   (double _height)    { height = _height; }
  void set_width    (double _width)     { width = _width; }
  void set_centroid (double _centroid)  { centroid = _centroid; }
  void set_nbins    (int _nbins);

  void set_denfrac  (int _frac)         { denoise_frac = _frac; }

  double get_height   () { return height; }
  double get_width    () { return width; }
  double get_centroid () { return centroid; }

  float  get_true_snr ();
  int    get_nbins    () { return nbins; }

  void true_rotate (double phase);

  Pulsar::Profile get_Profile();

  vector<float> get_amps  ();
  vector<float> get_noise ();

  void set_noise (vector<float> &amps);
  void set_amps  (vector<float> &amps);
  
  // Constructors and Destructors
  
  SyntheticProfile(Pulsar::Profile *_template = NULL);

  SyntheticProfile(int _nbins, double _height, 
	       double  _width, double _centroid);
  
  ~SyntheticProfile();
  
 protected:

  bmrng my_random;

  vector<float> xvals;
  vector<float> profvals;
  vector<float> noise;

  double height;
  double width;
  double centroid;

  int nbins;
  int denoise_frac;

  Pulsar::Profile theprofile;  
};

#endif
