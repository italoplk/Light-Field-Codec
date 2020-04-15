#if !defined(__TRANSFORMCONTEXT_H__)
#define __TRANSFORMCONTEXT_H__

#include <cstdlib>

template <typename T>
class TransformContext {
public:
    const size_t *size;
    const size_t *stride;
    /* Abstract method to be implemented during inheritance */
    virtual void forward(const T *input, T *output) = 0;
    /* Abstract method to be implemented during inheritance */
    virtual void inverse(const T *input, T *output) = 0;

};



#endif // __TRANSFORMCONTEXT_H__
