#pragma once
#pragma warning( disable : 4505 )  // unreferenced local function
enum eEndianMode
{
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN,
};

static eEndianMode GetLocalEndianess()  //Windows is usually LittleEndian
{
	union
	{
		int data;
		unsigned char buffer[4];
	} test;

	test.data = 1;
	if (test.buffer[0] == 1)
	{
		return LITTLE_ENDIAN;
	}
	else
	{
		return BIG_ENDIAN;
	}
};