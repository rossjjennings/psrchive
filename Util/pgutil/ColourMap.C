#include "ColourMap.h"
#include <cpgplot.h>

//! Default constructor
pgplot::ColourMap::ColourMap ()
{
  name = GreyScale;
  contrast = 1.0;
  brightness = 0.5;
}

//! Set the contrast (0 to 1; normally 1)
void pgplot::ColourMap::set_contrast (float _contrast)
{
  if (contrast == _contrast)
    return;

  contrast = _contrast;
  set_colour_map (name);
}


//! Set the brightness (0 to 1; normally 5)
void pgplot::ColourMap::set_brightness (float _brightness)
{
  if (brightness == _brightness)
    return;

  brightness = _brightness;
  set_colour_map (name);
}


//! Set the colour map name
void pgplot::ColourMap::set_colour_map (Name name)
{
  switch (name) {
    
  case GreyScale: {
    
    float grey_l[] = { 0.0, 1.};
    float grey_r[] = { 0.0, 1.};
    float grey_g[] = { 0.0, 1.};
    float grey_b[] = { 0.0, 1.};
    
    cpgctab (grey_l, grey_r, grey_g, grey_b, 2, contrast, brightness);
    
    break;
  }
  
  case Inverse: {
    
    float grey_l[] = { 0.0, 1.0};
    float grey_r[] = { 1.0, 0.0};
    float grey_g[] = { 1.0, 0.0};
    float grey_b[] = { 1.0, 0.0};
    
    cpgctab (grey_l, grey_r, grey_g, grey_b, 2, contrast, brightness);
    
    break;
  }

  case Heat: {
    
    float heat_l[] = {0.0, 0.2, 0.4, 0.6, 1.0};
    float heat_r[] = {0.0, 0.5, 1.0, 1.0, 1.0};
    float heat_g[] = {0.0, 0.0, 0.5, 1.0, 1.0};
    float heat_b[] = {0.0, 0.0, 0.0, 0.3, 1.0};
    
    cpgctab (heat_l, heat_r, heat_g, heat_b, 5, contrast, brightness);
    
    break;
  }
  
  case Cold: {
    
    float cool_l[] = {0.0, 0.2, 0.4, 0.6, 1.0};
    float cool_r[] = {0.0, 0.0, 0.0, 0.3, 1.0};
    float cool_g[] = {0.0, 0.0, 0.5, 1.0, 1.0};
    float cool_b[] = {0.0, 0.5, 1.0, 1.0, 1.0};
    
    cpgctab (cool_l, cool_r, cool_g, cool_b, 5, contrast, brightness);
    
    break;
  }
  
  case Plasma: {
    
    float cool_l[] = {0.0, 0.2, 0.4, 0.6, 1.0};
    float cool_r[] = {0.0, 0.0, 0.5, 1.0, 1.0};
    float cool_g[] = {0.0, 0.0, 0.0, 0.3, 1.0};
    float cool_b[] = {0.0, 0.5, 1.0, 1.0, 1.0};
    
    cpgctab (cool_l, cool_r, cool_g, cool_b, 5, contrast, brightness);
    
    break;
  }
  
  case Forest: {
    
    float cool_l[] = {0.0, 0.4, 0.7, 0.9, 1.0};
    float cool_r[] = {0.0, 0.0, 0.0, 0.3, 1.0};
    float cool_g[] = {0.0, 0.5, 1.0, 1.0, 1.0};
    float cool_b[] = {0.0, 0.0, 0.5, 1.0, 1.0};

    cpgctab (cool_l, cool_r, cool_g, cool_b, 5, contrast, brightness);
    
    break;
  }
  
  case AlienGlow: {
    
    float test_l[] = { 0.0, 0.1, 0.1, 0.2, 0.2, 0.3, 0.3, 0.4, 0.4, 0.5,
		       0.5, 0.6, 0.6, 0.7, 0.7, 0.8, 0.8, 0.9, 0.9, 1.};
    float test_r[] = { 0.0, 0.0, 0.3, 0.3, 0.5, 0.5, 0.0, 0.0, 0.0, 0.0,
		       0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.};
    float test_g[] = { 0.0, 0.0, 0.3, 0.3, 0.0, 0.0, 0.0, 0.0, 0.8, 0.8,
		       0.6, 0.6, 1.0, 1.0, 1.0, 1.0, 0.8, 0.8, 0.0, 0.};
    float test_b[] = { 0.0, 0.0, 0.3, 0.3, 0.7, 0.7, 0.7, 0.7, 0.9, 0.9,
		       0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.};
    
    cpgctab (test_l, test_r, test_g, test_b, 20, contrast, brightness);
    
    break;
  }
  
  case Test: {
    
    float test_l[] = { -0.5, 0.0, 0.17, 0.33, 0.50, 0.67, 0.83, 1.0, 1.7};
    float test_r[] = { 0.0, 0.0,  0.0,  0.0,  0.6,  1.0,  1.0, 1.0, 1.};
    float test_g[] = { 0.0, 0.0,  0.0,  1.0,  1.0,  1.0,  0.6, 0.0, 1.};
    float test_b[] = { 0.0, 0.3,  0.8,  1.0,  0.3,  0.0,  0.0, 0.0, 1.};
    
    cpgctab (test_l, test_r, test_g, test_b, 9, contrast, brightness);
    
    break;
  }
  
  } // end switch (name)
}
