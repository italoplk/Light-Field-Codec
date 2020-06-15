#include "Point4D.h"
#include "Transform.h"
#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>

#include <cmath>

template <typename T, typename size_type> struct segment_type {
  using value_type = T;
  std::vector<size_type> shape;
  T *block;
};

template <typename T> inline Point4D make_stride(T shape) {
  Point4D stride;
  stride.x = 1;
  stride.y = shape[0];
  stride.u = stride.y * shape[1];
  stride.v = stride.u * shape[2];
  return stride;
}

template <typename size_type>
auto make_shapes(const std::vector<size_type> &from_shape,
                 const std::vector<size_type> &base_shape) {
  std::vector<std::vector<size_type>> shapes;
  std::vector<size_type> shape;
  shape.resize(4);
  for (int v = 0; v < from_shape[3]; v += shape[3]) {
    shape[3] = std::min(base_shape[3], from_shape[3] - v);
    for (int u = 0; u < from_shape[2]; u += shape[2]) {
      shape[2] = std::min(base_shape[3], from_shape[2] - u);
      for (int y = 0; y < from_shape[1]; y += shape[1]) {
        shape[1] = std::min(base_shape[1], from_shape[1] - y);
        for (int x = 0; x < from_shape[0]; x += shape[0]) {
          shape[0] = std::min(base_shape[0], from_shape[0] - x);
          shapes.push_back(shape);
        }
      }
    }
  }
  return shapes;
}

template <typename T, typename size_type>
void segment_block(const T *from_block,
                   const std::vector<size_type> &from_shape, T *into_block,
                   const std::vector<size_type> &base_shape) {
  auto *curr_seg = into_block;
  auto stride = make_stride(from_shape);
  int seg_index = 0;
  const auto shapes = make_shapes(from_shape, base_shape);
  auto shape = shapes[seg_index];
  for (int v = 0; v < from_shape[3]; v += shape[3]) {
    for (int u = 0; u < from_shape[2]; u += shape[2]) {
      for (int y = 0; y < from_shape[1]; y += shape[1]) {
        for (int x = 0; x < from_shape[0]; x += shape[0]) {
          shape = shapes[seg_index++];
          auto into_stride = make_stride(shape);
          for (int dv = 0; dv < shape[3]; dv++)
            for (int du = 0; du < shape[2]; du++)
              for (int dy = 0; dy < shape[1]; dy++) {
                auto from_offset = stride.x * x + stride.y * (y + dy) +
                                   stride.u * (u + du) + stride.v * (v + dv);

                std::copy(from_block + from_offset,
                          from_block + from_offset + shape[0], curr_seg);
                curr_seg += shape[0];
              }
        }
      }
    }
  }
}

template <typename T, typename size_type>
void join_segments(const T *from_block,
                   const std::vector<size_type> &from_shape, T *into_block,
                   const std::vector<size_type> &base_shape) {
  auto *curr_seg = from_block;
  std::vector<segment_type<T, size_type>> segments;
  auto stride = make_stride(from_shape);
  int seg_index = 0;
  const auto shapes = make_shapes(from_shape, base_shape);
  auto shape = shapes[seg_index];
  for (int v = 0; v < from_shape[3]; v += shape[3]) {
    for (int u = 0; u < from_shape[2]; u += shape[2]) {
      for (int y = 0; y < from_shape[1]; y += shape[1]) {
        for (int x = 0; x < from_shape[0]; x += shape[0]) {
          shape = shapes[seg_index++];
          auto into_stride = make_stride(shape);
          for (int dv = 0; dv < shape[3]; dv++)
            for (int du = 0; du < shape[2]; du++)
              for (int dy = 0; dy < shape[1]; dy++) {
                auto into_offset = stride.x * x + stride.y * (y + dy) +
                                   stride.u * (u + du) + stride.v * (v + dv);
                std::copy(curr_seg, curr_seg + shape[0],
                          into_block + into_offset);
                curr_seg += shape[0];
              }
        }
      }
    }
  }
}

TEST(BlockSegmentation, segmentation_into_16_equal_parts) {
  std::vector from_shape{4, 4, 4, 4};
  std::vector first_shape{2, 2, 2, 2};
  const int SIZE = 256;
  float block[SIZE] = {0};
  float seg_block[SIZE] = {0};

  int count_block[16] = {0};

  float *pblock = block;
  for (int v = 0; v < from_shape[3]; v++)
    for (int u = 0; u < from_shape[2]; u++)
      for (int y = 0; y < from_shape[1]; y++)
        for (int x = 0; x < from_shape[0]; x++) {
          int num =
              (x >= 2) + ((y >= 2) << 1) + ((u >= 2) << 2) + ((v >= 2) << 3);
          *pblock++ = num;
          count_block[num]++;
        }

  segment_block(block, from_shape, seg_block, first_shape);
  float prev = seg_block[0];
  int counter = 1;
  int block_number = 0;
  for (int i = 1; i < SIZE; i++) {
    if (seg_block[i] == prev) {
      counter++;
    } else {
      ASSERT_EQ(count_block[block_number], counter) << block_number << " " << i;
      counter = 1;
      prev = seg_block[i];
      block_number++;
    }
  }
}

TEST(BlockSegmentation, segmentation_into_16_irregular_parts_2) {
  std::vector from_shape{5, 4, 4, 4};
  std::vector first_shape{3, 2, 2, 2};
  const int SIZE = 5 * 5 * 6 * 6;
  float block[SIZE] = {0};
  float seg_block[SIZE] = {0};
  int count_block[16] = {0};

  float *pblock = block;
  for (int v = 0; v < from_shape[3]; v++)
    for (int u = 0; u < from_shape[2]; u++)
      for (int y = 0; y < from_shape[1]; y++)
        for (int x = 0; x < from_shape[0]; x++) {
          int num =
              (x >= 3) + ((y >= 2) << 1) + ((u >= 2) << 2) + ((v >= 2) << 3);
          *pblock++ = num;
          count_block[num]++;
        }

  segment_block(block, from_shape, seg_block, first_shape);
  float prev = seg_block[0];
  int counter = 1;
  int block_number = 0;
  for (int i = 1; i < SIZE; i++) {
    if (seg_block[i] == prev) {
      counter++;
    } else {
      ASSERT_EQ(count_block[block_number], counter) << block_number << " " << i;
      counter = 1;
      prev = seg_block[i];
      block_number++;
    }
  }
}

TEST(BlockSegmentation, segment_join_2) {
  std::vector from_shape{4, 4, 4, 4};
  std::vector first_shape{2, 2, 2, 2};
  const int SIZE = 256;
  float block[SIZE] = {0};
  float seg_block[SIZE] = {0};
  float rec_block[SIZE] = {0};

  float *pblock = block;
  for (int v = 0; v < from_shape[3]; v++)
    for (int u = 0; u < from_shape[2]; u++)
      for (int y = 0; y < from_shape[1]; y++)
        for (int x = 0; x < from_shape[0]; x++) {
          int num =
              (x >= 2) + ((y >= 2) << 1) + ((u >= 2) << 2) + ((v >= 2) << 3);
          *pblock++ = num;
        }

  segment_block(block, from_shape, seg_block, first_shape);
  join_segments(seg_block, from_shape, rec_block, first_shape);

  for (int i = 0; i < SIZE; i++)
    ASSERT_EQ(block[i], rec_block[i]);
  // delete_segments(&segments, &seg_shapes, seg_count);
}

TEST(BlockSegmentation, segment_81_parts) {
  std::vector from_shape{6, 6, 6, 6};
  std::vector first_shape{2, 2, 2, 2};
  const int SIZE = 6 * 6 * 6 * 6;
  float block[SIZE] = {0};
  float seg_block[SIZE] = {0};
  float rec_block[SIZE] = {0};

  float *pblock = block;
  for (int v = 0; v < from_shape[3]; v++)
    for (int u = 0; u < from_shape[2]; u++)
      for (int y = 0; y < from_shape[1]; y++)
        for (int x = 0; x < from_shape[0]; x++) {
          int num = x + (y / 2) * 3 + (u / 2) * 9 + (v / 2) * 27;
          *pblock++ = num;
        }

  segment_block(block, from_shape, seg_block, first_shape);
  join_segments(seg_block, from_shape, rec_block, first_shape);

  for (int i = 0; i < SIZE; i++)
    ASSERT_EQ(block[i], rec_block[i]);
  // delete_segments(&segments, &seg_shapes, seg_count);
}

TEST(BlockSegmentation, join_irregular) {
  std::vector from_shape{5, 5, 6, 6};
  std::vector first_shape{3, 3, 3, 3};
  const int SIZE = 5 * 5 * 6 * 6;
  float block[SIZE] = {0};
  float seg_block[SIZE] = {0};
  float rec_block[SIZE] = {0};

  for (int i = 0; i < SIZE; i++)
    block[i] = i;

  segment_block(block, from_shape, seg_block, first_shape);
  join_segments(seg_block, from_shape, rec_block, first_shape);

  for (int i = 0; i < SIZE; i++)
    ASSERT_EQ(block[i], rec_block[i]);
  // delete_segments(&segments, &seg_shapes, seg_count);
}
