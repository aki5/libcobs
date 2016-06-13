# Consistent Overhead Byte Stuffing (COBS)

[![Build Status](https://travis-ci.org/aki5/libcobs.svg?branch=master)](https://travis-ci.org/aki5/libcobs)

Libcobs is a simple implementation of the consistent overhead byte stuffing algorithm for eliminating occurrences of the `0`-byte in a message. By eliminating the `0`-bytes from the payload, it becomes possible to use them for (re-)synchronizing a decoder to message boundaries.

Well known alternatives to COBS include bit-stuffing as used in HDLC and byte stuffing as used in PPP. The benefit of COBS is a lower worst case overhead than bit- or byte stuffing, without the software implementation challenges of bit stuffing.

Libcobs implements the end-of-message terminator as the two-byte sequence `0x01 0x00` so that it can be used for alignment at either the bit- or the byte level. This can be useful with synchronous serial lines that have no byte framing.

The idea is described concisely in the paper [Consistent overhead Byte stuffing](http://www.stuartcheshire.org/papers/cobsforton.pdf), with plenty of discussion and more advanced features. Libcobs provides a simple implementation of the basic insight.

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