/* various endian modification functions for the tid PC control program
 *
 * NOTE: for general use the macros in endian.h are probably the best
 * thing to use since they do the preprocessor stuff. just use one of
 *
 * [to|from][Big|Little]Endian(void *object, int nbytes)
 *
 */
#include "machine_endian.h"
#include <string.h>
#include <stdlib.h>

/* function to change endian-ness of a data, maximum 8 bytes long */
void
changeEndian(void *num, int nbytes)
{
	unsigned char tmp[8];
	unsigned char *numPtr = (unsigned char *)num;
	int i;

	for (i=0; i < nbytes; i++)
		tmp[i] = numPtr[nbytes-i-1];

	memcpy((void *)num, (void *)tmp, nbytes);
}

