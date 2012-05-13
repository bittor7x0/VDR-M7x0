/*
 * genindex.c   generates index.vdr file from mpeg files written by VDR
 *
 * Compile:     gcc -o genindex -O2 -Wall -funroll-loops genindex.c
 *
 * Authors:     varies, including me (Werner Fink <werner@suse.de>)
 *
 * Changes:
 *
 *   Fri Mar 15, 2002: Move from random scanner to streamid scanner
 *   for getting the correct offsets of the I-Frames.  Be sure that
 *   no other multiplex than video ones will be scanned.  Increase both
 *   buffers for ringbuffer and stdio to lower I/O load and increase
 *   speed.     //werner
 *
 * Usage:
 *
 *   cd /video[<number>]/<Film>/<Title>/<date>.<time>.<prio>.<life>.rec/
 *   [mv -f index.vdr index.vdr.bak]
 *   [pathto/]genindex
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <endian.h>
#include <byteswap.h>

#define BUFFER_SIZE     4096*128

#define SC_PICTURE      0x00

#define NO_PICTURE      0
#define I_FRAME         1
#define P_FRAME         2
#define B_FRAME         3

#define PROG_STREAM_MAP 0xBC
#ifndef PRIVATE_STREAM1
#define PRIVATE_STREAM1 0xBD
#endif
#define PADDING_STREAM  0xBE
#ifndef PRIVATE_STREAM2
#define PRIVATE_STREAM2 0xBF
#endif
#define AUDIO_STREAM_S  0xC0
#define AUDIO_STREAM_E  0xDF
#define VIDEO_STREAM_S  0xE0
#define VIDEO_STREAM_E  0xEF
#define ECM_STREAM      0xF0
#define EMM_STREAM      0xF1
#define DSM_CC_STREAM   0xF2
#define ISO13522_STREAM 0xF3
#define PROG_STREAM_DIR 0xFF

/* VDR supports only offset with sizeof(int) */
struct tIndex {int offset; unsigned char type; unsigned char number; short reserved; };

static FILE* fp;
static FILE* idx;
static long Foffset;
static char fname[20];
static unsigned char fbuffer[BUFFER_SIZE];

static unsigned char buf(long p)
{
    static unsigned char ringbuf[BUFFER_SIZE];

    if ((p >= Foffset) && (p < (Foffset + BUFFER_SIZE)))
        return ringbuf[p-Foffset];

    Foffset = p;
    if (fseek(fp, p, SEEK_SET) == 0)
    {
        size_t n = 0;

        memset(&ringbuf, 0, sizeof(ringbuf));
        n = fread(ringbuf, 1, sizeof(ringbuf), fp);

        if ((n < sizeof(ringbuf)) && ferror(fp))
        {
            fprintf(stderr, "Could not read from %s: %s\n", fname, strerror(errno));
            exit(1);
        }

        return ringbuf[p-Foffset];
    }
    return 0;
}

static int readfile(const unsigned char number)
{ 
    long filesize;
    long c = 0;

    sprintf(fname,"%03d.vdr", number);
    fp = fopen(fname, "r");
    if (!fp)
    {
        if (errno != ENOENT)
        {
            fprintf(stderr, "Could not open %s: %s\n", fname, strerror(errno));
            return -1;
        }
        putchar('\n');
        return 1;
    }
    setbuffer(fp, fbuffer, sizeof(fbuffer));
    if (fseek(fp, 0, SEEK_END) < 0)
    {
        fprintf(stderr, "Could not goto end of %s: %s\n", fname, strerror(errno));
        return -1;
    }
    if ((filesize = ftell(fp)) < 0)
    {
        fprintf(stderr, "Could not get size of %s: %s\n", fname, strerror(errno));
        return -1;
    }

    if (number > 1)
        putchar('\n');
    printf("Reading %s, %ld kB.\n", fname, (filesize >> 10));
    Foffset = filesize;

    rewind(fp); /* Even if done for p == 0 in buf() */

    while (c < filesize)
    {
        if (buf(c) == 0x00 && buf(c+1) == 0x00 && buf(c+2) == 0x01)
        {
            long w = ((buf(c+4) << 8) | buf(c+5)) + 6;    /* width of frame */
            const long l = w + c;                               /* absolute length */

            if (!((l > c) && (l <= filesize)))
                break;                                          /* while c < filesize */

            switch (buf(c+3))                                   /* streamid */
            {
                case PROG_STREAM_MAP:
                case PRIVATE_STREAM2:
                case PROG_STREAM_DIR:
                case ECM_STREAM     :
                case EMM_STREAM     :
                case PADDING_STREAM :
                case DSM_CC_STREAM  :
                case ISO13522_STREAM:
                case PRIVATE_STREAM1:
                case AUDIO_STREAM_S ... AUDIO_STREAM_E:
                    break;
                case VIDEO_STREAM_S ... VIDEO_STREAM_E:
                    {
                        long off = (buf(c+8) + 9) + c;          /* absolute offset of frame contents */
                        for (;off < l; off++)
                        {
                            if (buf(off) == 0x00 && buf(off+1) == 0x00 && buf(off+2) == 0x01 &&
                                buf(off+3) == SC_PICTURE)
                            {
                                const unsigned char Ptype = (buf(off+5)>>3) & 0x07;
                                if ((Ptype == I_FRAME) || (Ptype == P_FRAME) || (Ptype == B_FRAME))
                                {
                                    /* VDR supports only offset with sizeof(int) */
                                    struct tIndex i = {(int)c, Ptype, number, 0};
#if BYTE_ORDER == BIG_ENDIAN
                                    i.offset = bswap_32(i.offset);
#endif
                                    if (fwrite(&i, 1, sizeof(i), idx) != sizeof(i))
                                    {
                                        fprintf(stderr, "Error writing index file: %s\n", strerror(errno));
                                        return -1;
                                    }
                                    if (Ptype == I_FRAME)
                                    {
                                        printf("I-Frame found at %ld kB\r", (c >> 10));
                                        fflush(stdout);
                                    }
                                    break;                      /* while off < l */
                                }                               /* if Ptype == I_FRAME ... */
                            }                                   /* if buf(off) == 0x00 ... */
                        }                                       /* while off < l */
                    }
                    break;
                default:
                    fprintf(stderr, "\nError while scanning file %s, broken mpeg file?\n", fname);
                    w = 1;
                    break;
            }                                                   /* switch buf(c+3) */
            c += w;                                             /* Next frame */
        }
        else                                                    /* if buf(c) == 0x00 ... */
            c++;                                                /* Broken file, search for begin */
    }                                                           /* while c < filesize */
    fclose(fp);
    return 0;
}

int main()
{
    int c, ret;
    if (!(idx = fopen("index.vdr", "w")))
    {
        fprintf(stderr, "Could not open index.vdr: %s\n", strerror(errno));
        return -1;
    }
    for (c=1; ((ret = readfile(c)) == 0); c++)
        __asm__ __volatile__("": : :"memory");
    fclose(idx);
    return (ret > 0 ? 0 : 1);
}
