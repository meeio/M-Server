#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "copytype.hh"
#include "socket.hh"

#include <string>
#include <vector>
#include <assert.h>
namespace m
{


/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
///
class buffer
    : noncopyable
{
public:
    const static size_t PRETEND_SIZE = 8;
    const static size_t INIT_SIZE    = 1024;

    buffer() 
        : buffer_(PRETEND_SIZE + INIT_SIZE)
        , read_idx_(PRETEND_SIZE)
        , write_idx_(PRETEND_SIZE)
    {
        assert(readable_bytes() == 0);
        assert(writeable_bytes() == INIT_SIZE);
        assert(prependable_bytes() == PRETEND_SIZE);
    }

    /* -------------------- SIZE AND COPACITY -------------------- */

    size_t writeable_bytes() const { return buffer_.size() - write_idx_; }

    size_t readable_bytes() const { return write_idx_ - read_idx_; }

    size_t prependable_bytes() const { return read_idx_; }

    /* ----------------------- MEMBER ACESS ---------------------- */

    const char* peek() const { return begin() + read_idx_; }

    /* ----------------------- MODIFIERS ------------------------ */

    size_t read_socket(socket&);

    void append(const char* data, size_t len)
    {
        ensure_writeable_bytes(len);
        std::copy(data, data + len, write_begin());
    }

    void append(const std::string& str)
    {
        append(str.data(), str.size());
    }

    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void retrieve(size_t len)
    {
        assert(len < readable_bytes());
        read_idx_ += len;
    }

    void retrieve_all()
    {
        read_idx_ = PRETEND_SIZE;
        write_idx_ = PRETEND_SIZE;
    }

    std::string retrieve_as_string()
    {
        std::string str(peek(), readable_bytes());
        retrieve_all();
        return str;
    }

private:
    /* ------------------------ ITERATORS ------------------------ */

    const buffer* cthis() { return const_cast<const buffer*>(this); }

    const char* begin() const { return &*buffer_.begin(); }
    char*       begin() { return const_cast<char*>(cthis()->begin()); }

    const char* write_begin() const { return begin() + write_idx_; }
    char*       write_begin() { return const_cast<char*>(cthis()->write_begin()); }

    const char* read_begin() const { return begin() + read_idx_; }
    char*       read_begin() { return const_cast<char*>(cthis()->read_begin()); }

    /* -------------------- SIZE AND COPACITY -------------------- */

    void make_space(size_t size);

    void ensure_writeable_bytes(size_t size);

    std::vector<char> buffer_;
    size_t            read_idx_;
    size_t            write_idx_;
};

} // namespace m

#endif // __BUFFER_H__