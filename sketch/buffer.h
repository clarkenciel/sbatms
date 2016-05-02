#pragma once

class Buffer
{
  enum buf_state_t {
    LEADER = 0,
    MESSAGE,
    NUM_STATES
  };

  public:
    Buffer ();
    ~Buffer () {};

    void print ();
    void clear ();

    uint16_t length () { return mBufLen; }
    const uint32_t * values () { return mBuffer; }

  private:
    buf_state_t mBState;
    uint16_t mBufWritePtr;
    uint16_t mBufLen;
    uint32_t * mBuffer;


};

/* Comparison operator */
inline bool 
operator== (const Buffer& b1, const Buffer& b2)
{
  uint16_t b1Len = b1.length();
  if (b1Len != b2.length())
    return false;
  else
  {
    const uint32_t * b1Buf = b1.values();
    const uint32_t * b2Buf = b2.values();

    for (uint16_t i = 0; i < b1Len; i++)
    {
      if (b1Buf[i] != b2Buf[i])
        return false;
    }
    return true;
  }
}
  
