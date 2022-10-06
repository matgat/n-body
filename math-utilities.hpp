#ifndef GUARD_math_utilities_hpp
#define GUARD_math_utilities_hpp
//  ---------------------------------------------
//  Common math facilities
//  ---------------------------------------------
#include <cmath>
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

}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
