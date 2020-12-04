#ifndef __COPYTYPE_H__
#define __COPYTYPE_H__

namespace m
{

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};
    
} // namespace m
                                                            

#endif // __COPYTYPE_H__

 