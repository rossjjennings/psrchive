#ifndef __SyntheticProfile
#define __SyntheticProfile

#include <iostream>
#include <fstream>
#include <vector>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/box-muller.h"
#include "Pulsar/Calculator.h"

class SyntheticProfile {
  
 public:
  
  static bool verbose; 
  static int denoise_frac;
  static unsigned randsize;

  bool constructed;
  bool built_from_template;
  
  vector<float> xvals;
  vector<float> profvals;
  vector<float> random;
  vector<float> noise;
  
  void add_noise();

  void set_height(double _height)     { height = _height; }
  void set_width(double _width)       { width = _width; }
  void set_centroid(double _centroid) { centroid = _centroid; }
  void set_nbins(int _nbins)          { nbins = _nbins; }

  void clear();

  double get_height()   { return height; }
  double get_width()    { return width; }
  double get_centroid() { return centroid; }
  int get_nbins()       { return nbins; }
  
  float get_true_snr();
  
  void build(Pulsar::Profile* _template = NULL);
  void initialise_rand();      
  void generate();
  void integrate();
  void integrate(int numtimes);
  void true_rotate (float phase);

  Pulsar::Profile get_Profile();

  vector<float> get_amps();
  vector<float> get_noise();
  
  void set_noise(vector<float> &amps);
  void set_amps(vector<float> &amps);
  
  // Constructors and Destructors
  
  SyntheticProfile(Pulsar::Profile *_template = NULL);

  SyntheticProfile(int _nbins, double _height, 
	       double  _width, double _centroid);
  
  ~SyntheticProfile();
  
 protected:

  bmrng my_random;

  double height;
  double width;
  double centroid;

  int nbins;

  Pulsar::Profile theprofile;

  int last_randindex;
  
};

#endif
