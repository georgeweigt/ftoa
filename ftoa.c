// Print the exact decimal value of a C float
//
// Examples
//
// ./a.out 1.23
// 1.230000019073486328125
//
// ./a.out 0x7f7fffff
// 340282346638528859811704183484516925440.0
//
// Copyright (c) 2025, George Weigt
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DEBUG 0

char *ftoa(float d, char *buf);
int unpack(float d, uint32_t *u, char *buf);
void setbit(uint32_t *u, int k);
int mulby10(uint32_t *u);
int divby10(uint32_t *u);

// 1 sign char
// 39 leading digits max
// 1 decimal point at buf[40]
// 149 trailing digits max
// 1 null terminator

char buf[200];

int
main(int argc, char *argv[])
{
	float d;
	char *s;

	if (argc < 2)
		exit(1);

	if (strncmp(argv[1], "0x", 2) == 0)
		sscanf(argv[1] + 2, "%x", (int *) &d);
	else
		sscanf(argv[1], "%g", &d);

	s = ftoa(d, buf);

	puts(s);
}

// returns start of string in buf

char *
ftoa(float d, char *buf)
{
	int k;
	uint32_t u[9]; // bigint

#if DEBUG
	printf("%e\n", d);
#endif

	if (unpack(d, u, buf))
		return buf; // inf or nan

#if DEBUG
	printf("%x %x %x %x . %x %x %x %x %x\n", u[8], u[7], u[6], u[5], u[4], u[3], u[2], u[1], u[0]);
#endif

	// trailing digits

	k = 41;
	do
		buf[k++] = mulby10(u) + '0';
	while (u[0] || u[1] || u[2] || u[3] || u[4]);
	buf[k] = '\0';

	// leading digits

	k = 40;
	do
		buf[--k] = divby10(u + 5) + '0';
	while (u[5] || u[6] || u[7] || u[8]);

	// sign

	if (*((uint32_t *) &d) & 0x80000000)
		buf[--k] = '-';

	buf[40] = '.';

	return buf + k;
}

// returns 1 for inf or nan

int
unpack(float d, uint32_t *u, char *buf)
{
	int x;
	uint32_t m, n;
	n = *((uint32_t *) &d);
	x = (n >> 23) & 0xff; // exponent
	m = n << 9; // 23 bit mantissa
	if (x == 0xff) {
		if (m == 0) {
			if (n & 0x80000000)
				strcpy(buf, "-inf");
			else
				strcpy(buf, "inf");
		} else
			strcpy(buf, "nan");
		return 1;
	}
	memset(u, 0, 36); // 9 words = 36 bytes
	if (x)
		setbit(u, x--); // leading 1
	while (m) {
		if (m & 0x80000000) // test leading bit
			setbit(u, x);
		x--;
		m <<= 1;
	}
	return 0;
}

void
setbit(uint32_t *u, int k)
{
	k += 33; // biggest exponent is 254 -> bit index 287
	u[k / 32] |= 1 << (k % 32);
}

// returns the leading digit

int
mulby10(uint32_t *u)
{
	int i;
	uint64_t t = 0;
	for (i = 0; i < 5; i++) { // 5 word bigint
		t += (uint64_t) u[i] * 10;
		u[i] = (uint32_t) t;
		t >>= 32;
	}
	return (int) t;
}

// returns the remainder

int
divby10(uint32_t *u)
{
	int i;
	uint64_t r = 0;
	for (i = 3; i >= 0; i--) { // 4 word bigint
		r = r << 32 | u[i];
		u[i] = (uint32_t) (r / 10);
		r -= (uint64_t) u[i] * 10;
	}
	return (int) r;
}
