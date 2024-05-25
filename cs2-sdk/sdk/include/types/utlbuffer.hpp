#pragma once

class CUtlBuffer {
   public:
    CUtlBuffer(int a2, int a3, char a4);
    ~CUtlBuffer();
    void EnsureCapacity(int a2);

   private:
    PAD(0x80);
};
