#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>

#include "DiscreteCosineTransformContext.h"
#include "DiscreteCosineTransformContext4D.h"
#include "TransformContext.h"

#include "deprecated/Transform.h"
#include "utils.h"

#define ERROR_EPSILON 1e-6

/**
 * This tests if the new implementation is identical to the old one.
 * The test passes if both the DCT transform and its inverse are identical.
 * BUGS: Somehow, the old implementation used double as an intermediate value.
 * It caused small changes that, if accumulated, grew beyond 1e-5 in arrays for
 * 100 elements.
 */
TEST(BackwardsCompatibilityWithOldAPI, new_dct_1d_is_consistent) {
#define FULL_LENGTH 512
  TransformContext<float> *ctx;

  float input_new[FULL_LENGTH];
  float input_old[FULL_LENGTH];
  float output_new[FULL_LENGTH];
  float output_old[FULL_LENGTH];
  Point4D size(FULL_LENGTH, 1, 1, 1);

  // Seed the random number generator
  std::srand(0);

  // Populate input with random numbers and initialize output arrays.
  for (int i = 0; i < FULL_LENGTH; i++) {
    float value = 256 * (std::rand() / (RAND_MAX * 1.0));
    input_new[i] = value;
    input_old[i] = value;
    output_new[i] = 0;
    output_old[i] = 0;
  }

  old::Transform t(size);
  ctx = new DiscreteCosineTransformContext<float>(FULL_LENGTH);

  // Forward DCT
  ctx->forward(input_new, output_new);
  t.dct_4d(input_old, output_old, size, size);

  EXPECT_NEAR(distance_percent<float>(output_new, output_old, FULL_LENGTH), 0,
              ERROR_EPSILON)
      << "The distance between the intermediate DCT "
         "is bigger then the expected value.";

  std::swap(input_new, output_new);
  std::swap(input_old, output_old);

  // Inverse DCT
  ctx->inverse(input_new, output_new);
  t.idct_4d(input_old, output_old, size, size);

  EXPECT_NEAR(distance_percent<float>(output_new, output_old, FULL_LENGTH), 0,
              ERROR_EPSILON)
      << "The distance between the resulting vectors is "
         "bigger then the expected value.";

  delete ctx;
  DiscreteCosineTransformContext<float>::flush_coeff();
#undef FULL_LENGTH
}

TEST(BackwardsCompatibilityWithOldAPI, new_dct_4d_is_consistent) {
#define SIZE_X 15
#define SIZE_Y 16
#define SIZE_U 17
#define SIZE_V 18
#define FULL_LENGTH (SIZE_X * SIZE_Y * SIZE_U * SIZE_V)

  TransformContext<float> *ctx;

  Point4D size(SIZE_X, SIZE_Y, SIZE_U, SIZE_V);
  Point4D stride(1, SIZE_X, SIZE_X * SIZE_Y, SIZE_X * SIZE_Y * SIZE_U);
  float input_new[FULL_LENGTH];
  float input_old[FULL_LENGTH];
  float output_new[FULL_LENGTH];
  float output_old[FULL_LENGTH];

  // Seed the random number generator
  std::srand(0);

  // Populate input with random numbers and initialize output arrays.
  for (int i = 0; i < FULL_LENGTH; i++) {
    float value = 256 * (std::rand() / (RAND_MAX * 1.0));
    input_new[i] = value;
    input_old[i] = value;
    output_new[i] = 0;
    output_old[i] = 0;
  }

  old::Transform t(size);
  ctx = new DiscreteCosineTransformContext4D<float>(size, stride);

  // Forward DCT
  ctx->forward(input_new, output_new);
  t.dct_4d(input_old, output_old, size, size);

  EXPECT_NEAR(distance_percent<float>(output_new, output_old, FULL_LENGTH), 0,
              ERROR_EPSILON)
      << "The distance between the intermediate DCT "
         "is bigger then the expected value.";

  std::swap(input_new, output_new);
  std::swap(input_old, output_old);

  // Inverse DCT
  ctx->inverse(input_new, output_new);
  t.idct_4d(input_old, output_old, size, size);

  EXPECT_NEAR(distance_percent<float>(output_new, output_old, FULL_LENGTH), 0,
              ERROR_EPSILON)
      << "The distance between the resulting vectors is "
         "bigger then the expected value.";

  delete ctx;
  DiscreteCosineTransformContext<float>::flush_coeff();
#undef SIZE_X
#undef SIZE_Y
#undef SIZE_U
#undef SIZE_V
}

TEST(BackwardsCompatibilityWithOldAPI,
     new_api_accepts_blocks_of_smaller_sizes) {
#define SIZE_X 15
#define SIZE_Y 16
#define SIZE_U 17
#define SIZE_V 18
#define FULL_LENGTH (SIZE_X * SIZE_Y * SIZE_U * SIZE_V)

#define DELTA_SIZE 5

  TransformContext<float> *ctx;

  Point4D size_lightfield(SIZE_X, SIZE_Y, SIZE_U, SIZE_V);
  Point4D size_block = size_lightfield - DELTA_SIZE;
  Point4D stride_lightfield(1, SIZE_X, SIZE_X * SIZE_Y,
                            SIZE_X * SIZE_Y * SIZE_U);
  float input_new[FULL_LENGTH];
  float input_old[FULL_LENGTH];
  float output_new[FULL_LENGTH];
  float output_old[FULL_LENGTH];

  // Seed the random number generator
  std::srand(0);

  // Populate input with random numbers and initialize output arrays.
  for (int i = 0; i < FULL_LENGTH; i++) {
    float value = 256 * (std::rand() / (RAND_MAX * 1.0));
    input_new[i] = value;
    input_old[i] = value;
    output_new[i] = 0;
    output_old[i] = 0;
  }

  auto *size_block_array = size_block.to_array();

  old::Transform t(size_lightfield);
  ctx = new DiscreteCosineTransformContext4D<float>(size_lightfield,
                                                    stride_lightfield);

  // Forward DCT
  ctx->forward(input_new, output_new, size_block_array);
  t.dct_4d(input_old, output_old, size_block, size_lightfield);

  EXPECT_NEAR(distance_percent<float>(output_new, output_old, FULL_LENGTH), 0,
              ERROR_EPSILON)
      << "The distance between the intermediate DCT "
         "is bigger then the expected value.";

  std::swap(input_new, output_new);
  std::swap(input_old, output_old);

  // Inverse DCT
  ctx->inverse(input_new, output_new, size_block_array);
  t.idct_4d(input_old, output_old, size_block, size_lightfield);

  EXPECT_NEAR(distance_percent<float>(output_new, output_old, FULL_LENGTH), 0,
              ERROR_EPSILON)
      << "The distance between the resulting vectors is "
         "bigger then the expected value.";

  delete[] size_block_array;
  delete ctx;
  DiscreteCosineTransformContext<float>::flush_coeff();
#undef SIZE_X
#undef SIZE_Y
#undef SIZE_U
#undef SIZE_V
#undef FULL_LENGTH
#undef DELTA_SIZE
}