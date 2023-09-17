#pragma once
#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "StringUtils.h"

std::string const c_TabSpacing = "    ";

void FatalExit();

#ifdef _MSC_VER

#define FATAL_ERROR(format, ...)          \
do                                        \
{                                         \
    fprintf(stderr, format, __VA_ARGS__); \
    FatalExit();                          \
} while (0)

#define LOG_MSG(format, ...)              \
do                                        \
{                                         \
    fprintf(stderr, format "\n", __VA_ARGS__); \
} while (0)

#else

#define FATAL_ERROR(format, ...)            \
do                                          \
{                                           \
    fprintf(stderr, format, ##__VA_ARGS__); \
    FatalExit();                           \
} while (0)

#define LOG_MSG(format, ...)                \
do                                          \
{                                           \
    fprintf(stderr, format "\n", ##__VA_ARGS__); \
} while (0)

#endif // _MSC_VER
