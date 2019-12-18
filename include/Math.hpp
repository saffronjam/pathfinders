#pragma once
#include <cmath>

namespace Math
{
namespace Constants
{
constexpr double PI_D = 3.141592653589793238462643383279;
constexpr float PI = float(PI_D);
} // namespace Constants
} // namespace Math

template <typename T>
auto sq(T x)
{
	return x * x;
}