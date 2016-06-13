# Consistent Overhead Byte Stuffing (COBS)

[![Build Status](https://travis-ci.org/aki5/libcobs.svg?branch=master)](https://travis-ci.org/aki5/libcobs)

Libcobs is a simple implementation of the consistent overhead byte stuffing algorithm for using the `0`-byte as a message terminator, and eliminating them from elsewhere in the data stream.

Libcobs implements the end-of-message terminator with the two-byte sequence `0x01 0x00` so that this same pattern can be used to align to byte boundaries on a serial line.

The idea is described in the paper [Consistent overhead Byte stuffing](http://www.stuartcheshire.org/papers/cobsforton.pdf)

```
Cheshire, S. and Baker, M. 1999. Consistent overhead Byte stuffing.
IEEE/ACM Trans. Netw. 7, 2 (Apr. 1999), 159-172.
```


### Encoding and decoding API

The API has two calls, once for decoding and another one for encoding.
Both calls read input from the `src` buffer and write their results to the `dst` buffer. The return value tells how many bytes were written to `dst`, or `-1` in case an error occurred.

The `nrd` argument for `cobs_decode` returns the number of bytes read from `src`, and can be passed as NULL.

```
ssize_t cobs_encode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen);
ssize_t cobs_decode(uint8_t *dst, size_t dstcap, uint8_t *src, size_t srclen, size_t *nrd);
```

The implementation has good performance, but is intended to compile cleanly on anything from 8-bit microcontrollers to top end x86-64 machines. I wrote this code a rather long time ago, but I've continue use it regularly where frame synchronization is not guaranteed by hardware.