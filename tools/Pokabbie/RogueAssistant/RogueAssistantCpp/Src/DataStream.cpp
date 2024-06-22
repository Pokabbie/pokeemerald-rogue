#include "DataStream.h"
#include "Log.h"

DataStream::DataStream()
	: m_Pos(0)
	, m_IsWrite(true)
{
}

DataStream::DataStream(std::fstream& inStream)
	: m_Pos(0)
	, m_IsWrite(false)
{
	while (inStream.good())
	{
		char temp;
		inStream.read(&temp, 1);
		m_Data.push_back(temp);
	}
}

template<typename T>
static bool SerializeInternalWrite(std::vector<u8>& data, size_t& pos, T& val)
{
	u8* ptr = reinterpret_cast<u8*>(&val);

	for (size_t i = 0; i < sizeof(T); ++i)
		data.push_back(ptr[i]);

	pos += sizeof(T);
	return true;
}

template<typename T>
static bool SerializeInternalRead(std::vector<u8>& data, size_t& pos, T& val)
{
	// Fail
	if (pos + sizeof(T) > data.size())
	{
		val = 0;
		return false;
	}

	u8* ptr = reinterpret_cast<u8*>(&val);

	for (size_t i = 0; i < sizeof(T); ++i)
		ptr[i] = data[pos++];

	return true;
}

template<typename T>
static bool SerializeInternal(std::vector<u8>& data, size_t& pos, bool isWrite, T& val)
{
	if (isWrite)
		return SerializeInternalWrite(data, pos, val);
	else
		return SerializeInternalRead(data, pos, val);
}

bool DataStream::Serialize(u8& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}
bool DataStream::Serialize(s8& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}

bool DataStream::Serialize(u16& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}
bool DataStream::Serialize(s16& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}

bool DataStream::Serialize(u32& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}
bool DataStream::Serialize(s32& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}

bool DataStream::Serialize(u64& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}
bool DataStream::Serialize(s64& val)
{
	return SerializeInternal(m_Data, m_Pos, m_IsWrite, val);
}

bool DataStream::Serialize(u8* data, size_t size)
{
	bool success = true;

	for (size_t i = 0; i < size; ++i)
	{
		if (m_IsWrite)
		{
			m_Data.push_back(data[i]);
			m_Pos++;
		}
		else
		{
			if (m_Pos < m_Data.size())
				data[i] = m_Data[m_Pos++];
			else
			{
				data[i] = 0;
				success = false;
			}
		}
	}

	return success;
}