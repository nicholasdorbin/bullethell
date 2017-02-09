#include "Engine/Math/HashUtils.hpp"

// ----------------------------- crc32b --------------------------------

/* This is the basic CRC-32 calculation with some optimization but no
table lookup. The the byte reversal is avoided by shifting the crc reg
right instead of left and by using a reversed 32-bit word to represent
the polynomial.
When compiled to Cyclops with GCC, this function executes in 8 + 72n
instructions, where n is the number of bytes in the input message. It
should be doable in 4 + 61n instructions.
If the inner loop is strung out (approx. 5*8 = 40 instructions),
it would take about 6 + 46n instructions. */

unsigned int crc32b(unsigned char const *message, size_t const memory_size) {
	//int i, j;
	unsigned int byte, crc, mask;


	crc = 0xFFFFFFFF;
	//while (message[i] != 0) {
	for (size_t i = 0; i < memory_size; i++)
	{
		byte = message[i];            // Get next byte.
		crc = crc ^ byte;
		for ( int j = 7; j >= 0; j--) {    // Do eight times.
			mask = ~(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i = i + 1;
	}
	return ~crc;
}

unsigned int HashString(const std::string str)
{
	size_t size = sizeof(str.length()) * sizeof(unsigned char);
	return crc32b((unsigned char*)str.c_str(), size);
}