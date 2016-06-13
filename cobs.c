#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "cobs.h"

ssize_t
cobs_encode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen)
{
	uint8_t *cp, *esrc, *edst;
	uint8_t code;

	edst = dst + dstcap;
	esrc = src + srclen;
	code = 1;
	cp = dst++;
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

	// we use a 16-bit end marker, so that it will serve double-duty as
	// a byte boundary synchronizer on least-significant-bit first
	// serial lines, eg. '10000000 00000000' when time increases to the right
	*dst++ = 1;
	*dst++ = 0;
	return dstcap - (edst - dst);
}

ssize_t
cobs_decode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen, size_t *nrd)
{
	uint8_t i, code;
	uint8_t *edst, *esrc;

	edst = dst + dstcap;
	esrc = src + srclen;
	while(src < esrc && dst < edst){
		code = *src++;
		if(code == 0){ // message end
output:
			if(nrd != NULL)
				*nrd = srclen - (esrc - src);
			return dstcap - (edst - dst) - 2;
		}
		if((edst - dst) >= code-1 && (esrc - src) >= code-1){
			for(i = 0; i < code-1; i++){
				dst[i] = src[i];
				if(src[i] == 0){
					// this is an unexpected zero, so technically the
					// message is corrupt. we don't know why it is corrupt,
					// we break out here to synchronize with every zero we see
					dst += i;
					src += i+1;
					goto output;
				}
			}
			dst += i;
			src += i;
		} else {
			return -1;
		}
		// if code is 255, it is a continuation byte we skip over.
		// otherwise, it's a payload zero.
		if(code != 255){
			if(edst == dst){
				return -1;
			}
			*dst++ = 0;
		}
	}
	return -1;
}

