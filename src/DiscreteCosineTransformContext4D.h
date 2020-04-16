#if !defined(__DCTCX4D_H__)
#define __DCTCX4D_H__

#include <spdlog/spdlog.h>

#include "TransformContext.h"
#include "DiscreteCosineTransformContext.h"
#include "Point4D.h"


#define FORWARD 1
#define INVERSE 2



/**
 * Method used to apply some transformation across a single 
 * dimension given by a parameter `axis`
 * As long as `size` is consistent, all strides can be
 * calculated correctly. 
 */
template <typename T>
void separable_transform_across_axis(
    uint direction,
    TransformContext<T> *ctx,
    const T *input,
    T *output,
    const size_t *size,
    const size_t *stride,
    size_t axis
)
{
    // dims are all other dimensions but the one given to the function.
    // The transform applied on the X axis, for instance, will loop 
    // through Y, U and V. 
    // This array holds, then, all the other dimensions.
    uint dims[3];

    // populate dims 
    for (int j=0, i = 0; i < 4; i++) {
        if (i != axis)
            dims[j++] = i;
    }



    // Iterating accross all other axis.
    // Outter loop holds bigger stride steps.
    for (uint d3 = 0; d3 < size[dims[2]]; ++d3) {
        for (uint d2 = 0; d2 < size[dims[1]]; ++d2) {
            for (uint d1 = 0; d1 < size[dims[0]]; ++d1) {
                uint index = d1 * stride[dims[0]] + d2 * stride[dims[1]] + d3 * stride[dims[2]];
                switch (direction)
                {
                case FORWARD:
                    ctx->forward(input + index, output + index);
                    break;
                case INVERSE:
                    ctx->inverse(input + index, output + index);
                    break;
                default:
                    break;
                }
                
            }
        }
    }
}

template <typename T>
class DiscreteCosineTransformContext4D 
: public TransformContext<T>
{
private:
    bool should_delete_pointers = false;

public:

   ~DiscreteCosineTransformContext4D() {
        if (should_delete_pointers) {
            delete[] TransformContext<T>::size;
            delete[] TransformContext<T>::stride;
        }
    }
    DiscreteCosineTransformContext4D(size_t *size_, size_t *stride_) 
    {
        TransformContext<T>::size = size_;
        TransformContext<T>::stride = stride_;
    }
    DiscreteCosineTransformContext4D(Point4D &size_, Point4D &stride_) {
        TransformContext<T>::size = size_.to_array();
        TransformContext<T>::stride = stride_.to_array();
        should_delete_pointers = true;
    }
    
    void forward(const T *input, T *output)
    {
        forward(input, output, TransformContext<T>::size);
    }

    void forward(const T *input, T *output, const size_t * size)
    {
        TransformContext<T> *ctx;
        auto *stride = TransformContext<T>::stride;
        size_t FULL_LENGTH = stride[3] * size[3];
        T *partial_result = new T[FULL_LENGTH];

        // For the first round, the "last round output" is simple
        // the input values to be calculated.
        T *pout = (T*)input;
        
        for (size_t dim = 0; dim < 4; dim++)
        {
            ctx = new DiscreteCosineTransformContext<T>(size[dim], stride[dim]);
            
            // Copy all values from last round into`partial_result`.
            std::copy(pout, pout + FULL_LENGTH, partial_result);
            
            separable_transform_across_axis(FORWARD,
                ctx, partial_result, output, size, stride, dim);

            delete ctx;
            pout = output;
        }
        delete[] partial_result;
    }

    void inverse(const T *input, T *output)
    {
        inverse(input, output, TransformContext<T>::size);
    }
    void inverse(const T *input, T *output, const size_t * size)
    {
        TransformContext<T> *ctx;
        auto *stride = TransformContext<T>::stride;
        size_t FULL_LENGTH = stride[3] * size[3];
        T *partial_result = new T[FULL_LENGTH];

        // For the first round, the "last round output" is simple
        // the input values to be calculated.
        T *pout = (T*)input;
        
        for (int dim = 3; dim >= 0; dim--)
        {
            ctx = new DiscreteCosineTransformContext<T>(size[dim], stride[dim]);
            
            // Copy all values from last round into`partial_result`.
            std::copy(pout, pout + FULL_LENGTH, partial_result);
            
            separable_transform_across_axis(INVERSE, 
                ctx, partial_result, output, size, stride, dim);

            delete ctx;
            pout = output;
        }
        delete[] partial_result;
    }
};



#endif // __DCTCX4D_H__
