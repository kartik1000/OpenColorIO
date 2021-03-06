// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenColorIO Project.

#include <sstream>
#include <vector>

#include <OpenColorIO/OpenColorIO.h>

#include "Platform.h"

#ifndef _WIN32
#include <chrono>
#include <random>
#endif

namespace OCIO_NAMESPACE
{

const char * GetEnvVariable(const char * name)
{
    static std::string value;
    Platform::Getenv(name, value);
    return value.c_str();
}

void SetEnvVariable(const char * name, const char * value)
{
    Platform::Setenv(name, value);
}


namespace Platform
{
void Getenv (const char * name, std::string & value)
{
#ifdef _WIN32
    if(uint32_t size = GetEnvironmentVariable(name, nullptr, 0))
    {
        std::vector<char> buffer(size);
        GetEnvironmentVariable(name, buffer.data(), size);
        value = std::string(buffer.data());
    }
    else
    {
        value.clear();
    }
#else
    const char* val = ::getenv(name);
    value = (val && *val) ? val : "";
#endif 
}

void Setenv (const char * name, const std::string & value)
{
#ifdef _WIN32
    _putenv_s(name, value.c_str());
#else
    ::setenv(name, value.c_str(), 1);
#endif 
}

int Strcasecmp(const char * str1, const char * str2)
{
#ifdef _WIN32
    return ::_stricmp(str1, str2);
#else
    return ::strcasecmp(str1, str2);
#endif
}

int Strncasecmp(const char * str1, const char * str2, size_t n)
{
#ifdef _WIN32
    return ::_strnicmp(str1, str2, n);
#else
    return ::strncasecmp(str1, str2, n);
#endif
}

void* AlignedMalloc(size_t size, size_t alignment)
{
#ifdef _WIN32
    void* memBlock = _aligned_malloc(size, alignment);
    return memBlock;
#else
    void* memBlock = 0x0;
    if (!posix_memalign(&memBlock, alignment, size)) return memBlock;
    return 0x0;
#endif
}

void AlignedFree(void* memBlock)
{
#ifdef _WIN32
    _aligned_free(memBlock);
#else
    free(memBlock);
#endif
}

void CreateTempFilename(std::string & filename, const std::string & filenameExt)
{
    // Note: Because of security issue, tmpnam could not be used.

#ifdef _WIN32

    char tmpFilename[L_tmpnam];
    if(tmpnam_s(tmpFilename))
    {
        throw Exception("Could not create a temporary file.");
    }

    filename = tmpFilename;

#else

    std::stringstream ss;
    ss << "/tmp/ocio";

    // Obtain a seed from the system clock.
    const unsigned seed 
        = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    // Use the standard mersenne_twister_engine.
    std::mt19937 generator(seed);
    ss << generator();

    filename = ss.str();

#endif

    filename += filenameExt;
}


} // Platform

} // namespace OCIO_NAMESPACE

