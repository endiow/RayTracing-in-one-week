#pragma once
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <functional>
#include <random>


// Usings

using std::shared_ptr;
using std::make_shared;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) 
{
    return degrees * pi / 180;
}

inline double ffmin(double a, double b) { return a <= b ? a : b; }
inline double ffmax(double a, double b) { return a >= b ? a : b; }

//�����
inline double random_double(double min, double max)
{
    static std::uniform_real_distribution<double> distribution(min, max);   //����[min,max]��Χ�ڵĸ�����
    static std::mt19937 generator;  //ʹ��Mersenne Twister���棬���ɸ�������α�������
    static std::function<double()> rand_generator = std::bind(distribution, generator);
    return rand_generator();
}

//����x
inline double clamp(double x, double min, double max) 
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

#include "ray.h"
#include "vec3.h"