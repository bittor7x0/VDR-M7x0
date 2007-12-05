#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <endian.h>
#include <errno.h>
#include <string.h>
#define INDEXFILENAMEFORMAT "%s/index.vdr"
#define RECORDFILENAMEFORMAT "%s/%03d.vdr"
#define PACKETSIZE 2048
#define MAXMPASTREAMS 0x20
#define MAXAC3STREAMS 0x10
#define MAXFRAMESIZE 524288
#define BUFFERSIZE 524288


struct tIndex { int offset; unsigned char type; unsigned char number; short reserved; };

struct tIndex *readIndexFile(const char *path, int *count)
{
  char fileName[MAXPATHLEN];
  snprintf(fileName, MAXPATHLEN, INDEXFILENAMEFORMAT, path);
  FILE *indexFile = fopen(fileName, "r");

  *count = 0;
  if (!indexFile) {
     fprintf(stderr, "Cannot open %s file\n", fileName);
     return NULL;
     }

  struct tIndex *indices;
  indices = malloc(sizeof(struct tIndex));

  if (!indices) {
     fprintf(stderr, "Cannot alloc memory for index entries\n");
     return NULL;
     }

  while (fread(&indices[*count], sizeof(struct tIndex), 1, indexFile) == 1) {
#if BYTE_ORDER == BIG_ENDIAN
        indices[*count].offset = bswap32(indices[*count].offset);
#endif
        (*count)++;

        struct tIndex *tmp = realloc(indices,sizeof(struct tIndex) * (*count) + 1);
        if (!tmp) {
           fprintf(stderr, "Cannot alloc memory for index entries\n");
           fclose(indexFile);
           return indices;
           }

        indices = tmp;
        }

  fclose(indexFile);
  return indices;
}

int openRecordFile(const char *path, int nr)
{
  char fileName[MAXPATHLEN];
  snprintf(fileName, MAXPATHLEN, RECORDFILENAMEFORMAT, path, nr);
  return open(fileName, O_RDONLY);
}

int checkVideoFrame(unsigned char *buffer, int length, uint32_t *scanner, int *off, int *off2, int *type)
{
  int offset = 0;
  int found = 0;
  int i = 0;
  while (i < length) {
        *scanner <<= 8;
        *scanner |= buffer[i];
        if ((*scanner & 0xFFFFFF00) == 0x00000100) {
           switch (*scanner & 0xFF) {
             case 0xB3:
                  found = 1;
                  offset = i - 3;
                  *off2 = i + 1;
                  break;
             case 0xB8:
                  if (!found) {
                     found = 1;
                     offset = i - 3;
                     }
                  *off2 = i + 1;
                  break;
             case 0x00:
                  if (!found) {
                     found = 1;
                     offset = i - 3;
                     }
                  if (i + 2 < length) {
                     *type = (buffer[i+2] >> 3) & 0x7;
                     }
                  else
                     *type = 8;
                  *off = offset;
                  *off2 = i + 1;
                  return 1;
             }
           }
        i++;
        }

  return 0;
}

const int mpaFrameSizes[2048] = {
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   26,   27,   24,   25,   36,   37,   -1,   -1,
    52,   53,   48,   49,   72,   73,   -1,   -1,   78,   79,   72,   73,  108,  109,   -1,   -1,
   104,  105,   96,   97,  144,  145,   -1,   -1,  130,  131,  120,  121,  180,  181,   -1,   -1,
   156,  157,  144,  145,  216,  217,   -1,   -1,  182,  183,  168,  169,  252,  253,   -1,   -1,
   208,  209,  192,  193,  288,  289,   -1,   -1,  261,  262,  240,  241,  360,  361,   -1,   -1,
   313,  314,  288,  289,  432,  433,   -1,   -1,  365,  366,  336,  337,  504,  505,   -1,   -1,
   417,  418,  384,  385,  576,  577,   -1,   -1,  470,  471,  432,  433,  648,  649,   -1,   -1,
   522,  523,  480,  481,  720,  721,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   26,   27,   24,   25,   36,   37,   -1,   -1,
    52,   53,   48,   49,   72,   73,   -1,   -1,   78,   79,   72,   73,  108,  109,   -1,   -1,
   104,  105,   96,   97,  144,  145,   -1,   -1,  130,  131,  120,  121,  180,  181,   -1,   -1,
   156,  157,  144,  145,  216,  217,   -1,   -1,  182,  183,  168,  169,  252,  253,   -1,   -1,
   208,  209,  192,  193,  288,  289,   -1,   -1,  261,  262,  240,  241,  360,  361,   -1,   -1,
   313,  314,  288,  289,  432,  433,   -1,   -1,  365,  366,  336,  337,  504,  505,   -1,   -1,
   417,  418,  384,  385,  576,  577,   -1,   -1,  470,  471,  432,  433,  648,  649,   -1,   -1,
   522,  523,  480,  481,  720,  721,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   52,   53,   48,   49,   72,   73,   -1,   -1,
   104,  105,   96,   97,  144,  145,   -1,   -1,  156,  157,  144,  145,  216,  217,   -1,   -1,
   208,  209,  192,  193,  288,  289,   -1,   -1,  261,  262,  240,  241,  360,  361,   -1,   -1,
   313,  314,  288,  289,  432,  433,   -1,   -1,  365,  366,  336,  337,  504,  505,   -1,   -1,
   417,  418,  384,  385,  576,  577,   -1,   -1,  522,  523,  480,  481,  720,  721,   -1,   -1,
   626,  627,  576,  577,  864,  865,   -1,   -1,  731,  732,  672,  673, 1008, 1009,   -1,   -1,
   835,  836,  768,  769, 1152, 1153,   -1,   -1,  940,  941,  864,  865, 1296, 1297,   -1,   -1,
  1044, 1045,  960,  961, 1440, 1441,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   52,   53,   48,   49,   72,   73,   -1,   -1,
   104,  105,   96,   97,  144,  145,   -1,   -1,  156,  157,  144,  145,  216,  217,   -1,   -1,
   208,  209,  192,  193,  288,  289,   -1,   -1,  261,  262,  240,  241,  360,  361,   -1,   -1,
   313,  314,  288,  289,  432,  433,   -1,   -1,  365,  366,  336,  337,  504,  505,   -1,   -1,
   417,  418,  384,  385,  576,  577,   -1,   -1,  522,  523,  480,  481,  720,  721,   -1,   -1,
   626,  627,  576,  577,  864,  865,   -1,   -1,  731,  732,  672,  673, 1008, 1009,   -1,   -1,
   835,  836,  768,  769, 1152, 1153,   -1,   -1,  940,  941,  864,  865, 1296, 1297,   -1,   -1,
  1044, 1045,  960,  961, 1440, 1441,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   68,   72,   64,   68,   96,  100,   -1,   -1,
   104,  108,   96,  100,  144,  148,   -1,   -1,  120,  124,  112,  116,  168,  172,   -1,   -1,
   136,  140,  128,  132,  192,  196,   -1,   -1,  172,  176,  160,  164,  240,  244,   -1,   -1,
   208,  212,  192,  196,  288,  292,   -1,   -1,  240,  244,  224,  228,  336,  340,   -1,   -1,
   276,  280,  256,  260,  384,  388,   -1,   -1,  312,  316,  288,  292,  432,  436,   -1,   -1,
   348,  352,  320,  324,  480,  484,   -1,   -1,  380,  384,  352,  356,  528,  532,   -1,   -1,
   416,  420,  384,  388,  576,  580,   -1,   -1,  484,  488,  448,  452,  672,  676,   -1,   -1,
   556,  560,  512,  516,  768,  772,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   68,   72,   64,   68,   96,  100,   -1,   -1,
   104,  108,   96,  100,  144,  148,   -1,   -1,  120,  124,  112,  116,  168,  172,   -1,   -1,
   136,  140,  128,  132,  192,  196,   -1,   -1,  172,  176,  160,  164,  240,  244,   -1,   -1,
   208,  212,  192,  196,  288,  292,   -1,   -1,  240,  244,  224,  228,  336,  340,   -1,   -1,
   276,  280,  256,  260,  384,  388,   -1,   -1,  312,  316,  288,  292,  432,  436,   -1,   -1,
   348,  352,  320,  324,  480,  484,   -1,   -1,  380,  384,  352,  356,  528,  532,   -1,   -1,
   416,  420,  384,  388,  576,  580,   -1,   -1,  484,  488,  448,  452,  672,  676,   -1,   -1,
   556,  560,  512,  516,  768,  772,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,  104,  105,   96,   97,  144,  145,   -1,   -1,
   130,  131,  120,  121,  180,  181,   -1,   -1,  156,  157,  144,  145,  216,  217,   -1,   -1,
   182,  183,  168,  169,  252,  253,   -1,   -1,  208,  209,  192,  193,  288,  289,   -1,   -1,
   261,  262,  240,  241,  360,  361,   -1,   -1,  313,  314,  288,  289,  432,  433,   -1,   -1,
   365,  366,  336,  337,  504,  505,   -1,   -1,  417,  418,  384,  385,  576,  577,   -1,   -1,
   522,  523,  480,  481,  720,  721,   -1,   -1,  626,  627,  576,  577,  864,  865,   -1,   -1,
   731,  732,  672,  673, 1008, 1009,   -1,   -1,  835,  836,  768,  769, 1152, 1153,   -1,   -1,
  1044, 1045,  960,  961, 1440, 1441,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,  104,  105,   96,   97,  144,  145,   -1,   -1,
   130,  131,  120,  121,  180,  181,   -1,   -1,  156,  157,  144,  145,  216,  217,   -1,   -1,
   182,  183,  168,  169,  252,  253,   -1,   -1,  208,  209,  192,  193,  288,  289,   -1,   -1,
   261,  262,  240,  241,  360,  361,   -1,   -1,  313,  314,  288,  289,  432,  433,   -1,   -1,
   365,  366,  336,  337,  504,  505,   -1,   -1,  417,  418,  384,  385,  576,  577,   -1,   -1,
   522,  523,  480,  481,  720,  721,   -1,   -1,  626,  627,  576,  577,  864,  865,   -1,   -1,
   731,  732,  672,  673, 1008, 1009,   -1,   -1,  835,  836,  768,  769, 1152, 1153,   -1,   -1,
  1044, 1045,  960,  961, 1440, 1441,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,  104,  105,   96,   97,  144,  145,   -1,   -1,
   156,  157,  144,  145,  216,  217,   -1,   -1,  182,  183,  168,  169,  252,  253,   -1,   -1,
   208,  209,  192,  193,  288,  289,   -1,   -1,  261,  262,  240,  241,  360,  361,   -1,   -1,
   313,  314,  288,  289,  432,  433,   -1,   -1,  365,  366,  336,  337,  504,  505,   -1,   -1,
   417,  418,  384,  385,  576,  577,   -1,   -1,  522,  523,  480,  481,  720,  721,   -1,   -1,
   626,  627,  576,  577,  864,  865,   -1,   -1,  731,  732,  672,  673, 1008, 1009,   -1,   -1,
   835,  836,  768,  769, 1152, 1153,   -1,   -1, 1044, 1045,  960,  961, 1440, 1441,   -1,   -1,
  1253, 1254, 1152, 1153, 1728, 1729,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,  104,  105,   96,   97,  144,  145,   -1,   -1,
   156,  157,  144,  145,  216,  217,   -1,   -1,  182,  183,  168,  169,  252,  253,   -1,   -1,
   208,  209,  192,  193,  288,  289,   -1,   -1,  261,  262,  240,  241,  360,  361,   -1,   -1,
   313,  314,  288,  289,  432,  433,   -1,   -1,  365,  366,  336,  337,  504,  505,   -1,   -1,
   417,  418,  384,  385,  576,  577,   -1,   -1,  522,  523,  480,  481,  720,  721,   -1,   -1,
   626,  627,  576,  577,  864,  865,   -1,   -1,  731,  732,  672,  673, 1008, 1009,   -1,   -1,
   835,  836,  768,  769, 1152, 1153,   -1,   -1, 1044, 1045,  960,  961, 1440, 1441,   -1,   -1,
  1253, 1254, 1152, 1153, 1728, 1729,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   32,   36,   32,   36,   48,   52,   -1,   -1,
    68,   72,   64,   68,   96,  100,   -1,   -1,  104,  108,   96,  100,  144,  148,   -1,   -1,
   136,  140,  128,  132,  192,  196,   -1,   -1,  172,  176,  160,  164,  240,  244,   -1,   -1,
   208,  212,  192,  196,  288,  292,   -1,   -1,  240,  244,  224,  228,  336,  340,   -1,   -1,
   276,  280,  256,  260,  384,  388,   -1,   -1,  312,  316,  288,  292,  432,  436,   -1,   -1,
   348,  352,  320,  324,  480,  484,   -1,   -1,  380,  384,  352,  356,  528,  532,   -1,   -1,
   416,  420,  384,  388,  576,  580,   -1,   -1,  452,  456,  416,  420,  624,  628,   -1,   -1,
   484,  488,  448,  452,  672,  676,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     0,    0,    0,    0,    0,    0,   -1,   -1,   32,   36,   32,   36,   48,   52,   -1,   -1,
    68,   72,   64,   68,   96,  100,   -1,   -1,  104,  108,   96,  100,  144,  148,   -1,   -1,
   136,  140,  128,  132,  192,  196,   -1,   -1,  172,  176,  160,  164,  240,  244,   -1,   -1,
   208,  212,  192,  196,  288,  292,   -1,   -1,  240,  244,  224,  228,  336,  340,   -1,   -1,
   276,  280,  256,  260,  384,  388,   -1,   -1,  312,  316,  288,  292,  432,  436,   -1,   -1,
   348,  352,  320,  324,  480,  484,   -1,   -1,  380,  384,  352,  356,  528,  532,   -1,   -1,
   416,  420,  384,  388,  576,  580,   -1,   -1,  452,  456,  416,  420,  624,  628,   -1,   -1,
   484,  488,  448,  452,  672,  676,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1
  };

int checkMpaFrame(unsigned char *buffer, int length, uint32_t *scanner, int *off, int *frameSize)
{
  int i = 0;

  while (i < length) {
        *scanner <<= 8;
        *scanner |= buffer[i];

        if ((*scanner & 0xFFF00000) == 0xFFF00000 && (*scanner & 0x00000003) != 2 &&
                        (*frameSize = mpaFrameSizes[(*scanner >> 9) & 0x7ff]) != -1) {
           *off = i - 3;
           return 1;
           }

        i++;
        }

  return 0;
}

const int ac3FrameSizes[] = {
  // fs = 48 kHz
   128,  128,  160,  160,  192,  192,  224,  224,  256,  256,  320,  320,  384,  384,  448,  448,
   512,  512,  640,  640,  768,  768,  896,  896, 1024, 1024, 1280, 1280, 1536, 1536, 1792, 1792,
  2048, 2048, 2304, 2304, 2560, 2560,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  // fs = 44.1 kHz
   138,  140,  174,  176,  208,  210,  242,  244,  278,  280,  348,  350,  416,  418,  486,  488,
   556,  558,  696,  698,  834,  836,  974,  976, 1114, 1116, 1392, 1394, 1670, 1672, 1950, 1952,
  2228, 2230, 2506, 2508, 2786, 2788,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  // fs = 32 kHz
   192,  192,  240,  240,  288,  288,  336,  336,  384,  384,  480,  480,  576,  576,  672,  672,
   768,  768,  960,  960, 1152, 1152, 1344, 1344, 1536, 1536, 1920, 1920, 2304, 2304, 2688, 2688,
  3072, 3072, 3456, 3456, 3840, 3840,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  //
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
  };

int checkAc3Frame(unsigned char *buffer, int length, uint32_t *scanner, int *off, int *frameSize)
{
  int i = 0;

  while (i < length) {
        if ((*scanner & 0xFFFF0000) == 0x0B770000 &&
                         (*frameSize = ac3FrameSizes[buffer[i]]) != 0) {
           *off = i - 4;
           return 1;
           }
        *scanner <<= 8;
        *scanner |= buffer[i];
        i++;
        }

  return 0;
}

struct pesPacketHeaderInfo {
  int id;
  int length;
  int payloadOffset;
  int alignmentFlag;
  int ptsDtsFlag;
  int continuation;
  uint32_t errors;
  int packetCount;
  int packetErrCount;
  };


struct pesPacketHeader {
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t marker:2;
  uint8_t scrambling_contol:2;
  uint8_t priority:1;
  uint8_t alignment:1;
  uint8_t copyright:1;
  uint8_t original:1;
#else
  uint8_t original:1;
  uint8_t copyright:1;
  uint8_t alignment:1;
  uint8_t priority:1;
  uint8_t scrambling_contol:2;
  uint8_t marker:2;
#endif
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t pts_dts_flag:2;
  uint8_t escr_flag:1;
  uint8_t es_rate_flag:1;
  uint8_t dsm_flag:1;
  uint8_t add_copy_info_flag:1;
  uint8_t crc_flag:1;
  uint8_t extension_flag:1;
#else
  uint8_t extension_flag:1;
  uint8_t crc_flag:1;
  uint8_t add_copy_info_flag:1;
  uint8_t dsm_flag:1;
  uint8_t es_rate_flag:1;
  uint8_t escr_flag:1;
  uint8_t pts_dts_flag:2;
#endif
  uint8_t header_length:8;
  };

struct pesPtsDtsField {
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t marker1:4;
  uint8_t ts_32_30:3;
  uint8_t marker2:1;
#else
  uint8_t marker2:1;
  uint8_t ts_32_30:3;
  uint8_t marker1:4;
#endif
  uint8_t ts_29_22:8;
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t ts_21_15:7;
  uint8_t marker3:1;
#else
  uint8_t marker3:1;
  uint8_t ts_21_15:7;
#endif
  uint8_t ts_14_7:8;
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t ts_6_0:7;
  uint8_t marker4:1;
#else
  uint8_t marker4:1;
  uint8_t ts_6_0:7;
#endif
  };

struct pesESCRField {
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t reserved : 2;
  uint8_t escr_base_32_30:3;
  uint8_t marker1:1;
  uint8_t escr_base_29_28:2;
#else
  uint8_t escr_base_29_28:2;
  uint8_t marker1:1;
  uint8_t escr_base_32_30:3;
  uint8_t reserved : 2;
#endif
  uint8_t escr_base_27_20:8;
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t escr_base_19_15:5;
  uint8_t marker2:1;
  uint8_t escr_base_14_13:2;
#else
  uint8_t escr_base_14_13:2;
  uint8_t marker2:1;
  uint8_t escr_base_19_15:5;
#endif
  uint8_t escr_base_12_5:8;
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t escr_base_4_0:5;
  uint8_t marker3:1;
  uint8_t escr_ext_hi:2;
#else
  uint8_t escr_ext_hi:2;
  uint8_t marker3:1;
  uint8_t escr_base_4_0:5;
#endif
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t escr_ext_lo:7;
  uint8_t marker4:1;
#else
  uint8_t marker4:1;
  uint8_t escr_ext_lo:7;
#endif
  };

struct pesESRateField {
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t marker1:1;
  uint8_t es_rate_21_15:7;
#else
  uint8_t es_rate_21_15:7;
  uint8_t marker1:1;
#endif
  uint8_t es_rate_14_7:8;
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t es_rate_6_0:7;
  uint8_t marker2:1;
#else
  uint8_t marker2:1;
  uint8_t es_rate_6_0:7;
#endif
  };

struct pesExtensionHeader {
#if BYTE_ORDER == BIG_ENDIAN
  uint8_t private_data_flag :1;
  uint8_t pack_header_flag :1;
  uint8_t packet_counter_flag :1;
  uint8_t buffer_flag :1;
  uint8_t reserved:3;
  uint8_t extension2_flag:1;
#else
  uint8_t extension2_flag:1;
  uint8_t reserved:3;
  uint8_t buffer_flag :1;
  uint8_t packet_counter_flag :1;
  uint8_t pack_header_flag :1;
  uint8_t private_data_flag :1;
#endif
  };


#define ERR_PTS_DTS_FLAG (1<<0)
#define ERR_HEAD_SIZE (1<<1)
#define ERR_EXT_HEAD_SIZE (1<<2)
int checkPesHeader(unsigned char *buffer, struct pesPacketHeaderInfo *headerInfo)
{
  headerInfo->packetCount++;

  if (headerInfo->length < 9) {
     headerInfo->packetErrCount++;
     return 0;
     }

  struct pesPacketHeader *head = (struct pesPacketHeader *)(buffer + 6);
  headerInfo->payloadOffset = head->header_length + 9;

  if (headerInfo->length < headerInfo->payloadOffset) {
     headerInfo->packetErrCount++;
     return 0;
     }

  headerInfo->alignmentFlag = head->alignment; //(buffer[6] >> 2) & 1;
  headerInfo->ptsDtsFlag = head->pts_dts_flag; // (buffer[7] >> 6) & 3;
  headerInfo->continuation = buffer[6] == 0x80 && !buffer[7] && !buffer[8];
  headerInfo->errors = 0;
  int headerOffset = 9;
  switch (head->pts_dts_flag) {
    case 1:
         headerOffset += 5;
         headerInfo->errors |= ERR_PTS_DTS_FLAG;
         break;
    case 2:
         headerOffset += 5;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
         break;
    case 3:
         headerOffset += 10;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
         break;
    }

  if (head->escr_flag) {
         headerOffset += 6;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
     }
  if (head->es_rate_flag) {
         headerOffset += 3;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
     }
  if (head->dsm_flag) {
         headerOffset += 1;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
     }
  if (head->add_copy_info_flag) {
         headerOffset += 1;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
     }
  if (head->crc_flag) {
         headerOffset += 2;
         if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_HEAD_SIZE;
            }
     }
  if (head->extension_flag ) {
     headerOffset += 1;
     if (headerInfo->payloadOffset < headerOffset) {
        headerInfo->errors |= ERR_HEAD_SIZE;
        return 1;
        }
     struct pesExtensionHeader *ext_head =(struct pesExtensionHeader *) (buffer + headerOffset -1);

     if (ext_head->private_data_flag) {
        headerOffset += 16;
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        }
     if (ext_head->pack_header_flag) {
        headerOffset += 1;
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        headerOffset += *(buffer + headerOffset -1);
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        }
     if (ext_head->packet_counter_flag) {
        headerOffset += 2;
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        }
     if (ext_head->buffer_flag) {
        headerOffset += 2;
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        }
     if (ext_head->extension2_flag) {
        headerOffset += 1;
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        headerOffset += *(buffer + headerOffset -1) & 0x7F;
        if (headerInfo->payloadOffset < headerOffset) {
            headerInfo->errors |= ERR_EXT_HEAD_SIZE;
            return 1;
            }
        }

     }
  return 1;
}

int readNextPacket(int fd, uint64_t *fileOffset,  unsigned char *buffer, int *offset, int *length, struct pesPacketHeaderInfo *headerInfo)
{
  int neof = 1;
  int i=0;
  int skipped = 0;
  while (neof) {
        if (*length - *offset - i < 6) {
           *length -= *offset + i;
           memcpy(buffer, buffer + *offset + i, *length);

           *offset = 0;
           skipped += i;
           i = 0;

           while ((neof = read(fd, buffer + *length, BUFFERSIZE - *length)) < 0
                                       && (errno == EINTR || errno == EBUSY))
                 ;

           if (neof < 0)
              return -1;

           *length += neof;
           continue;
           }

        if (!buffer[*offset + i] && !buffer[*offset + i + 1] &&
                               buffer[*offset + i + 2] == 1 &&
                              (buffer[*offset + i + 3] == 0xBD ||
                              (buffer[*offset + i + 3] >= 0xC0 &&
                               buffer[*offset + i + 3] <= 0xEF))) {
           skipped += i;
           *offset += i;
           i = 0;

           if (skipped) {
              fprintf(stderr, "Skipped %d bytes while syncing on next pes "
                                                      "packet at 0x%llX\n",
                                                     skipped, *fileOffset);
              *fileOffset += skipped;
              skipped = 0;
              }

           headerInfo->id = buffer[*offset + 3];
           headerInfo->length = ((buffer[*offset + 4] << 8) | buffer[*offset + 5]) + 6;

           if (*length - *offset < headerInfo->length) {
              *length -= *offset;
              memmove(buffer, buffer + *offset, *length);
              *offset = 0;

              while ((neof = read(fd, buffer + *length, BUFFERSIZE - *length)) < 0
                                       && (errno == EINTR || errno == EBUSY))
                    ;

              if (neof < 0)
                 return -1;

              *length += neof;
              if (*length < headerInfo->length) {
                 fprintf(stderr, "Incomplete packet (0x%02X) in file at 0x%llX "
                                          "(Expected %d bytes Found %d bytes)\n",
                                                     headerInfo->id, *fileOffset,
                                                    headerInfo->length, *length);
                 return 0;
                 }
              }

           if (!checkPesHeader(buffer + *offset, headerInfo)) {
              fprintf(stderr, "Illegal pes-header (0x%02X) in file at 0x%llX "
                                                   "(PacketLength %d bytes)\n",
                              headerInfo->id, *fileOffset, headerInfo->length);
              *fileOffset += headerInfo->length;
              *offset += headerInfo->length;
              continue;
              }

           if (headerInfo->length == headerInfo->payloadOffset) {
              fprintf(stderr, "Packet (0x%02X) without payload in file at "
                                         "0x%llX (PacketLength %d bytes)\n",
                           headerInfo->id, *fileOffset, headerInfo->length);
              *fileOffset += headerInfo->length;
              *offset += headerInfo->length;
              continue;
              }
           return 1;
           }
        else
           i++;
        }

  skipped += *length - *offset;
  if (skipped) {
     fprintf(stderr, "Skipped %d bytes while syncing on next pes "
                                              "packet at 0x%llX\n",
                                              skipped, *fileOffset);
     *fileOffset += skipped;
     }
  return 0;
}

int checkFrameIndices(struct tIndex *index, int count)
{
  if (!count)
     return 1;

  fprintf(stderr,"Checking indicies...\n");
  int i;
  int r = 1;
  int wrongCount = 0;
  for (i = 0; i < count - 1; i++) {
      if ((index[i].offset >= index[i + 1].offset &&
          index[i].number == index[i + 1].number) ||
          index[i].number > index[i + 1].number)  {
         fprintf(stderr,"Offset of index %d out of order\n", i);
         r = 0;
         wrongCount++;
         }
      else if(index[i].number == index[i + 1].number &&
         index[i + 1].offset - index[i].offset > MAXFRAMESIZE) {
         fprintf(stderr, "Frame Size %d of index %d too big\n",
                      index[i + 1].offset - index[i].offset, i);
         wrongCount++;
         }

      if (!index[i].type || index[i].type > 3) {
         fprintf(stderr,"Illegal frame type %hhd in index %d\n", index[i].type, i);
         wrongCount++;
         }

      }

  if (!index[i].type || index[i].type > 3) {
     fprintf(stderr,"Illegal frame type %hhd in index %d\n", index[i].type, i);
     wrongCount++;
     }

  if (wrongCount) {
     fprintf(stderr, "Found %d inconsistent in %d index entries\n", wrongCount, count);
     }
  else
     fprintf(stderr, "Index file consistency check passed\n");
  return r;
}
int check_alignment_flag = 0;
void checkAndPrintErrors(int framesCount, int firstFrameOffset,
                     struct pesPacketHeaderInfo *curHeaderInfo,
                    struct pesPacketHeaderInfo *lastHeaderInfo,
                         uint64_t curFileOffset, int curFileNr,
                                       uint64_t lastFileOffset)
{

  if (curHeaderInfo->errors)
     curHeaderInfo->packetErrCount++;
  if (curHeaderInfo->errors & ERR_PTS_DTS_FLAG) {
     fprintf(stderr, "Wrong pts dts flag in header of packet id 0x%02X at 0x%llX in "
                     "file %d\n", curHeaderInfo->id, curFileOffset, curFileNr);
     }
  if (curHeaderInfo->errors & ERR_HEAD_SIZE) {
     fprintf(stderr, "Wrong header size smaller than indicated in header of packet id 0x%02X at 0x%llX in "
                            "file %d\n", curHeaderInfo->id, curFileOffset, curFileNr);
     }
  if (curHeaderInfo->errors & ERR_EXT_HEAD_SIZE) {
     fprintf(stderr, "Wrong header size smaller than indicated extension-header of packet id 0x%02X at 0x%llX in "
                            "file %d\n", curHeaderInfo->id, curFileOffset, curFileNr);
     }
  if (framesCount) {
     if (firstFrameOffset != 0)
        fprintf(stderr, "Frame not aligned in packet id 0x%02X at 0x%llX in "
                   "file %d\n", curHeaderInfo->id, curFileOffset, curFileNr);

     if (check_alignment_flag && !curHeaderInfo->alignmentFlag)
        fprintf(stderr, "Alignment not set in packet id 0x%02X with framestart"
                                   "at 0x%llX in file %d\n", curHeaderInfo->id,
                                                     curFileOffset, curFileNr);

     if (framesCount > 1)
        fprintf(stderr, "%d Frames starting in packet id 0x%02X at 0x%llX in "
                                  "file %d\n", framesCount, curHeaderInfo->id,
                                                    curFileOffset, curFileNr);
     }
  else {
     if (curHeaderInfo->alignmentFlag)
        fprintf(stderr, "Alignment set in packet id 0x%02X without "
                                 "framestart at 0x%llX in file %d\n",
                        curHeaderInfo->id, curFileOffset, curFileNr);

     if (curHeaderInfo->ptsDtsFlag)
        fprintf(stderr, "PTS/DTS in packet id 0x%02X without framestart "
                                                 "at 0x%llX in file %d\n",
                             curHeaderInfo->id, curFileOffset, curFileNr);

     if (!lastHeaderInfo)
        fprintf(stderr, "No frame starts in first packet id 0x%02X at 0x%llX "
                                           "in file %d\n",  curHeaderInfo->id,
                                                    curFileOffset, curFileNr);
     else if (lastHeaderInfo->length < PACKETSIZE)
        fprintf(stderr, "Packet not fully filled (%d) in packet id 0x%02X "
                                                   "at 0x%llX in file %d\n",
                                  lastHeaderInfo->length, curHeaderInfo->id,
                            lastFileOffset, lastFileOffset < curFileOffset ?
                                                  curFileNr : curFileNr -1);
     }
}

void checkRecording(const char *path)
{
  struct tIndex *frameIndices;
  int indexFrameCount;
  int curIndex = 0;
  if (!(frameIndices = readIndexFile(path, &indexFrameCount))) {
     fprintf(stderr, "Cannot read index.vdr file\n");
     indexFrameCount = 0;
     }

  if (!checkFrameIndices(frameIndices, indexFrameCount))
     indexFrameCount = 0;

  int fileNr = 1;
  int recFileFd = openRecordFile(path, 1);

  if (recFileFd < 0) {
     fprintf(stderr, "Cannot open first recording file. Really a vdr recording???\n");
     free (frameIndices);
     return;
     }

  uint32_t videoScanner = 0xFFFFFFFF;
  struct pesPacketHeaderInfo lastVideoHeaderInfo;
  uint64_t videoFileOffset = 0;
  int videoInfoAvail = 0;
  int videoFramesCount = 0;
  int fileStart = 1;

  uint32_t mpaScanner[MAXMPASTREAMS];
  struct pesPacketHeaderInfo lastMpaHeaderInfo[MAXMPASTREAMS];
  int mpaInfoAvail[MAXMPASTREAMS];
  int mpaFrameSize[MAXMPASTREAMS];
  uint64_t mpaFileOffset[MAXMPASTREAMS];
  int i;
  for (i = 0; i < MAXMPASTREAMS; i++) {
      mpaScanner[i] = 0;
      mpaInfoAvail[i] = 0;
      mpaFrameSize[i] = 0;
      mpaFileOffset[i] = 0;
      }

  uint32_t ac3Scanner[MAXAC3STREAMS];
  struct pesPacketHeaderInfo lastAc3HeaderInfo[MAXAC3STREAMS];
  int ac3InfoAvail[MAXAC3STREAMS];
  int ac3FrameSize[MAXAC3STREAMS];
  uint64_t ac3FileOffset[MAXAC3STREAMS];
  for (i = 0; i < MAXAC3STREAMS; i++) {
      ac3Scanner[i] = 0;
      ac3InfoAvail[i] = 0;
      ac3FrameSize[i] = 0;
      ac3FileOffset[i] = 0;
      }

  unsigned char *pesBuffer;
  int pesBufferLength=0;
  int pesBufferOffset=0;
  uint64_t fileOffset = 0;
  pesBuffer = malloc(BUFFERSIZE);

  if (!pesBuffer) {
     fprintf(stderr,"Cannot alloc memory for pes buffer\n");
     free(frameIndices);
     return;
     }

  fprintf(stderr, "Starting analyse of record file number %d\n",fileNr);
  struct pesPacketHeaderInfo curHeaderInfo;
  curHeaderInfo.packetCount = 0;
  curHeaderInfo.packetErrCount = 0;
  while (recFileFd >= 0) {
        int r = readNextPacket(recFileFd, &fileOffset, pesBuffer,
             &pesBufferOffset, &pesBufferLength, &curHeaderInfo);

        if (r < 0) {
           fprintf(stderr,"Error while reading\n");
           free(pesBuffer);
           free(frameIndices);
           return;
           }

        if (!r) {
           fileNr++;
           close(recFileFd);
           recFileFd = openRecordFile(path, fileNr);
           while (curIndex < indexFrameCount &&
                  frameIndices[curIndex].number < fileNr) {
                 fprintf(stderr, "Offset 0x%X of Index %d not present in file %hhd\n",
                                            frameIndices[curIndex].offset, curIndex,
                                                     frameIndices[curIndex].number);
                 curIndex++;
                 }
           fileOffset = 0;
           pesBufferLength = 0;
           pesBufferOffset = 0;
           if (recFileFd >= 0) {
              fprintf(stderr, "Starting analyse of record file number %d\n",fileNr);
              }
           fileStart = 1;
           continue;
           }

        while (curIndex < indexFrameCount &&
               frameIndices[curIndex].offset < fileOffset &&
               frameIndices[curIndex].number == fileNr) {
              fprintf(stderr, "No Packet starts at 0x%X for Index %d in file %d\n",
                                           frameIndices[curIndex].offset, curIndex,
                                                                           fileNr);
              curIndex++;
              }

        if (curIndex < indexFrameCount && (curHeaderInfo.id & 0xF0) != 0xE0 &&
                                frameIndices[curIndex].offset == fileOffset &&
                                    frameIndices[curIndex].number == fileNr) {
           fprintf(stderr, "Packet starts at 0x%X for Index %d in file %d has "
                            "wrong Id(0x%02X)\n",frameIndices[curIndex].offset,
                                           curIndex, fileNr, curHeaderInfo.id);
           curIndex++;
           }

        if (curHeaderInfo.length > PACKETSIZE) {
           fprintf(stderr, "Overfilled Packet (%d) Id 0x%02X found at 0x%llX "
                                                              "in file %d \n",
                                       curHeaderInfo.length, curHeaderInfo.id,
                                                          fileOffset, fileNr);
           }

        int payloadOffset = pesBufferOffset + curHeaderInfo.payloadOffset;
        int payloadLength = curHeaderInfo.length - curHeaderInfo.payloadOffset;

        switch (curHeaderInfo.id) {
          case 0xBD: {
               if (payloadLength <= 4) {
                  fprintf(stderr, "Payload too small for AC3 packet at 0x%llX "
                                           "in file %d\n", fileOffset, fileNr);
                  break;
                  }
               int subStreamId = pesBuffer[payloadOffset] - 0x80;
               if (subStreamId < 0 || subStreamId > 0xF) {
                  fprintf(stderr, "Illegal sub stream id for AC3 packet at "
                                 "0x%llX in file %d\n", fileOffset, fileNr);
                  break;
                  }
               int framesStarting = 0;
               int firstFrameOffset = 0;
               int startScan = 4 + ac3FrameSize[subStreamId];
               int curOffset = 0;

               while (startScan < payloadLength &&
                      checkAc3Frame(pesBuffer + payloadOffset + startScan,
                      payloadLength - startScan, &ac3Scanner[subStreamId],
                               &curOffset, &ac3FrameSize[subStreamId])) {

                     framesStarting++;
                     ac3Scanner[subStreamId] = 0;

                     if (framesStarting == 1) {
                        firstFrameOffset = startScan + curOffset;
                        }

                     if (curOffset > 0) {
                        fprintf(stderr, "%d Bytes of none AC3-framedata in "
                                             "packet at 0x%llX in file %d\n",
                                              curOffset, fileOffset, fileNr);
                        }
                     startScan += curOffset + ac3FrameSize[subStreamId];
                     }

               if (startScan >= payloadLength)
                   ac3FrameSize[subStreamId] = startScan - payloadLength;
               else if (ac3FrameSize[subStreamId])
                   ac3FrameSize[subStreamId] -= payloadLength - startScan;

               if (framesStarting && pesBuffer[payloadOffset + 3] != 1)
                  fprintf(stderr, "Wrong substream header in packet at "
                                                   "0x%llX in file %d\n",
                                                     fileOffset, fileNr);

               if (!framesStarting && pesBuffer[payloadOffset + 3] != 0)
                  fprintf(stderr, "Wrong substream header in packet at "
                                                   "0x%llX in file %d\n",
                                                     fileOffset, fileNr);

               checkAndPrintErrors(framesStarting, firstFrameOffset - 4,
                             &curHeaderInfo, ac3InfoAvail[subStreamId] ?
                                 &lastAc3HeaderInfo[subStreamId] : NULL,
                                                     fileOffset, fileNr,
                                            ac3FileOffset[subStreamId]);

               ac3FileOffset[subStreamId] = fileOffset;
               ac3InfoAvail[subStreamId] = 1;
               memcpy(&lastAc3HeaderInfo[subStreamId], &curHeaderInfo,
                                              sizeof(curHeaderInfo));
               break;
               }
          case 0xC0 ... 0xDF: {
               int streamId = curHeaderInfo.id - 0xC0;

               int framesStarting = 0;
               int firstFrameOffset = 0;
               int startScan = mpaFrameSize[streamId];
               int curOffset = 0;
               int curFrameSize = 0;

               while (startScan < payloadLength &&
                      checkMpaFrame(pesBuffer + payloadOffset + startScan,
                         payloadLength - startScan, &mpaScanner[streamId],
                                             &curOffset, &curFrameSize)) {

                     mpaFrameSize[streamId] = curFrameSize;
                     framesStarting++;
                     mpaScanner[streamId] = 0;

                     if (framesStarting == 1) {
                        firstFrameOffset = startScan + curOffset;
                        }

                     if (curOffset > 0 && mpaFrameSize[streamId]) {
                        fprintf(stderr, "%d Bytes of none mpa-framedata in "
                                    "packet (0x%02X) at 0x%llX in file %d\n",
                            curOffset, curHeaderInfo.id, fileOffset, fileNr);
                        }
                     startScan += curOffset + (mpaFrameSize[streamId] > 0 ?
                                               mpaFrameSize[streamId] : 4);
                     }

               if (startScan >= payloadLength)
                  mpaFrameSize[streamId] = startScan - payloadLength;
               else if (mpaFrameSize[streamId])
                  mpaFrameSize[streamId] -= payloadLength - startScan;

               checkAndPrintErrors(framesStarting, firstFrameOffset,
                            &curHeaderInfo, mpaInfoAvail[streamId] ?
                                &lastMpaHeaderInfo[streamId] : NULL,
                                                 fileOffset, fileNr,
                                           mpaFileOffset[streamId]);

               mpaFileOffset[streamId] = fileOffset;
               mpaInfoAvail[streamId] = 1;
               memcpy(&lastMpaHeaderInfo[streamId], &curHeaderInfo,
                                           sizeof(curHeaderInfo));
               break;
               }
          case 0xE0 ... 0xEF: {
               int framesStarting = 0;
               int firstFrameOffset = 0;
               int firstType = 0;
               int curOffset = 0;
               int skipOffset = 0;
               int curType = 0;
               int startScan = 0;

               while (startScan < payloadLength &&
                      checkVideoFrame(pesBuffer + payloadOffset + startScan,
                                   payloadLength - startScan, &videoScanner,
                                                    &curOffset, &skipOffset, &curType)) {

                     framesStarting++;
                     videoScanner = 0xFFFFFFFF;

                     if (framesStarting == 1) {
                        firstFrameOffset = startScan + curOffset;
                        firstType = curType;
                        }

                     startScan += skipOffset;
                     }

               videoFramesCount += framesStarting;

               checkAndPrintErrors(framesStarting, firstFrameOffset,
                                    &curHeaderInfo, videoInfoAvail ?
                                        &lastVideoHeaderInfo : NULL,
                                                 fileOffset, fileNr,
                                                   videoFileOffset);
               if (indexFrameCount) {
                  if (framesStarting) {
                     if (curIndex >= indexFrameCount ||
                         frameIndices[curIndex].offset != fileOffset ||
                         frameIndices[curIndex].number != fileNr)
                        fprintf(stderr, "No index entry found for video "
                                       "frame in video packet at 0x%llX "
                                       "in file %d\n", fileOffset, fileNr);
                     else {
                        if (frameIndices[curIndex].type != firstType)
                           fprintf(stderr, "Frame type (%hhd/%d) mismatched "
                                            "for video in index %d in video "
                                              "packet at 0x%llX in file %d\n",
                                       frameIndices[curIndex].type, firstType,
                                                curIndex, fileOffset, fileNr);
                        curIndex++;
                        }
                     }
                  else if (curIndex < indexFrameCount &&
                         frameIndices[curIndex].offset == fileOffset &&
                         frameIndices[curIndex].number == fileNr) {
                     fprintf(stderr, "No frame found but index available "
                             "for video in index (type %hhd) %d in video "
                                           "packet at 0x%llX in file %d\n",
                                               frameIndices[curIndex].type,
                                             curIndex, fileOffset, fileNr);
                     curIndex++;
                     }
                  }

               if (fileStart && firstType != 1)
                  fprintf(stderr, "File not starting with an i-frame in "
                                 "in video packet at 0x%llX in file %d\n",
                                                      fileOffset, fileNr);
               videoFileOffset = fileOffset;
               videoInfoAvail = 1;
               fileStart = 0;
               memcpy(&lastVideoHeaderInfo, &curHeaderInfo, sizeof(curHeaderInfo));
               break;
               }

          }
          fileOffset += curHeaderInfo.length;
          pesBufferOffset += curHeaderInfo.length;
        }

  fprintf(stderr, "Analyse completed!\n");
  fprintf(stderr, "%d video frames found length is about %02d:%02d:%02d\n",
                                videoFramesCount, videoFramesCount / 90000,
                                         (videoFramesCount % 90000) / 1500,
                                           (videoFramesCount % 1500) / 25);
  if (indexFrameCount) {
     fprintf(stderr, "%d index entries found length is about %02d:%02d:%02d\n",
                                    indexFrameCount, indexFrameCount / 90000,
                                             (indexFrameCount % 90000) / 1500,
                                               (indexFrameCount % 1500) / 25);
     }
  fprintf(stderr, "Found %d pes packet %d with errors\n",curHeaderInfo.packetCount, curHeaderInfo.packetErrCount);
  free(pesBuffer);
  free(frameIndices);
  close(recFileFd);
}

void printUsage(void)
{
  fprintf (stderr,"Usage: check_vdr_rec [-a] <record_dir> [<record_dir> ...]\n");
}

int main(int argc, char **argv)
{
  int i = 1;
  if (argc >= 1 && !strcmp(argv[1],"-a")) {
     check_alignment_flag = 1;
     i++;
     }
  if (argc <= i) {
     printUsage();
     return EXIT_FAILURE;
     }

  for (; i < argc; i++) {
      fprintf(stderr, "Starting analysing recording %s ...\n", argv[i]);
      checkRecording(argv[i]);
      }
  return EXIT_SUCCESS;
}
