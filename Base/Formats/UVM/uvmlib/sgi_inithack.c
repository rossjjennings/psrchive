
/*
 * sample hack to simulate the sun getfd() functionality
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void sgi_inithack_(void);
void sgi_prepfd_(void);
void sgi_getfd_(int *r);

int *fds;                       /* table of fd's */
int nfd;                        /* maximum number of fd's*/

/*
 * initialize the hacks
 */
void sgiinithack_(void)
{
        nfd = getdtablesize();

        /* allocate memory for the tables */

        if ((fds = (int *)malloc(sizeof(int)*nfd)) == (int *)0) {
                fprintf(stderr, "sgi_inithack_: no memory!\n");
                exit(-1);
        }
}

/*
 * prepare for an open call
 */
void sgiprepfd_(void)
{
        int i;

        /*
         * initialize the table
         */
        for (i = 0; i < nfd; i++) {
                int fdflag;

                if (fcntl(i, F_GETFD, &fdflag) < 0)
                        fds[i] = 0; /* invalid FD */
                else
                        fds[i] = 1; /* valid FD */
        }

}

/*
 * find out which fd the new file has
 */
void sgigetfd_(int *r)
{
        int i, fdflag;

        /*
         * search the table again
         */
        for (i = 0; i < nfd; i++) {

                /* test */
                if (fcntl(i, F_GETFD, &fdflag) < 0)
                        continue;

                if (fds[i] == 0) {
                        /* BINGO! */
                        fds[i] = 1;
                        *r = i;
                        return;
                }
        }
}

