#include "Engine/Utils/ByteUtils.hpp"

bool CycleGreaterThanEqual(uint16_t a, uint16_t b)
{
	uint16_t diff = a - b;
	return (diff >= 0 && diff < 0x7fff);
}

bool CycleGreaterThan(uint16_t a, uint16_t b)
{
	uint16_t diff = a - b;
	return (diff > 0 && diff < 0x7fff);
}