#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include "cobs.h"

enum {
	MaxLen = 8000,
};

uint8_t dst[1+(MaxLen*255/254)];
uint8_t src[1+(MaxLen*255/254)];
uint8_t dec[1+(MaxLen*255/254)];

int
main(void)
{
	struct timeval tv;
	int i, loop, len, enclen, declen;

	gettimeofday(&tv, NULL);
	srand48(tv.tv_sec ^ tv.tv_usec);

	for(loop = 0; loop < 10000; loop++){
		len = lrand48() % MaxLen;
		for(i = 0; i < len; i++)
			src[i] = lrand48() & 255;
		enclen = cobs_encode(dst, sizeof dst, src, len, NULL);
		declen = cobs_decode(dec, sizeof dec, dst, enclen, NULL);
		if(declen != len){
			fprintf(stderr, "trouble! len %d enclen %d declen %d\n", len, enclen, declen);
			goto error_out;

		}
		if(memcmp(src, dec, len) != 0){
			fprintf(stderr, "contents don't match!\n");
			goto error_out;
		}
	}

	return 0;
error_out:
	printf("src:");
	for(i = 0; i < len; i++)
		printf(" %02x", src[i]);
	printf("\n");

	printf("dec:");
	for(i = 0; i < declen; i++)
		printf(" %02x", dec[i]);
	printf("\n");
	return 1;
}
