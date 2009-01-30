/***************************************************************************
 *
 *   Copyright (C) 2008 by Aris Noutsos
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Config.h"

#include "4D_interpol.h"

#include <vector>
#include <string>
#include <fstream>

using namespace std;

static Pulsar::Option<string> default_pa_error_filename 
(
 "rmfit::pa_error",
 Pulsar::Config::get_runtime() + "/pa_error_matrix.asc",

 "nkjj08 PA error matrix filename",

 "The name of the file containing the position angle error matrix \n"
 "used in the njkk08 additions to rmfit."
);

#define PI 3.14159265358
#define c0 299.792458

float lookup_PA_err(float &xint)
{
  
  float x0,x1;
  vector<float> x0s;
  vector<float> x1s;
  
  string filename = default_pa_error_filename;

  ifstream data2D ( filename.c_str() );

//////////// Read in table ////////////

  while(1){
    
    data2D >> x0 >> x1;
    
    if (data2D.eof()) break;
       
    x0s.push_back(x0);
    x1s.push_back(x1);
        
  }

  data2D.close();

  float* x0a = new float[x0s.size()+1];
  float* x1a = new float[x0s.size()+1];

  for (unsigned iv=0; iv<x0s.size(); iv++)
  {  
    x0a[iv+1]=x0s[iv];
    x1a[iv+1]=x1s[iv];
  }  
  
  float* y2a = new float[x0s.size()+1];
  float yp1 = -0.523;
  float ypn = 0.;

  float g = 0.;

  spline(x0a,x1a,x0s.size(),yp1,ypn,y2a);  

  splint(x0a,x1a,y2a,x0s.size(),xint,&g);

  return g;

}

static Pulsar::Option<string> default_rm_error_filename 
(
 "rmfit::rm_error",
 Pulsar::Config::get_runtime() + "/2D_data.asc",

 "nkjj08 RM error matrix filename",

 "The name of the file containing the rotation measure error matrix \n"
 "used in the njkk08 additions to rmfit."
);

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
float lookup_RM_err(vector<float>& xint)
{
  
  float x0,x1,f;
  
  vector<float> x0s;
  vector<float> x1s;
  vector<float> fs;

  string filename = default_rm_error_filename;

  ifstream data2D (filename.c_str());

  //////////// Read in table ////////////

  while(1)
  {
    data2D >> x0 >> x1 >> f;
    
    if (data2D.eof()) break;
       
    x0s.push_back(x0);
    x1s.push_back(x1);

    fs.push_back(f);
  }

  data2D.close();



////////// Copy everything into 2D vector x ///////////

  vector<vector<float> > x;
  
  x.push_back(x0s);
  x.push_back(x1s);


/////////  Convert x into sorted vector xa /////////
  
  vector<float> xarg; //1D vector for the contents of each column 
  vector<vector<float> > xa;
  
  for (unsigned i=0; i<x.size(); i++)
  {
    xarg.clear();
    xarg.push_back(x[i][0]);
    for (unsigned j=1; j<x[i].size(); j++)
    {
      if(x[i][j]<x[i][j-1]) break;
      if(x[i][j]>x[i][j-1])
        xarg.push_back(x[i][j]);
    }
  
    xa.push_back(xarg);
  }

  


///////// Interpolate //////////////////////  

  float** f2D = Make2DArray(xa[0].size()+1, xa[1].size()+1);
  
  
  int counter=0;
  for (unsigned i=1; i<=xa[0].size(); i++)
    for (unsigned j=1; j<=xa[1].size(); j++,counter++)
      f2D[i][j] = fs[counter];

  
  float* x0a = new float[xa[0].size()+1];
  float* x1a = new float[xa[1].size()+1];

  for (unsigned i=0; i<xa[0].size(); i++) x0a[i+1] = xa[0][i];
  for (unsigned i=0; i<xa[1].size(); i++) x1a[i+1] = xa[1][i];
  
  
  float** y2a = Make2DArray(xa[0].size()+1, xa[1].size()+1);
  
  splie2(x0a, x1a, f2D, xa[0].size(), xa[1].size(), y2a);

  float g = 0.;
  
  splin2(x0a, x1a, f2D, y2a, xa[0].size(), xa[1].size(), xint[0], xint[1], &g);

  return g;

}
