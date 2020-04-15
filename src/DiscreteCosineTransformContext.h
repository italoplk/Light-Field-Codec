#if !defined(__DISCRETECOSINETRANSFORMCONTEXT_H__)
#define __DISCRETECOSINETRANSFORMCONTEXT_H__

#include <map>
#include <stdexcept>
#include <cmath>
#include "TransformContext.h"


template <typename T>
class DiscreteCosineTransformContext : public TransformContext<T> {
public:
    static std::map<int, const T*> coeff_cache;


    /* Generate the DCT coefficients.
     * Never checks whether they were created previously or not. */
    static const T *generate_dct_coeff(const size_t size)
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

    /* Search in the cache for the coefficients by a given dimension.
     * If one is not found, generate a new one and return.
     */
    static const T *get_coeff(const size_t size)
    {
        // BUG: Race conditions?
        try {
            // Just returns whatever is stored at coeff_cache[size].
            return coeff_cache.at(size);
        } catch(const std::out_of_range& e) {
            // Expected exception for cases where coeff_cache[size] is invalid.
            const T* coeff = generate_dct_coeff(size);
            coeff_cache[size] = coeff;
            return coeff;
        }
    }

public:
    const size_t size;
    const size_t stride;

    DiscreteCosineTransformContext(size_t size_, size_t stride_ = 1)
    : size(size_), stride(stride_)
    {

    }   
    /* Forward application of the DST on a single dimension */
    void forward(const T *input, T *output)
    {
        dct(input, output, size, stride);
    }

    /* Inverse application of the DST on a single dimension */
    void inverse(const T *input, T *output)
    {   
        idct(input, output, size, stride);
    }

    // Silently drops every entry in coeff_cache. 
    static void flush_coeff()
    {
        for (const auto& kv : coeff_cache) {
            delete[] kv.second;
            coeff_cache.erase(kv);
        }
    }
    
    // 1D DCT II implementation.
    // TODO: Improve this algorithm to use vector notaton.
    static void dct(const T *in, T *out, const size_t size, const size_t stride)
    {
        const T *pin;
        T *pout = out;
        const T *coeff = get_coeff(size);

        // out[k] = in[k] * coeff[k][n] for n=0..size
        for (int k = 0; k < size; ++k) {
            double sum = 0;
            pin = in;
            
            for (int n = 0; n < size; ++n) {
                sum += *pin * *coeff;
                pin += stride;
                coeff++;
            }

            *pout = sum;
            pout += stride;
        }
    }

    // 1D DCT III implementation.
    // TODO: Improve this algorithm to use vector notaton.
    static void idct(const T *in, T *out, const size_t size, const size_t stride)
    {
        const T *pin;
        T *pout = out;
        const T *pcoeff;
        const T *coeff = get_coeff(size);

        // out[k] = in[k] * coeff[n][k] for n=0..size
        for (int k = 0; k < size; ++k) {
            float sum = 0;
            float partial_sums[size];
            pin = in;
            pcoeff = &coeff[k];
            for (int n = 0; n < size; ++n) {
                sum += *pin * *pcoeff;
                pin += stride;
                pcoeff += size;
                partial_sums[n] = sum;
            }
            *pout = sum;
            pout += stride;
        }
    }
};


template <typename T>
std::map<int, const T*> DiscreteCosineTransformContext<T>::coeff_cache;


#endif // __DISCRETECOSINETRANSFORMCONTEXT_H__
