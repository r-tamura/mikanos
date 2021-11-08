#pragma once

#include <cstring> // Note: size_tに必要

class FileDescriptor {
  public:
    virtual ~FileDescriptor() = default;
    virtual size_t Read(void* buf, size_t len) = 0;
};