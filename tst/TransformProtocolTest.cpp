#include "TransformProtocol.h"
#include "Point4D.h"
#include <gtest/gtest.h>



class TransformProtocolTest : public ::testing::Test 
{
public:
  float *signal;
  float *transformed_signal;
  float *recovered_signal;
  Point4D block_size;
  Point4D block_stride;

  ~TransformProtocolTest() {
    delete[] signal;
    delete[] transformed_signal;
    delete[] recovered_signal;
    DiscreteCosineTransformContext<float>::flush_coeff();
    DiscreteSineTransformContext<float>::flush_coeff();
  }
  void init_arrays(size_t size) {
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
    block_size.x = sizes[0];
    block_size.y = sizes[1];
    block_size.u = sizes[2];
    block_size.v = sizes[3];
    block_stride.x = 1;
    block_stride.y = sizes[0];
    block_stride.u = block_stride.y * sizes[1];
    block_stride.v = block_stride.u * sizes[2];

    init_arrays(block_stride.v * sizes[3]);
  }
};



TEST_F(TransformProtocolTest, DCT_is_implemented) {

  init_arrays_4d({8,8,8,8});

  TransformProtocol<float> p("DCT", block_size);
  DiscreteCosineTransformContext4D<float> dct(block_size, block_stride);
  
  // Recovered signal was previously allocated and initalized.
  // As we need two blocks, `recovered_signal` is renamed to reflect its use here.
  auto expected_transformed_signal = recovered_signal; 

  p.forward(signal, transformed_signal);
  dct.forward(signal, expected_transformed_signal);

  for (int i = 0; i < 4096; i++)
    ASSERT_EQ(transformed_signal[i], expected_transformed_signal[i]);
}


TEST_F(TransformProtocolTest, DST_is_implemented) {
  init_arrays_4d({8,8,8,8});

  TransformProtocol<float> p("DST", block_size);
  DiscreteSineTransformContext4D<float> dst(block_size, block_stride);
  
  // Recovered signal was previously allocated and initalized.
  // As we need two blocks, `recovered_signal` is renamed to reflect its use here.
  auto expected_transformed_signal = recovered_signal; 

  p.forward(signal, transformed_signal);
  dst.forward(signal, expected_transformed_signal);

  for (int i = 0; i < 4096; i++)
    ASSERT_EQ(transformed_signal[i], expected_transformed_signal[i]);
}