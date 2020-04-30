#if !defined(__BLOCK_H__)
#define __BLOCK_H__

#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

auto logger = spdlog::basic_logger_mt("block_logger", "logs/block.txt");

template <typename T>
std::string join(const std::string &delim, const T *begin, const T *end) {
  std::stringstream s;
  for (auto it = begin; it != end; it++) {
    if (it != begin) {
      s << delim;
    }
    s << *it;
  }
  return s.str();
}

template <typename T> std::string join(const std::string &delim, const T &v) {
  std::stringstream s;
  for (auto it = std::begin(v); it != std::end(v); it++) {
    if (it != std::begin(v)) {
      s << delim;
    }
    s << *it;
  }
  return s.str();
}

template <typename T> class Block {
public:
  struct Range {
    int begin;
    int end;

    Range(int begin = -1, int end = -1) : begin(begin), end(end) {}
    friend std::ostream &operator<<(std::ostream &os, const Range &r) {
      os << r.str();
      return os;
    }
    std::string str() const {
      std::stringstream s;

      s << "Range(begin=" << (begin == -1 ?: begin)
        << (begin == -1 ? "None" : "") << ", end=" << (end == -1 ?: end)
        << (end == -1 ? "None" : "") << ")";
      return s.str();
    }
  };

private:
  T *array;
  size_t size_dim;
  size_t *size;
  size_t *stride = nullptr;
  Range *ranges = nullptr;

  inline size_t _index(const size_t *index) {
    // Dimension checks
    for (int i = 0; i < size_dim; i++)
      if (index[size_dim - 1 - i] >= size[i]) {
        logger->error("_index: invalid position. index={{{}}}, shape={{{}}}",
                      join<size_t>(", ", index, index + size_dim),
                      join<size_t>(", ", size, size + size_dim));
        throw std::out_of_range("Tried to access invalid position.");
      }
    size_t idx = 0;
    for (int i = 0; i < size_dim; i++) {
      auto rev_i = size_dim - 1 - i;
      idx += (index[rev_i] + ranges[rev_i].begin) * stride[i];
    }
    return idx;
  }

  inline void recalculate_stride() {
    if (stride)
      delete[] stride;
    stride = new size_t[size_dim + 1];
    stride[0] = 1;
    for (int d = 0; d < size_dim; d++)
      stride[d + 1] = size[d] * stride[d];
  }

  inline void recalculate_ranges() {
    if (ranges)
      delete[] ranges;
    ranges = new Range[size_dim];
    for (int i = 0; i < size_dim; i++) {
      ranges[i].begin = 0;
      ranges[i].end = size[i];
    }
  }

  Block(const Block &b, const std::vector<Range> &ranges) {
    array = b.array;
    size_dim = b.size_dim;
    size = new size_t[size_dim];
    for (int i = 0; i < size_dim; i++)
      size[i] = b.size[i];
    recalculate_stride();
    this->ranges = new Range[size_dim];
    for (int i = 0; i < size_dim; i++) {
      this->ranges[i].begin = ranges[i].begin + b.ranges[i].begin;
      this->ranges[i].end = ranges[i].end + b.ranges[i].begin;
    }
    logger->debug("Block: got ranges: {}. Updated ranges: {}",
                  join<std::vector<Range>>(", ", ranges),
                  join<Range>(", ", this->ranges, this->ranges + size_dim));
  }

public:
  Block(T *array_, const size_t size) : Block(array_, &size, 1) {}

  Block(T *array_, const size_t *size_, const size_t size_dim_)
      : array(array_) {
    logger->set_level(spdlog::level::debug);
    if (!array) {
      logger->error("Block: Tried to wrap an invalid memory address: {X}",
                    (void *)array);
      throw std::invalid_argument("Invalid pointer.");
    }
    size_dim = size_dim_;
    size = new size_t[size_dim];

    for (int i = 0; i < size_dim; i++)
      size[i] = size_[i];

    stride = nullptr;
    recalculate_stride();
    recalculate_ranges();
  }

  T &operator()(const size_t index0, ...) {
    // Array to index underlying "tensor"
    size_t index[size_dim];
    index[0] = index0;

    // Read variadic argument list into index[]
    va_list ap;
    va_start(ap, index0);
    for (int i = 1; i < size_dim; i++)
      index[i] = va_arg(ap, size_t);
    va_end(ap);

    return array[_index(index)];
  }

  void reshape(std::initializer_list<size_t> dim) {
    size_t total_elements = 1;

    for (auto it = dim.begin(); it != dim.end(); it++)
      total_elements *= *it;

    if (total_elements != stride[size_dim]) {
      logger->error("reshape: invalid shape. Current {{{}}}, got {{{}}}",
                    join<size_t>(", ", size, size + size_dim),
                    join<size_t>(", ", dim.begin(), dim.end()));
      throw std::length_error("Invalid shape");
    }

    size_t *new_size = new size_t[dim.size()];
    size_dim = dim.size();
    size_t *p = new_size;
    for (auto it = dim.begin(); it != dim.end(); it++)
      *p++ = *it;
    delete[] size;
    size = new_size;
    recalculate_stride();
    recalculate_ranges();
  }

  auto view(std::initializer_list<Range> ranges) {
    const char *message;
    if (ranges.size() != size_dim) {
      logger->error("view: nedded {} ranges. Got {}", size_dim, ranges.size());
      throw std::invalid_argument(
          "Number of Range instances does not match dimension");
    }
    int i = 0;
    std::vector<Range> adjusted_ranges;
    for (auto &it : ranges) {
      auto begin = 0;
      auto end = this->ranges[i].end - this->ranges[i].begin;
      if ((it.begin != -1 && it.begin < begin) ||
          (it.end != -1 && it.end > end)) {
        logger->error("view: invalid range. Current {}, got {}",
                      this->ranges[i].str(), it.str());
        message = "Invalid Range";
        throw std::out_of_range(message);
      }
      if (it.begin != -1 && it.end != -1 && it.end < it.begin) {
        logger->error(
            "view: decreasing indexing not allowed. Current {}, got {}",
            this->ranges[i].str(), it.str());
        throw std::invalid_argument("Decreasing indexing not allowed");
      }
      i++;
      // auto r = Range();
      // r.begin = it.begin == -1 ? -1 : it.begin + this->ranges[i].begin - 1;
      // r.end = it.end == -1 ? -1 : it.end + this->ranges[i].begin - 1;
      // adjusted_ranges.push_back(r);
    }
    return Block(*this, ranges);
  }
};

#endif // __BLOCK_H__
