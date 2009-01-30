

#define PI 3.14159265358
#define c0 299.792458




class KaraFit
{

public:

   KaraFit();
   ~KaraFit();
   
   
   void passData(vector<double> _pas, 
                 vector<double> _errs, 
	        vector<double> _freqs,
            double _rm1, 
	   double _rm2, 
	  double _drm,
         double _pa1, 
	double _pa2, 
       double _dpa)
   {

     rm1 = _rm1; rm2 = _rm2;
     drm = _drm; dpa = _dpa;
     pa1 = _pa1; pa2 = _pa2;

     pas = _pas;
     errs = _errs;
     freqs = _freqs;
     
   }
 
   int doFit();

   vector<double> fitValues;


private:


   void fitResult();



   double r2d(){return 180.0/PI;}
   double d2r(){return PI/180.0;}

   double rm;
   double drm;
   double pa;
   double dpa;
   
   double rm1,rm2;
   double pa1,pa2;
   
   double fu;
   double xmin,xmax;
   double expo;
   
   double prob;
   double intstep;
   
   double probmin,probmax;
   double bestrm,bestpa;
   double sigpa;
   
   vector<double> freqs;
   vector<double> pas;
   vector<double> errs;


}; 
