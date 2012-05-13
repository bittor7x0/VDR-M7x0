/*
 * Copyright (c) 2008 open7x0.org
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAXBUF 512

extern int h_errno;

int svdrprecv (int sd) {

  char buf1[MAXBUF] = "";
  char buf2[MAXBUF + 6] = "";
  char tmp[6] = "\n";
  int l = 3;

  struct timeval tv;
  fd_set rfds;

  tv.tv_sec = 1;
  tv.tv_usec = 50000;

  FD_ZERO (&rfds);
  FD_SET (sd, &rfds);

  select (sd + 1, &rfds, NULL, NULL, &tv);

  while (l) {

    if ((recv (sd, buf1, MAXBUF - 1, 4)) < 1) {
      perror ("recv faild");
      return (0);

    } else {

      buf1[strlen (buf1)] = '\0';
      printf ("%s", buf1);

      if ((select (sd + 1, &rfds, NULL, NULL, &tv)) < 1) {
	if (errno != 0) {
	  perror ("select faild");
	  return (0);
	}
      }
     
      /*
       * wow, this is stupid to send "Access Denied!" outside
       * their own format spec. deal with it anyway....
       */

      if (l == 3) {
	if (strncmp (buf1, "Access denied!\n", sizeof (buf1)) == 0) {
	  close (sd);
	  exit (1);
	}
	l = 2;
      }

      strncat (buf2, tmp, sizeof (tmp));
      bzero (tmp, sizeof (tmp));
      strncat (buf2, buf1, sizeof (buf1));

      if (l == 2) {
	int i, j = 0;
	for (i = 0; i < strlen (buf2); i++) {
	  if (buf2[i] == '\n' && isdigit (buf2[1 + i]) && isdigit (buf2[2 + i])
	      && isdigit (buf2[3 + i]) && buf2[4 + i] == ' ') {
		l = 1;
	  }
	  if (i > (strlen (buf2) - 6))
	    tmp[j++] = buf2[i];
	}
      }

      if (l == 1) {
	if (buf1[strlen (buf1) - 1] == '\n' && buf1[strlen (buf1)] == '\0') {
	  //printf("DEBUG MATCH EOT\n");
	  l = 0;
	}
      }

      bzero (buf1, sizeof (buf1));
      bzero (buf2, sizeof (buf2));
    }
  }
  return (1);
}

void svdrpsend (int sd, char *msg) {
  send (sd, msg, strlen (msg), 0);
  svdrprecv (sd);
}

int svdrpconn (int port, char *host) {

  int sd;
  struct hostent *hostaddr;
  struct sockaddr_in socketaddr;

  if (port < 1 || port > 65535) {
    printf ("invalid port number\n");
    return (EINVAL);
  }

  sd = socket (PF_INET, SOCK_STREAM, 6);

  if (sd == -1) {
    perror ("socket faild");
    return (errno);
  }

  memset (&socketaddr, 0, sizeof (socketaddr));

  socketaddr.sin_family = AF_INET;
  socketaddr.sin_port = htons (port);

  hostaddr = gethostbyname (host);

  if (!hostaddr) {
    printf ("gethostbyname: %s\n", hstrerror (h_errno));
    return (h_errno);
  }

  memcpy (&socketaddr.sin_addr, hostaddr->h_addr, hostaddr->h_length);

  if (connect (sd, (struct sockaddr *) &socketaddr, sizeof (socketaddr)) == -1) {
    if (errno != EINPROGRESS) {
      perror ("connect faild");
      return (errno);
    }
  }
  return (sd);
}

int main (int argc, char **argv) {

  if (argc < 4) {
    printf ("usage:  host port \"command [option]\" \n");
    return (EINVAL);
  }

  int port = atoi (argv[2]);
  int sd = svdrpconn (port, argv[1]);

  svdrprecv (sd);
  char *msg = strncat (argv[3], "\n\r", 2);
  svdrpsend (sd, msg);

  svdrpsend (sd, "QUIT\n\r");

  close (sd);
  return (0);
}
