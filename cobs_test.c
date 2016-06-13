#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include "cobs.h"

enum {
	ShortLen = 30,
	NumShort = 100,
	MaxLen = ShortLen*NumShort,
	debug = 0,
};

uint8_t dst[3+(MaxLen*255/254)];
uint8_t src[3+(MaxLen*255/254)];
uint8_t dec[3+(MaxLen*255/254)];

void
dump(char *tag, uint8_t *buf, size_t len)
{
	size_t i;
	printf("%s:", tag);
	for(i = 0; i < len; i++)
		printf(" %02x", buf[i]);
	printf("\n");

}

int
main(void)
{
	struct timeval tv;
	size_t i, loop, off, len, enclen, declen, nrd;
	size_t lens[NumShort];

	gettimeofday(&tv, NULL);
	srand48(tv.tv_sec ^ tv.tv_usec);

	off = 0;
	declen = 0;
	enclen = 0;
	for(loop = 0; loop < NumShort; loop++){
		len = lrand48() % ShortLen;
		lens[loop] = len;
		for(i = 0; i < len; i++)
			src[i] = lrand48() & 255;
		enclen = cobs_encode(dst+off, sizeof dst-off, src, len);
		if((ssize_t)enclen == -1){
			fprintf(stderr, "cobs_encode failed at off %zu loop %zu\n", off, loop);
			goto error_out;
		}
		off += enclen;
	}
	off = 0;
	for(loop = 0; loop < NumShort; loop++){
		declen = cobs_decode(dec, sizeof dec, dst+off, sizeof dst-off, &nrd);
		if(lens[loop] != declen){
			fprintf(stderr, "cobs_decode nrd %zu declen %zu wanted %zu loop %zu\n", nrd, declen, lens[loop], loop);
			goto error_out;
		}
		if((ssize_t)declen == -1){
			fprintf(stderr, "cobs_decode failed at off %zu loop %zu\n", off, loop);
			goto error_out;
		}
		off += nrd;
	}

	for(loop = 0; loop < 10000; loop++){
		len = lrand48() % MaxLen;
		for(i = 0; i < len; i++)
			src[i] = lrand48() & 255;
		enclen = cobs_encode(dst, sizeof dst, src, len);
		for(i = enclen; i < sizeof dst; i++)
			dst[i] = lrand48() & 255;
		declen = cobs_decode(dec, sizeof dec, dst, sizeof dst, &nrd);
		if(debug){
			dump("src", src, len);
			dump("enc", dst, enclen);
			dump("dec", dec, declen);
			printf("\n");
		}
		if(nrd != enclen){
			fprintf(stderr, "trouble! len %zu enclen %zu declen %zu nrd %zu\n", len, enclen, declen, nrd);
			goto error_out;
		}
		if(declen != len){
			fprintf(stderr, "trouble! len %zu enclen %zu declen %zu nrd %zu\n", len, enclen, declen, nrd);
			goto error_out;

		}
		if(memcmp(src, dec, len) != 0){
			fprintf(stderr, "contents don't match!\n");
			goto error_out;
		}
	}

	for(loop = 0; loop < 10000; loop++){
		len = lrand48() % MaxLen;
		for(i = 0; i < len; i++)
			src[i] = lrand48() & 255;
		declen = cobs_decode(dec, sizeof dec, src, len, &nrd);
		if((ssize_t)declen != -1){
			if(nrd > len){
				fprintf(stderr, "decoder misbehaved on random input! nrd %zu len %zu\n", nrd, len);
				exit(1);
			}
		}
	}

	return 0;
error_out:
	dump("src", src, len);
	dump("dec", dec, declen);
	return 1;
}
