#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cobs.h"

ssize_t
cobs_encode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen, size_t *nrd)
{
	uint8_t *cp, *esrc, *edst;
	uint8_t code;

	cp = dst++;
	edst = dst + dstcap;
	esrc = src + srclen;
	code = 1;
	while(src < esrc && dst < edst){
		if(*src == 0){
			*cp = code;
			cp = dst++;
			code = 1;
			src++;
		} else {
			*dst++ = *src++;
			code++;
			// insert continuation byte before we overflow
			if(code == 255){
				*cp = code;
				if(dst == edst)
					return -1;
				cp = dst++;
				code = 1;
			}
		}
	}
	if(src != esrc)
		return -1;
	*cp = code;
	*dst++ = 0; // message end
	if(nrd != NULL)
		*nrd = srclen - (esrc - src);
	return dstcap - (edst - dst);
}

ssize_t
cobs_decode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen, size_t *nrd)
{
	uint8_t code;
	uint8_t *edst, *esrc;

	edst = dst + dstcap;
	esrc = src + srclen;
	while(src < esrc && dst < edst){
		code = *src++;
		if(code == 0) // message end
			break;
		if((edst - dst) >= code-1 && (esrc - src) >= code-1){
			memcpy(dst, src, code-1);
			dst += code-1;
			src += code-1;
		} else {
			fprintf(stderr, "bad 1, code %d, dstroom %ld srcroom %ld\n", code, (edst-dst), (esrc-src));
			return -1;
		}
		// if code is 255, it is a continuation byte we skip over.
		// otherwise, it's a payload zero.
		if(code != 255){
			if(edst == dst){
				fprintf(stderr, "bad 2\n");
				return -1;
			}
			*dst++ = 0;
		}
	}
	if(src != esrc){
		fprintf(stderr, "bad 3\n");
		return -1;
	}
	if(nrd != NULL)
		*nrd = srclen - (esrc - src);
	return dstcap - (edst - dst) - 1;
}

