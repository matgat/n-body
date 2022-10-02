#ifndef GUARD_vect2d_hpp
#define GUARD_vect2d_hpp
//  ---------------------------------------------
//  A 2D physical vector
//  ---------------------------------------------
#include <ostream> // std::ostream
//#include <fmt/format.h> // fmt::formatter
#include "math-utilities.hpp" // math::*


////////////////////////////////////////////////////////////////////////
struct Vect2D final
{
    double x, y;

    Vect2D() noexcept : x(0.0), y(0.0) {}
    Vect2D(const double X, const double Y) noexcept : x(X), y(Y) {}

    Vect2D operator+=(const Vect2D& other) noexcept
       {
        x+=other.x; y+=other.y;
        return *this;
       }

    [[nodiscard]] Vect2D operator-() const noexcept
       {
        return Vect2D{-x, -y};
       }

    [[nodiscard]] friend Vect2D operator+(const Vect2D& v1, const Vect2D& v2) noexcept
       {
        return Vect2D{v1.x+v2.x, v1.y+v2.y};
       }

    [[nodiscard]] friend Vect2D operator-(const Vect2D& v1, const Vect2D& v2) noexcept
       {
        return Vect2D{v1.x-v2.x, v1.y-v2.y};
       }

    [[nodiscard]] friend Vect2D operator*(double k, const Vect2D& v) noexcept
       {
        return Vect2D{k*v.x, k*v.y};
       }

    friend std::ostream& operator<<(std::ostream& os, const Vect2D& v)
       {
        os << v.x << ',' << v.y;
        return os;
       }

    [[nodiscard]] double norm2() const noexcept { return x*x + y*y; }
    [[nodiscard]] double norm() const noexcept { return std::sqrt(norm2()); }
};


////////////////////////////////////////////////////////////////////////
//template<> struct fmt::formatter<Vect2D> final
//{
//    template<typename ParseContext>
//    constexpr auto parse(ParseContext& ctx) noexcept { return ctx.begin(); }
//
//    template<typename FormatContext>
//    auto format(const Vect2D& v, FormatContext& ctx) const
//       {
//        return fmt::format_to(ctx.out(), "{},{},{}", v.x, v.y);
//       }
//};


//---- end unit --------------------------------------------------------------
#endif
