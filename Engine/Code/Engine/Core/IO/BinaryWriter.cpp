typedef unsigned char byte_t;
/*
enum eEndianMode
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

eEndianMode GetLocalEndianess()
{
	union
	{
		byte_t bdata[4];
		uint32_t uidata;
	} data;

	data.uidata = 0x04030201;
	return (data.bdata[0] == 0x01)
		? LITTLE_ENDIAN
		: BIG_ENDIAN
};


class IBinaryWriter
{
private:
	eEndianMode endian_mode;
public:
	// returns number of bytes written
	virtual size_t WriteBytes(void const *src, size_t const num_bytes) = 0;

	void SetEndianess(eEndianMode mode)
	{
		endian_mode = mode;
	}

	// I want to be able to...
	// - write nullptr
	// - write empty strings
	// - write normal strings
	bool WriteString(char const *string)
	{
		// write length of the buffer
		if (nullptr == string) {
			Write<uint32_t>(0U);
			return true;
		}

		size_t len = strlen(string);
		size_t buf_len = len + 1;
		return Write<uint32_t>(buf_len)
			&& (WriteBytes(string, buf_len) == buf_len);
	}

	// 
	template <typename T>
	bool Write(T const &v)
	{
		T copy = v;
		if (GetLocalEndianess() != mode) {
			ByteSwap(&copy, sizeof(T));
		}
		return WriteBytes(&copy, sizeof(T)) == sizeof(T);
	}
};

class IBinaryReader
{
public:
	...

		template <typename>
	bool Read(T *v) { ... }

};

void ByteSwap(void *data, size_t const data_size)
{
	...
}
*/