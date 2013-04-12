/***********************************************************************/
/* Routine to read Harris ASCII or binary files created by the P110 or */
/* P467 programs.  This routine gets around the problem of reading a   */
/* long disk file that has no record markers at all.                   */
/* JMR -- February 1995                                                */
/***********************************************************************/

# include <stdio.h>

readbytes_(header,nhdr,data,ndat,fileid)

char *header, *data;
int *fileid, *nhdr, *ndat;

{
      int fd, nbytes, i, recl, data_length, header_length;
      fd = *fileid;
      header_length = *nhdr;
      nbytes = read(fd,header,header_length);
      if(nbytes > 0)
               {
                data_length = *ndat;
                nbytes = read(fd, data, data_length);
                *ndat = nbytes;
               }
      else
               {
                 *ndat = 0;
               }
      return;
}
