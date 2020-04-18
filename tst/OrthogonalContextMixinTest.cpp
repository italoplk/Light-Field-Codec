#include <gtest/gtest.h>

#include <cmath>
#include "TransformContext.h"
#include "DiscreteCosineTransformContext.h"
#include "DiscreteSineTransformContext.h"

#define ERROR_ENERGY_EPSILON 1

class OrthogonalContextMixinTest : public ::testing::Test
{
public:
    TransformContext<float> *ctx;
    float *signal;
    float *transformed_signal;

    ~OrthogonalContextMixinTest()
    {
        delete ctx;
        delete[] signal;
        delete[] transformed_signal;
    }

    void init_arrays(size_t size)
    {
        signal = new float[size];
        transformed_signal = new float[size];

        // Seed the random number generator
        std::srand(0);

        // Populate and initialize arrays for a clean environment.
        for (int i = 0; i < size; i++)
        {
            float value = 256 * (std::rand() / (RAND_MAX * 1.0));
            signal[i] = value;
            transformed_signal[i] = 0;
        }
    }

    double calculate_energy(float *array, size_t size)
    {
        double energy = 0;
        for (int i = 0; i < size; i++)
            energy += pow(array[i], 2);
        return sqrt(energy);
    }
};

TEST_F(OrthogonalContextMixinTest, dct_is_orthogonal)
{
    const size_t FULL_LENGTH = 1024;
    init_arrays(FULL_LENGTH);

    ctx = new DiscreteCosineTransformContext<float>(FULL_LENGTH);
    ctx->forward(signal, transformed_signal);

    EXPECT_NEAR(calculate_energy(signal, FULL_LENGTH),
                calculate_energy(transformed_signal, FULL_LENGTH),
                ERROR_ENERGY_EPSILON);
}

TEST_F(OrthogonalContextMixinTest, dst_is_orthogonal)
{
    const size_t FULL_LENGTH = 1024;
    init_arrays(FULL_LENGTH);

    ctx = new DiscreteSineTransformContext<float>(FULL_LENGTH);
    ctx->forward(signal, transformed_signal);

    EXPECT_NEAR(calculate_energy(signal, FULL_LENGTH),
                calculate_energy(transformed_signal, FULL_LENGTH),
                ERROR_ENERGY_EPSILON);
}