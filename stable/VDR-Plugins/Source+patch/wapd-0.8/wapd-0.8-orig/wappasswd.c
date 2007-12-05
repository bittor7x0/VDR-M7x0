#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  if (argc != 3) exit(EXIT_FAILURE);

  char *sset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
  char salt[3] = "AA";

  srand((int) time((time_t *) NULL));
  salt[0] = sset[rand() & 0x3f];
  salt[1] = sset[(rand() >> 6) & 0x3f];

  printf("%s:%s\n", argv[1], crypt(argv[2],salt));

  exit(EXIT_SUCCESS);
}
