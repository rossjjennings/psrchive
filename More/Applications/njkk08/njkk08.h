#include <cpgplot.h>
#include <fstream>
#include <iostream>
#include <vector>

float plotpa0,plotRM,plotRM_err;

vector<double> goodfreqs;
vector<double> goodpa;
vector<double> goodpa_stddev;

vector<double> goodi;
vector<double> goodirms;
vector<double> goodv;
vector<double> goodvrms;
vector<double> goodq;
vector<double> goodqrms;
vector<double> goodu;
vector<double> goodurms;

vector<double> goodl;
vector<double> goodlrms;


vector<double> delta_V;
vector<double> delta_Verr;
vector<double> delta_L;
vector<double> delta_Lerr;


void set_plotparams(float _plotpa0,float _plotRM,float _plotRM_err){
 
 plotpa0=_plotpa0;
 plotRM=_plotRM;
 plotRM_err=_plotRM_err;

}

vector<float> fbscr_RMs;
vector<float> fbscr_RM_errs;
vector<float> fbscr_RM_probmax;
//vector<float> fbscr_RM_chisqmin;

float best_fbscr_probmax;
//float best_fbscr_chisqmin;
float best_fbscr_RM;
float best_fbscr_RM_err;

bool outofrange_err;
bool zero_err;
bool anomalous_err;
 
float lthresh;
//bool good_lthresh;
bool good_fbscrunch;
bool plotv;

float minrm;
float maxrm;
double rmstep;
unsigned rmsteps;



float lookup_RM_err2D(vector<float>& xint);


float lookup_PA_err(float &xint);

float lookup_RM_err(vector<float>& xint);

vector<int> ibin_vec;


void Kasterg_Fit(float& fRM_low,float& fRM_high,float& fRM_step,
                 const vector<double>& freqs,
                 const vector<double>& PAs,
		 const vector<double>& PA_stddevs,
                 double& RM,double& PA0,double& sigRM,double& sigPA0,
		 double& probmax,int& iRM);

