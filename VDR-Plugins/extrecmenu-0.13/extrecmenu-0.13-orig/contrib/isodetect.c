/*
 isodetect.c 

 gcc -O2 -Wall isodetect.c -o isodetect

 Looks in first block of an block-device and checks if there is an ISO9660-
 filesystem on it. This is done by checking for a string "CD001".
 
 Also reads certain information out of the block.

 14.11.95 T.Niederreiter  (based on cdlabel.c by aeb)
*/

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>

char buf[1024];
char devname[80];

void printnsp(int begin, int end, char *txt);
void printchar(unsigned char c);

main(int argc, char **argv) {
FILE * infile;
int c;
int id=0;

 /* Default device to check */
     strcpy(devname,"/dev/cdrom");

     while ((c = getopt (argc,argv,"hVpPAd:")) != EOF) {
       switch(c) {
       case 0:
         break;
 
       case 'V':
         id=1;
         break;
 
       case 'p':
        id=2;
         break;
 
       case 'P':
         id=3;
         break;
 
       case 'A':
         id=4;
         break;
 
       case 'd':
         strcpy(devname,optarg);
         break;

       case 'h':
          printf("Usage: %s [OPTIONS]\n",argv[0]);
          printf("\
        -h  show help (this text)\n\
        -V print VOLUME_ID of ISO9660-Image\n\
        -p print PREPARER_ID of ISO9660-Image\n\
        -P print PUBLISHER_ID of ISO9660-Image\n\
        -A print APPLICATION_ID of ISO9660-Image\n\
        -d <device>\n");  
              
         exit(1);
       }
     } 

     infile = fopen(devname, "rb");

     if (infile == NULL) {
         perror(devname);
         exit(1);
     }

     if (lseek(fileno(infile), 32768, SEEK_SET) < 0) {
         perror("lseek");
         exit(1);
     }

     if (read(fileno(infile), buf, sizeof(buf)) != sizeof(buf)) {
         perror("read");
         exit(1);
     }

     if (strncmp(buf, "\001CD001\001", 8) != 0) {
    if (id==0) {
           printf("Not ISO9660\n");
           exit(0);
         }
     }
     else {
        switch(id) { 
         case 0:
           printf("ISO9660\n");
           exit(0);
           break;

         case 1:   
           printnsp(40,72,"Volume_ID");            /*  32 */
           break;

         case 3:
           printnsp(318,446,"Publisher");      /* 128 */
           break;
   
         case 2:
           printnsp(446,574,"Preparer");        /* 128 */
           break;
       
         case 4:
           printnsp(574,702,"Application");    /* 128 */
           break;
       } 
     }
 exit(0); 
}

int empty(char c) {
     return (c == 0 || c == ' ');
}

void printnsp(int begin, int end, char *txt) {
int i,j,k;

     for(i=begin; i<end; i++) {
         if (empty(buf[i]))
           continue;
         for(j=i+1; j<end; j++)
            if (!buf[j] || (j < end-1 
   && empty(buf[j]) && empty(buf[j+1]))) break;
         for(k=i; k<j; k++)
            printchar(buf[k]);
         printf("\n");
         i = j;
     }
}

void printchar(unsigned char c) {
  if (isprint(c) || isspace(c))
        putchar(c);
     else
        printf("\\%03o", c);
}


