#pragma once
#include <cstdio>

#ifdef _DEBUG
#define _ASSERTS
#endif

#ifdef _DEBUG

#define LOG_INFO(format, ...) printf("[INFO]: " ## format ## "\n", __VA_ARGS__)
#define LOG_WARN(format, ...) printf("[WARNING]: " ## format  ## "\n", __VA_ARGS__)
#define LOG_ERROR(format, ...) printf("[ERROR]: " ## format  ## "\n", __VA_ARGS__)
#else

#define LOG_INFO(format, ...)
#define LOG_WARN(format, ...)
#define LOG_ERROR(format, ...)
#endif


#ifdef _ASSERTS
#define ASSERT_MSG(condition, ...) if(!(condition)) { LOG_ERROR(__VA_ARGS__); __debugbreak(); } (void*)0
#else
#define ASSERT_MSG(condition, ...)
#endif

#define ASSERT_FAIL(...) ASSERT_MSG(false, __VA_ARGS__)