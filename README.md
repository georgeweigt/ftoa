Print the exact decimal value of a C float

Build and run
```
make
./a.out 1.23
```

Examples

Most decimals are approximated
```
./a.out 0.1
0.100000001490116119384765625
```

Largest integer that fits in 24 bits
```
./a.out 16777215
16777215.0
```

This fits because it is a power of 2
```
./a.out 16777216
16777216.0
```

Now losing precision
```
./a.out 16777217
16777216.0
```

But this larger integer okay because the least significant bit is zero
```
./a.out 16777218
16777218.0
```

Biggest float
```
./a.out 0x7f7fffff
340282346638528859811704183484516925440.0
```

Smallest float
```
./a.out 0x1
0.00000000000000000000000000000000000000000000140129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125
```

See also [IEEE-754 Floating Point Converter](https://www.h-schmidt.net/FloatConverter/IEEE754.html)
