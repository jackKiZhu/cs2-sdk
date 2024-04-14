#pragma once

struct ResourceBinding_t;

class CResourceSystem
{
public:
  void* QueryInterface(const char* interfaceName);
};

class CResourceHandleUtils
{
public:
    void DeleteResource(const ResourceBinding_t* binding);
};
