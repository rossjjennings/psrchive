//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/extutil.h,v $
   $Revision: 1.1 $
   $Date: 2008/11/27 05:39:17 $
   $Author: straten $ */

#ifndef __Pulsar_get_extension_h
#define __Pulsar_get_extension_h

//! implementation of Archive, Integration, and Profile::get<T> methods
template<class T, class Container>
const T* get_extension (const Container* container,
			const char* method, bool verbose)
{
  const T* extension = 0;

  const unsigned next = container->get_nextension();
  for (unsigned iext=0; iext<next; iext++)
  {
    const typename Container::Extension* ext = container->get_extension (iext);

    if (verbose)
      std::cerr << method << " name="
		<< ext->get_extension_name() << std::endl;
      
    extension = dynamic_cast<const T*>( ext );
      
    if (extension)
      return extension;
  }

  if (verbose)
    std::cerr << method << " failed to find extension type="
	      << typeid(extension).name() << std::endl;

  return extension;
}

template<class T> 
void clean_dangling (vector<T>& data)
{
  for (unsigned i=0; i<data.size(); i++)
    if (!data[i])
      data.erase( data.begin() + i );
}

#endif
