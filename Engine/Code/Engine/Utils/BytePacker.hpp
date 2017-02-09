#pragma once
#include "Engine/Core/IO/EndianUtils.hpp"


class BytePacker
{
public:
	void* m_buffer;
	size_t m_maxBufferSize; //capacity of buffer. Should not change
	size_t m_currentContentSize; //size / length of buffer
	eEndianMode m_endianness;
	mutable size_t m_offset;


	BytePacker();
	BytePacker(void* buffer, const size_t currentContentSize, const size_t writeSizeMax, eEndianMode endianess = GetLocalEndianess());
	~BytePacker();




	

	size_t WriteForward(const void* data, const size_t dataSize);
	size_t WriteBackward(const void* data, const size_t dataSize);
	void RewindWrite(const size_t& amount);


	size_t ReadForward(void* outData, const size_t dataSize) const;
	size_t ReadBackwards(void* outData, const size_t dataSize) const;


	template <typename T>
	size_t Write(void* data)
	{
		const size_t dataSize = sizeof(T);
		if (GetWriteableBytes() < dataSize)
		{
			return 0u;
		}


		
		if (GetLocalEndianess() == m_endianness)
		{
			//match. Write it normally / write forward
			return WriteForward(data, dataSize); //check if you have room, if you do memcpy, advance the head
		}
		else
		{
			return WriteBackward(data, dataSize);
		}
	}


	template <typename T>
	size_t OverWrite(size_t& location, void* data)
	{
		const size_t dataSize = sizeof(T);
		size_t writtenAmount = 0u;

		size_t oldOffset = m_offset;
		m_offset = location;

		if (GetLocalEndianess() == m_endianness)
		{
			//match. Write it normally / write forward
			writtenAmount = WriteForward(data, dataSize); //check if you have room, if you do memcpy, advance the head
		}
		else
		{
			writtenAmount = WriteBackward(data, dataSize);
		}

		RewindWrite(dataSize);
		m_offset = oldOffset;
		return writtenAmount;
	}


	template <typename T>
	size_t Read(void* outData) const
	{
		const size_t dataSize = sizeof(T);
		if (GetReadableBytes() < dataSize)
		{
			return 0u;
		}


		if (GetLocalEndianess() == m_endianness)
		{
			//match. Write it normally / write forward
			return ReadForward(outData, dataSize); //check if you have room, if you do memcpy, advance the head
		}
		else
		{
			return ReadBackwards(outData, dataSize);
		}
	}



	void WriteString(const char* str);

	char const* ReadString();
	size_t AdvanceWrite(size_t size);
	size_t AdvanceRead(size_t size) const;

	void* GetHead() const; //returns m_buffer + offset
	size_t GetReadableBytes() const;
	size_t GetTotalReadableBytes() const;
	size_t GetWriteableBytes() const;
	size_t GetTotalWriteableBytes() const;
};


