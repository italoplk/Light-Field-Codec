#if !defined(__BLOCK_H__)
#define __BLOCK_H__

#include <cstdarg>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

auto logger = spdlog::basic_logger_mt("block_logger", "logs/block.txt");

/**
 * @brief Join container using `delim`
 *
 * @tparam T Type of container
 * @param delim string to be used as delimiter
 * @param begin start of container
 * @param end end of container
 * @return std::string elements joined
 */
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

/**
 * @brief Join container using `delim`
 *
 * @tparam T Type of container
 * @param delim string to be used as delimiter
 * @param v container
 * @return std::string elements joined
 */
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

/**
 * @brief Wrapper for contiguous memory spaces
 *
 * `Block` provides an interface to access, reshape and iterate over a
 * contiguous memory region.
 *
 * @tparam T Type of elements.
 */
template <typename T> class Block {
public:
  /**
   * @brief Slicer across dimensions
   */
  struct Range {
    static const int OPEN_INTERVAL;
    static const int NOT_DEFINED;
    int begin;
    int end;
    int step;
    int scalar;

    Range(const int scalar = NOT_DEFINED)
        : begin(NOT_DEFINED), end(NOT_DEFINED), step(NOT_DEFINED),
          scalar(scalar) {}
    explicit Range(int begin, int end = NOT_DEFINED, int step = NOT_DEFINED)
        : begin(begin), end(end), step(step), scalar(NOT_DEFINED) {}

    /**
     * @brief Friendly representation of a Range.
     *
     * @param os outputstream object.
     * @param r range
     * @return std::ostream&
     */
    friend std::ostream &operator<<(std::ostream &os, const Range &r) {
      os << r.str();
      return os;
    }

    /**
     * @brief Nice string representation of a Range.
     *
     * @return std::string Range representation.
     */
    std::string str() const {
      std::stringstream s;
      s << "Range(";
      if (scalar != NOT_DEFINED) {
        s << "scalar=" << scalar;
      } else {
        s << "begin=";
        if (begin != NOT_DEFINED)
          s << begin << ", end=";
        else
          s << "None, end=";
        if (end != NOT_DEFINED)
          s << end << ", step=";
        else
          s << "None, end=";
        if (step != NOT_DEFINED)
          s << step << "";
        else
          s << "None";
      }
      s << ")";
      return s.str();
    }
  };

  /**
   * @brief Iterator over all elements of a block.
   *
   */
  class Iterator {
    Block b;
    int index;
    bool reverse_order;

  public:
    Iterator(Block &b, int index, bool reverse_order) {
      this->b = b.flat_view();
      this->reverse_order = reverse_order;
      this->index = index;
    }
    Iterator(const Iterator& it) {
      b = it.b;
      index = it.index;
      reverse_order = it.reverse_order;
    }
    Iterator(const Iterator&& it) {
      b = it.b;
      index = it.index;
      reverse_order = it.reverse_order;
    }
    T &operator*() { return b(index); }
    Iterator operator++() {
      index += reverse_order ? -1 : 1;
      auto it = Iterator(*this);
      return it;
    }
    Iterator operator++(int) {
      auto it = Iterator(*this);
      index += reverse_order ? -1 : 1;
      return it;
    }
    bool operator==(const Iterator &other) { return index == other.index; }
    bool operator!=(const Iterator &other) { return !(*this == other); }
  };

private:
  T *array;
  size_t rank;
  std::vector<size_t> size;
  std::vector<size_t> stride;
  std::vector<Range> ranges;

  template <typename... Ts> inline size_t _find_index(const Ts &... _index) {
    // Dimension checks
    std::vector index({_index...});
    for (int i = 0; i < rank; i++)
      if (index[rank - 1 - i] >= size[i]) {
        logger->error(
            "_find_index: invalid position. index={{{}}}, shape={{{}}}",
            join(", ", index), join(", ", size));
        throw std::out_of_range("Tried to access invalid position.");
      }

    auto s = stride.begin();
    auto r = ranges.rbegin();
    size_t idx = 0;
    for (auto i = index.rbegin(); i != index.rend(); i++, s++, r++)
      idx += (*i + (*r).begin) * *s;
    return idx;
  }

  inline void recalculate_stride() {
    stride.resize(rank + 1);
    auto s = stride.begin();
    *s = 1;
    for (const auto &n : size)
      *++s = *s * n;
  }

  inline void recalculate_ranges() {
    ranges.resize(rank);
    auto s = size.begin();
    for (auto &r : ranges)
      r.begin = 0, r.end = *s++;
  }

  Block(const Block &b, const std::vector<Range> &ranges) {
    array = b.array;
    rank = b.rank;
    size = b.size;
    recalculate_stride();
    this->ranges.resize(rank);

    for (int i = 0; i < rank; i++) {
      this->ranges[i].begin = ranges[i].begin + b.ranges[i].begin;
      this->ranges[i].end = ranges[i].end + b.ranges[i].begin;
    }
    logger->debug("Block: got ranges: {}. Updated ranges: {}",
                  join(", ", ranges), join(", ", this->ranges));
  }

public:
  Block() = default;
  /**
   * @brief Construct a new Block object
   *
   * @param array Pointer to contigous memory region
   * @param size Number of elements
   */
  Block(T *array, const size_t size) : Block(array, &size, 1) {}
  /**
   * @brief Construct a new Block object
   *
   * @param array Pointer to contigous memory region
   * @param size Number of elements per dimension.
   * @param rank number of dimensions
   */
  Block(T *array, const size_t *size, const size_t rank) {
    logger->set_level(spdlog::level::debug);
    if (!array) {
      logger->error("Block: Tried to wrap an invalid memory address: {X}",
                    (void *)array);
      throw std::invalid_argument("Invalid pointer.");
    }
    this->array = array;
    this->rank = rank;
    this->size.resize(rank);
    this->size.assign(size, size + rank);

    recalculate_stride();
    recalculate_ranges();
  }
  /**
   * @brief Copy constructor
   *
   * @param obj object to be copied.
   */
  Block(const Block &obj) {
    rank = obj.rank;
    array = obj.array;
    size = obj.size;

    recalculate_stride();
    recalculate_ranges();
  }
  /** Iterator to beginner of block. */
  auto begin() { return Iterator(*this, 0, false); }
  /** Reverse Iterator to end of block. */
  auto rbegin() { return Iterator(*this, flat_size() - 1, true); }
  /** Iterator to end of block. */
  auto end() { return Iterator(*this, flat_size(), false); }
  /** Reverse Iterator to beginner of block. */
  auto rend() { return Iterator(*this, -1, true); }

  /**
   * @brief Index elements within inner array.
   *
   * @param index0 first index
   * @param ... other dimensions.
   * @return T& Object at indexed position.
   */
  template <typename... Ts> T &operator()(const Ts &... index) {
    // Array to index underlying "tensor"
    auto i = _find_index(index...);
    return array[i];
  }

  /**
   * @brief Allows reshapping the indexer of inner array.
   *
   * @param dim new shape
   */
  void reshape(std::initializer_list<size_t> dim) {
    size_t total_elements = 1;

    for (auto it = dim.begin(); it != dim.end(); it++)
      total_elements *= *it;

    if (total_elements != stride[rank]) {
      logger->error("reshape: invalid shape. Current {{{}}}, got {{{}}}",
                    join(", ", size), join(", ", dim.begin(), dim.end()));
      throw std::length_error("Invalid shape");
    }

    rank = dim.size();
    size.assign(dim.begin(), dim.end());
    recalculate_stride();
    recalculate_ranges();
  }

  template <typename ...Ts>
  void reshape(const Ts& ...dim)
  {
    reshape({static_cast<size_t>(dim)...}); 
  }

  /**
   * @brief Create a sliced view
   *
   * @param ranges Range for each dimension
   * @return view object.
   */
  auto view(std::initializer_list<Range> ranges) {
    if (ranges.size() != rank) {
      logger->error("view: nedded {} ranges. Got {}", rank, ranges.size());
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
        throw std::out_of_range("Invalid Range");
      }
      if (it.begin != -1 && it.end != -1 && it.end < it.begin) {
        logger->error(
            "view: decreasing indexing not allowed. Current {}, got {}",
            this->ranges[i].str(), it.str());
        throw std::invalid_argument("Decreasing indexing not allowed");
      }
      i++;
    }
    return Block(*this, ranges);
  }

  /**
   * @brief Flat view of inner array.
   *
   * @return view object.
   */
  auto flat_view() {
    auto clone = Block(*this);
    clone.reshape({this->stride[rank]});
    return clone;
  }

  const auto &shape() { return size; }

  const size_t flat_size() { return stride[rank]; }
};

template <typename T>
const int Block<T>::Range::OPEN_INTERVAL = std::numeric_limits<int>::max();

template <typename T>
const int Block<T>::Range::NOT_DEFINED = std::numeric_limits<int>::min();

#endif // __BLOCK_H__
