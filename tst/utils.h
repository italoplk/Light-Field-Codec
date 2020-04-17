#if !defined(__TEST_UTILS_H__)
#define __TEST_UTILS_H__

#include <sstream>
#include <cmath>


/* Euclidean distance between two arrays */
template <typename T>
double distance(const T *a, const T *b, size_t length) 
{
    double _distance = 0;
    for (size_t i = 0; i < length; i++) 
        _distance += pow(a[i] - b[i], 2);
    return sqrt(_distance);
}


#endif // __TEST_UTILS_H__
