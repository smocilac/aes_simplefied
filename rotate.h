#ifndef ROTATE_H_INCLUDED
#define ROTATE_H_INCLUDED
#include <iostream>

// Rotacija desno (kopirano sa three fish službene implementacije)
template <typename T> T ROR(T x, const uint64_t & n, const uint64_t & bits)
{
	T mask = 1;
	mask = (mask << (n - 1)) - 1;
	mask = (mask << 1) + 1;
	T mask_bits = x & mask;
    return (x >> n) | (mask_bits << (bits - n));
}

// Rotacija lijevo
template <typename T> T ROL(const T & x, const uint64_t & n, const uint64_t & bits)
{
	return ROR(x, bits - n, bits);
}


#endif // ROTATE_H_INCLUDED
