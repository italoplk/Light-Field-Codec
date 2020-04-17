#if !defined(__DISCRETECOSINETRANSFORMCONTEXT_H__)
#define __DISCRETECOSINETRANSFORMCONTEXT_H__

#include <map>
#include <stdexcept>
#include <cmath>
#include "TransformContext.h"
#include "OrthogonalTransformMixin.h"

template <typename T>
class _DiscreteCosineTransformContext : public TransformContext<T>
{
private:
    const size_t _size;
    const size_t _stride;
protected:
    static std::map<int, const T*> coeff_cache;

    /* Implementation of the helper method to calculate the DCT coefficients */
    static const T *generate_coefficients(const size_t size)
    {
        auto N = size;
        auto *output = new T[N * N];
        auto *p_output = output;
        for (int k = 0; k < N; k++) {
            double s = (k == 0) ? 1 / (double) sqrt(N) : (sqrt(((double) 2 / N)));

            for (int n = 0; n < N; n++)
                *p_output++ = s * (double) cos((double) M_PI * (2 * n + 1) * k / (2 * N));
        }
        return output;
    }
    _DiscreteCosineTransformContext(size_t size_, size_t stride_ = 1)
    : _size(size_), _stride(stride_)
    {
        // Update parent pointers to correct values.
        TransformContext<T>::size = &_size;
        TransformContext<T>::stride = &_stride;
    }
    _DiscreteCosineTransformContext(size_t * size_, size_t * stride_)
    {
        // Update parent pointers to correct values.
        _size = *size_;
        _stride = *stride_;
        TransformContext<T>::size = &_size;
        TransformContext<T>::stride = &_stride;
    } 
};


/** Declaration of static member */
template <typename T>
std::map<int, const T*> _DiscreteCosineTransformContext<T>::coeff_cache;

/** Exports the name DiscreteCosineTransformContext with the mixin 
 * OrthogonalTransformContext */
template <typename T>
using DiscreteCosineTransformContext = OrthogonalTransformMixin<_DiscreteCosineTransformContext<T>>;



#endif // __DISCRETECOSINETRANSFORMCONTEXT_H__
