#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>

#include "TransformContext.h"
#include "DiscreteCosineTransformContext.h"
#include "DiscreteSineTransformContext.h"
#include "DiscreteCosineTransformContext4D.h"
#include "utils.h"


#include <spdlog/spdlog.h>
#include <xtensor/xarray.hpp>

#define ERROR_REC_EPSILON   1e-3

auto gtestlog = spdlog::basic_logger_mt("gtestlog", "logs/gtestlog.txt");


class TransformContextTest: public ::testing::Test
{
public:
    TransformContext<float> *ctx;
    float *signal;
    float *transformed_signal;
    float *recovered_signal;
    Point4D lf_size;
    Point4D lf_stride;

    ~TransformContextTest()
    {
        delete ctx;
        delete[] signal;
        delete[] transformed_signal;
        delete[] recovered_signal;
    }

    void init_arrays(size_t size)
    {
        signal = new float[size];
        transformed_signal = new float[size];
        recovered_signal = new float[size];

        // Seed the random number generator
        std::srand(0);

        // Populate and initialize arrays for a clean environment.
        for (int i = 0; i < size; i++) {
            float value = 256 * (std::rand() / (RAND_MAX * 1.0));
            signal[i] = value;
            transformed_signal[i] = 0;
            recovered_signal[i] = 0;
        }
    }

    void init_arrays_4d(const std::vector<size_t> sizes) {
        lf_size.x = sizes[0];
        lf_size.y = sizes[1];
        lf_size.u = sizes[2];
        lf_size.v = sizes[3];
        lf_stride.x = 1;
        lf_stride.y = sizes[0];
        lf_stride.u = lf_stride.y * sizes[1];
        lf_stride.v = lf_stride.u * sizes[2];

        init_arrays(lf_stride.v * sizes[3]);     
    }

};


TEST_F(TransformContextTest, dct_recovered_signal_within_error_margin)
{
    const size_t FULL_LENGTH = 1024;
    ctx = new DiscreteCosineTransformContext<float>(FULL_LENGTH);

    init_arrays(FULL_LENGTH);

    // Forward DCT
    ctx->forward(signal, transformed_signal);
    ctx->inverse(transformed_signal, recovered_signal);

    EXPECT_NEAR(distance_percent<float>(signal, recovered_signal, FULL_LENGTH), 
        0, ERROR_REC_EPSILON) << "Distance between original and recovered is "
                                 "bigger than 0.0001%.";
    
    DiscreteCosineTransformContext<float>::flush_coeff();
}

TEST_F(TransformContextTest, dct_4d_recovered_signal_within_error_margin)
{
    init_arrays_4d({15,18,14,8});
    ctx = new DiscreteCosineTransformContext4D<float>(lf_size, lf_stride);

    // Forward DCT
    ctx->forward(signal, transformed_signal);
    ctx->inverse(transformed_signal, recovered_signal);

    EXPECT_NEAR(distance_percent<float>(signal, recovered_signal, (2,2,2,2)), 
        0, ERROR_REC_EPSILON) << "Distance between original and recovered is "
                                 "bigger than 0.0001%.";
    

    DiscreteCosineTransformContext<float>::flush_coeff();
}

TEST_F(TransformContextTest, dst_recovered_signal_within_error_margin)
{
    const size_t FULL_LENGTH = 1024;
    ctx = new DiscreteSineTransformContext<float>(FULL_LENGTH);

    init_arrays(FULL_LENGTH);

    // Forward DCT
    ctx->forward(signal, transformed_signal);
    ctx->inverse(transformed_signal, recovered_signal);

    EXPECT_NEAR(distance_percent<float>(signal, recovered_signal, FULL_LENGTH), 
        0, ERROR_REC_EPSILON) << "Distance between original and recovered is "
                                 "bigger than 1%.";
    
    DiscreteSineTransformContext<float>::flush_coeff();
}

TEST_F(TransformContextTest, ensure_dct_and_dst_generate_different_coefficients)
{
    const size_t FULL_LENGTH = 2014;
    init_arrays(FULL_LENGTH);

    ctx = new DiscreteSineTransformContext<float>(FULL_LENGTH);
    TransformContext<float> * ctx_evil_twin = 
        new DiscreteCosineTransformContext<float>(FULL_LENGTH);

    // Use the already allocated arrays 
    auto *transformed_signal_evil = recovered_signal;

    ctx->forward(signal, transformed_signal);
    ctx_evil_twin->forward(signal, transformed_signal_evil);

    delete ctx_evil_twin;


    ASSERT_GE(distance_percent<float>(transformed_signal,
                                      transformed_signal_evil,
                                      FULL_LENGTH),
             0.01) << "The proportional distance between the DST and DCT "
                      " coefficients should be at least 1";
}   
