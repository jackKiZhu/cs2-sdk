#pragma once

#include <types/utlmemory.hpp>

class CUtlBuffer {
   public:
    CUtlBuffer(const void* buffer, int size, uint8_t flags);
    CUtlBuffer(int a2 = 0, int a3 = 0, uint8_t a4 = 0);
    ~CUtlBuffer();
    void EnsureCapacity(int a2);

    enum BufferFlags_t {
        TEXT_BUFFER = 0x1,        // Describes how get + put work (as strings, or binary)
        EXTERNAL_GROWABLE = 0x2,  // This is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow
                                  // happens when Putting.
        CONTAINS_CRLF = 0x4,      // For text buffers only, does this contain \n or \n\r?
        READ_ONLY = 0x8,          // For external buffers; prevents null termination from happening.
        AUTO_TABS_DISABLED = 0x10,  // Used to disable/enable push/pop tabs
    };

    // Overflow functions when a get or put overflows
    typedef bool (CUtlBuffer::*UtlBufferOverflowFunc_t)(int size);

    CUtlMemory<unsigned char> m_Memory;
    int m_Get;
    int m_Put;

    unsigned char m_Error;
    unsigned char m_Flags;
    unsigned char m_Reserved;

    int m_nTab;
    int m_nMaxPut;
    int m_nOffset;

    UtlBufferOverflowFunc_t m_GetOverflowFunc;
    UtlBufferOverflowFunc_t m_PutOverflowFunc;

    // PAD(0x50);
    PAD(0x50 - 0x8);
};
