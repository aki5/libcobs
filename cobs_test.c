#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include "cobs.h"

enum {
	MaxLen = 3000,
	debug = 0,
};

uint8_t dst[2+(MaxLen*255/254)];
uint8_t src[2+(MaxLen*255/254)];
uint8_t dec[2+(MaxLen*255/254)];

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
	size_t i, loop, len, enclen, declen, nrd;

	gettimeofday(&tv, NULL);
	srand48(tv.tv_sec ^ tv.tv_usec);

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

	return 0;
error_out:
	dump("src", src, len);
	dump("dec", dec, declen);
	return 1;
}
