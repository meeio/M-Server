#include "buffer.hh"

#include <algorithm>
#include <sys/uio.h>
#include <unistd.h>

namespace m
{

size_t buffer::read_socket(socket& sok)
{
    char extra_buffer[64 * 1024];
    const size_t buf_writeable = writeable_bytes();

    iovec vec[2];
    vec[0].iov_base = write_begin();
    vec[0].iov_len  = buf_writeable;
    vec[1].iov_base = extra_buffer;
    vec[1].iov_len  = sizeof extra_buffer;

    // ! here readv is a scatter/gather IO
    // if have many buffer to read, only need to call 'read' once.
    const size_t total_readed = sok.readv(vec, 2);
    if (total_readed > 0)
    {
        if (buf_writeable > total_readed)
        {
            write_idx_ += total_readed;
        }
        else
        {
            write_idx_ = buffer_.size();
            append(extra_buffer, total_readed - buf_writeable);
        }
        
    }
    return total_readed;
}

void buffer::make_space(size_t size)
{
    if (prependable_bytes() + writeable_bytes() < size + PRETEND_SIZE)
    {
        buffer_.resize(size + PRETEND_SIZE);
    }
    else
    {
        size_t writeable = writeable_bytes();
        std::move(read_begin(), write_begin(), begin() + PRETEND_SIZE);
        read_idx_  = PRETEND_SIZE;
        write_idx_ = read_idx_ + writeable;
        assert(writeable_bytes() == writeable);
    }
}

void buffer::ensure_writeable_bytes(size_t size)
{
    if (writeable_bytes() < size)
    {
        make_space(size);
    }
    assert(writeable_bytes() > size);
}

} // namespace m
