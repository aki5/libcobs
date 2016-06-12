/*
 *	Cheshire, S. and Baker, M. 1999. Consistent overhead Byte stuffing.
 *	IEEE/ACM Trans. Netw. 7, 2 (Apr. 1999), 159-172.
 *	DOI= http://dx.doi.org/10.1109/90.769765
 */
ssize_t cobs_encode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen);
ssize_t cobs_decode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen, size_t *nrd);
