/***********************************************************************/
/* Routine to read Harris data from tape or disk. The Harris header is */
/* read first and the length of the data part of a record is determined*/
/* from header(112). After that the remaining bytes of the original    */
/* record are read in. (AW -- May 1990)                                */
/***********************************************************************/

# include <stdio.h>
# include <sys/types.h>
# include <unistd.h>
# define HEADER_LENGTH 336

skipbytes_(header,nhdr,fileid)

char *header;
int *fileid, nhdr;

{
      int fd, nbytes, i, recl, data_length, temp[3];
      char head[HEADER_LENGTH];
      fd = *fileid;
      nbytes = read(fd,head,HEADER_LENGTH);
      if(nbytes > 0)
               {
                 for(i = 0; i < HEADER_LENGTH; i++)
                   {
                     *header = head[i];
                     *header++;
                   }
                 for(i = 0; i < 3; i++)
                   temp[i] = head[i+333] & 0377;
                 recl = (temp[0] << 16) | (temp[1] << 8) | temp[2];
                 data_length = recl * 3 - HEADER_LENGTH;
                 nbytes = lseek(fd, data_length, SEEK_CUR);
               }
      else
               {
                 printf("Hit EOF on input...\n");
                 exit(0);
               }
      return(0);
}
