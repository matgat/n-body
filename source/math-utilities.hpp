#pragma once
//  ---------------------------------------------
//  Common math facilities
//  ---------------------------------------------
#include <cmath> // std::abs
#include <limits> // std::numeric_limits


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace math //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

//----------------------------------------------------------------------
/*constexpr*/ inline bool is_zero(const double x) noexcept
{
    return std::abs(x) < std::numeric_limits<double>::epsilon();
}

//----------------------------------------------------------------------
/*constexpr*/ inline double ratio(const double n, double d) noexcept
{
    if( is_zero(d) ) d = std::numeric_limits<double>::epsilon();
    return n/d;
}

//----------------------------------------------------------------------
constexpr inline double square(const double x) noexcept { return x*x; }
constexpr inline double cube(const double x) noexcept { return x*x*x; }

}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
