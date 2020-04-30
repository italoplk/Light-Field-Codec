#if !defined(__BLOCK_H__)
#define __BLOCK_H__

#include <cstdarg>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <initializer_list>

template <typename T>
class Block
{
public:
    struct Range
    {
        int begin;
        int end;

        Range(int begin = -1, int end = -1)
            : begin(begin), end(end)
        {
        }
    };

private:
    T *array;
    size_t size_dim;
    size_t *size;
    size_t *stride = nullptr;
    Range *ranges = nullptr;

    inline size_t _index(const size_t *index)
    {
        // Dimension checks
        for (int i = 0; i < size_dim; i++)
            if (index[size_dim - 1 - i] >= size[i])
            {
                std::stringstream message("Tried to access position ");
                message << index;
                message << " beyound size ";
                message << size;
                message << " at dimension ";
                message << i;
                throw std::out_of_range(message.str());
            }
        size_t idx = 0;
        for (int i = 0; i < size_dim; i++)
        {
            auto rev_i = size_dim - 1 - i;
            idx += (index[rev_i] + ranges[rev_i].begin) * stride[i];
        }

        return idx;
    }

    inline void recalculate_stride()
    {
        if (!stride)
            delete[] stride;
        stride = new size_t[size_dim + 1];
        stride[0] = 1;
        for (int d = 0; d < size_dim; d++)
            stride[d + 1] = size[d] * stride[d];
    }

    inline void recalculate_ranges()
    {
        if (ranges)
            delete[] ranges;
        ranges = new Range[size_dim];
        for (int i = 0; i < size_dim; i++)
        {
            ranges[i].begin = 0;
            ranges[i].end = size[i];
        }
    }

    Block(const Block &b, const std::initializer_list<Range> &ranges)
    {
        array = b.array;
        size_dim = b.size_dim;
        size = new size_t[size_dim];
        for (int i = 0; i < size_dim; i++)
            size[i] = b.size[i];
        recalculate_stride();
        this->ranges = new Range[size_dim];
        auto *r = this->ranges;
        for (auto it = ranges.begin(); it != ranges.end(); it++)
            *r++ = *it;
    }

public:
    Block(T *array_, const size_t size)
        : Block(array_, &size, 1)
    {
    }

    Block(T *array_, const size_t *size_, const size_t size_dim_)
        : array(array_)
    {
        size_dim = size_dim_;
        size = new size_t[size_dim];

        for (int i = 0; i < size_dim; i++)
        {
            size[i] = size_[i];
        }
        stride = nullptr;
        recalculate_stride();
        recalculate_ranges();
    }

    T &operator()(const size_t index0, ...)
    {
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

    void reshape(std::initializer_list<size_t> dim)
    {
        size_t total_elements = 1;

        for (auto it = dim.begin(); it != dim.end(); it++)
            total_elements *= *it;

        if (total_elements != stride[size_dim])
        {
            std::stringstream message("Invalid shape ");
            std::copy(dim.begin(),
                      dim.end(),
                      std::ostream_iterator<int>(message, " "));
            throw std::length_error(message.str());
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

    auto view(std::initializer_list<Range> ranges)
    {
        bool ranges_are_good = true;
        const char *message;
        if (ranges.size() != size_dim)
        {
            ranges_are_good = false;
            message = "Number of Range instances does not match dimension";
            throw std::invalid_argument(message);
        }
        int i = 0;
        for (const auto &it : ranges)
        {
            if ((it.begin != -1 && it.begin < this->ranges[i].begin)
                || (it.end != -1 && it.end > this->ranges[i].end))
            {
                message = "Invalid Range";
                throw std::out_of_range(message);
            }
            if (it.begin != -1 && it.end != -1 && it.end < it.begin)
            {
                message = ".end can't be smaller than .begin";
                throw std::invalid_argument(message);
            }
            i++;
        }
        return Block(*this, ranges);
    }
};

#endif // __BLOCK_H__
