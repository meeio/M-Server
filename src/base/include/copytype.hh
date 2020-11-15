#ifndef __COPYTYPE_H__
#define __COPYTYPE_H__

namespace m
{

class noncopyable
{
public:
    noncopyable& operator=(const noncopyable&) = delete;
};
    
} // namespace m
                                                            

#endif // __COPYTYPE_H__

 