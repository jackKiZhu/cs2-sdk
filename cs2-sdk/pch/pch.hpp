#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include <source_location>
#include <functional>
#include <optional>
#include <iostream>
#include <format>
#include <vector>
#include <thread>
#include <mutex>
#include <span>

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#elif __linux__
#include <dlfcn.h>
#include <link.h>
#include <string.h>
#endif

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define PAD(size) std::byte STR_MERGE(_pad, __COUNTER__)[size]
#define MEMBER(type, name, offset) \
    struct {                       \
        PAD(offset);               \
        type name;                 \
    }
