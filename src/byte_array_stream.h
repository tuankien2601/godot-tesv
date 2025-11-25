#ifndef BYTE_ARRAY_STREAM
#define BYTE_ARRAY_STREAM

#include <streambuf>

class ByteArrayStream : public std::streambuf {
public:
    ByteArrayStream(const char* data, std::size_t size) {
        char* p = const_cast<char*>(data);
        setg(p, p, p + size); // Set the get area pointers
    }

protected:
    int underflow() override {
        // No more data to fetch, as we've already set the entire buffer
        return EOF; 
    }
};

#endif