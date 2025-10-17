// Print the exact decimal value of a C float
//
// Examples
//
// ./a.out 1.23
// 1.230000019073486328125
//
// ./a.out 0x7f7fffff
// 340282346638528859811704183484516925440.0

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DEBUG 0

void unpack(float d, uint32_t *u);
void setbit(uint32_t *u, int k);
int mulby10(uint32_t *u);
int divby10(uint32_t *u);

// 39 leading digits max
// 1 decimal point at buf[39]
// 149 trailing digits max
// 1 null terminator

char buf[200];

int
main(int argc, char *argv[])
{
	int k;
	float d;
	uint32_t u[9]; // bigint

	if (argc < 2)
		exit(1);

	if (strncmp(argv[1], "0x", 2) == 0)
		sscanf(argv[1] + 2, "%x", (int *) &d);
	else
		sscanf(argv[1], "%g", &d);

#if DEBUG
	printf("%e\n", d);
#endif

	unpack(d, u);

#if DEBUG
	printf("%x %x %x %x . %x %x %x %x %x\n", u[8], u[7], u[6], u[5], u[4], u[3], u[2], u[1], u[0]);
#endif

	// trailing digits

	k = 40;
	do
		buf[k++] = mulby10(u) + '0';
	while (u[0] || u[1] || u[2] || u[3] || u[4]);

	// leading digits

	k = 39;
	do
		buf[--k] = divby10(u + 5) + '0';
	while (u[5] || u[6] || u[7] || u[8]);

	// print result

	buf[39] = '.';
	if (*((uint32_t *) &d) & 0x80000000) // sign bit
		putchar('-');
	puts(buf + k);
}

void
unpack(float d, uint32_t *u)
{
	int x;
	uint32_t m, n;
	n = *((uint32_t *) &d);
	x = (n >> 23) & 0xff; // exponent
	m = n & 0x7fffff; // 23 bit mantissa
	if (x == 0xff) {
		if (m == 0)
			(n & 0x80000000) ? puts("-inf") : puts("inf");
		else
			puts("nan");
		exit(1);
	}
	memset(u, 0, 36); // 9 words = 36 bytes
	if (x)
		setbit(u, x--); // leading 1
	while (m) {
		if (m & 0x400000) // test leading bit
			setbit(u, x);
		x--;
		m <<= 1;
	}
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
	for (i = 0; i < 5; i++) {
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
	for (i = 3; i >= 0; i--) {
		r = r << 32 | u[i];
		u[i] = (uint32_t) (r / 10);
		r -= (uint64_t) 10 * u[i];
	}
	return (int) r;
}
