/* 
gcc -Wall ttpci-firmware.c -o ttpci-firmware
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <string.h>

/* crc stuff shamelessly stolen from linux-2.6.0/lib/crc32.c */

/*
 * There are multiple 16-bit CRC polynomials in common use, but this is
 * *the* standard CRC-32 polynomial, first popularized by Ethernet.
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
 */
#define CRCPOLY_LE 0xedb88320
#define CRCPOLY_BE 0x04c11db7

uint32_t crc32_le(uint32_t crc, unsigned char const *p, size_t len)
{
	int i;
	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
	}
	return crc;
}

uint32_t crc32_be(uint32_t crc, unsigned char const *p, size_t len)
{
	int i;
	while (len--) {
		crc ^= *p++ << 24;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x80000000) ? CRCPOLY_BE : 0);
	}
	return crc;
}

int convert(const char *filename, FILE * out)
{
	uint32_t crc = 0, len = 0;
	long size = 0;
	unsigned char *mem = NULL;
	FILE *f = NULL;
	int ret = 0;

	f = fopen(filename, "rb");
	if (NULL == f) {
		fprintf(stderr, "cannot open input file '%s'\n", filename);
		ret = -1;
		goto out;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (0 == size) {
		fprintf(stderr, "file size is 0\n");
		ret = -1;
		goto out;
	}

	mem = (unsigned char *) malloc(size);
	if (NULL == mem) {
		fprintf(stderr, "out of memory.\n");
		ret = -1;
		goto out;
	}
	if (1 != fread(mem, size, 1, f)) {
		fprintf(stderr, "cannot read from input file.\n");
		ret = -1;
		goto out;
	}

	crc = crc32_le(0, mem, size);
	crc = htonl(crc);

	if (4 != fwrite((unsigned char *) &crc, 1, 4, out)) {
		fprintf(stderr, "cannot write to output file #1.\n");
		ret = -1;
		goto out;
	}

	len = htonl(size);
	if (4 != fwrite((unsigned char *) &len, 1, 4, out)) {
		fprintf(stderr, "cannot write to output file #2.\n");
		ret = -1;
		goto out;
	}

	if (1 != fwrite(mem, size, 1, out)) {
		fprintf(stderr, "cannot write to output file #3.\n");
		ret = -1;
		goto out;
	}

	fprintf(stderr, "file: '%s', crc32: 0x%08x, size:%ld\n", filename, ntohl(crc), size);
	ret = 0;

      out:
	if (NULL != mem)
		free(mem);

	if (NULL != f)
		fclose(f);

	return ret;
}

int main(int argc, char *argv[])
{
	char fn[256];
	FILE *f = NULL;

	const char *magic = "AVFW";

	if (argc < 4) {
		fprintf(stderr, "usage: %s dpram root version [output]\n", argv[0]);
		return 1;
	}

	if (argc >= 4) {
		snprintf(fn, 255, "dvb-ttpci-%s.fw",argv[3]);
	} else {
		snprintf(fn, 255, "dvb-ttpci-unknown.fw");
	}

	f = fopen(fn, "wb+");
	if (NULL == f) {
		fprintf(stderr, "cannot open output file '%s'\n", fn);
		return -1;
	}

	if (4 != fwrite(magic, 1, 4, f)) {
		fprintf(stderr, "cannot write magic bytes to output file.\n");
		fclose(f);
		return -1;
	}

	if (0 != convert(argv[1], f)) {
		fprintf(stderr, "cannot write dpram stuff output file.\n");
		fclose(f);
		return -1;
	}

	if (0 != convert(argv[2], f)) {
		fprintf(stderr, "cannot write root stuff output file.\n");
		fclose(f);
		return -1;
	}

	fclose(f);
	fprintf(stderr, "ok, firmware written to '%s'\n",fn);
	return 0;
}
