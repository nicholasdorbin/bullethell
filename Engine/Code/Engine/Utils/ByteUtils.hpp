#pragma once
#include <stdint.h>

typedef unsigned char byte_t;
#define INVALID_BYTE 0xff
#define BIT(x) (1<<(x))

bool CycleGreaterThanEqual(uint16_t a, uint16_t b);
bool CycleGreaterThan(uint16_t a, uint16_t b);

