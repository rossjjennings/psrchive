#ifndef __EPNIO_H
#define __EPNIO_H

#include "epnhdr.h"
#include "f772c.h"

#ifdef __cplusplus
extern "C" {
#endif

  void F772C(rwepn) (char* filename, int* readwri, int* recno, int* padout, int strlen);

  extern epn_header_line1 F772C(epn1);

  extern epn_header_line2 F772C(epn2);

  extern epn_header_line3 F772C(epn3);

  extern epn_header_line4 F772C(epn4);

  extern epn_header_line5 F772C(epn5);

  extern epn_block_subheader_line1 F772C(epns1);

  extern epn_block_subheader_line2 F772C(epns2);

  extern epn_data_block F772C(dblk);

#ifdef __cplusplus
}
#endif

#endif
