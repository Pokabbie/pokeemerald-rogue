#pragma once

#include <memory>

void memcpy(void* dst, void* src, size_t size)
{
	std::memcpy(dst, src, size);
}