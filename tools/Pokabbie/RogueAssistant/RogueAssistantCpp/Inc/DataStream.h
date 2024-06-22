#pragma once
#include "Defines.h"

#include <fstream>
#include <vector>

class DataStream
{
public:
	DataStream();
	DataStream(std::fstream& inStream);

	inline u8* GetData() { return m_Data.data(); }
	inline u8 const* GetData() const { return m_Data.data(); }
	inline size_t GetSize() const { return m_Data.size(); }

	inline bool IsWrite() const { return m_IsWrite; }
	inline bool IsRead() const { return !m_IsWrite; }

	bool Serialize(u8& val);
	bool Serialize(s8& val);
	bool Serialize(u16& val);
	bool Serialize(s16& val);
	bool Serialize(u32& val);
	bool Serialize(s32& val);
	bool Serialize(u64& val);
	bool Serialize(s64& val);

	bool Serialize(u8* data, size_t size);

private:
	size_t m_Pos;
	std::vector<u8> m_Data;

	bool m_IsWrite;
};