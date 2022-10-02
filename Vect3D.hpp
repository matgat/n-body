#ifndef GUARD_vect3d_hpp
#define GUARD_vect3d_hpp
//  ---------------------------------------------
//  A 3D physical vector
//  ---------------------------------------------
#include <ostream> // std::ostream
//#include <fmt/format.h> // fmt::formatter
#include "math-utilities.hpp" // math::*


////////////////////////////////////////////////////////////////////////
struct Vect3D final
{
    double x, y, z;

    Vect3D() noexcept : x(0.0), y(0.0), z(0.0) {}
    Vect3D(const double X, const double Y, const double Z) noexcept : x(X), y(Y), z(Z) {}

    Vect3D operator+=(const Vect3D& other) noexcept
       {
        x+=other.x; y+=other.y; z+=other.z;
        return *this;
       }

    [[nodiscard]] Vect3D operator-() const noexcept
       {
        return Vect3D{-x, -y, -z};
       }

    [[nodiscard]] friend Vect3D operator+(const Vect3D& v1, const Vect3D& v2) noexcept
       {
        return Vect3D{v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
       }

    [[nodiscard]] friend Vect3D operator-(const Vect3D& v1, const Vect3D& v2) noexcept
       {
        return Vect3D{v1.x-v2.x, v1.y-v2.y, v1.z-v2.z};
       }

    [[nodiscard]] friend Vect3D operator*(double k, const Vect3D& v) noexcept
       {
        return Vect3D{k*v.x, k*v.y, k*v.z};
       }

    friend std::ostream& operator<<(std::ostream& os, const Vect3D& v)
       {
        os << v.x << ',' << v.y << ',' << v.z;
        return os;
       }

    [[nodiscard]] double norm2() const noexcept { return x*x + y*y + z*z; }
    [[nodiscard]] double norm() const noexcept { return std::sqrt(norm2()); }
};


////////////////////////////////////////////////////////////////////////
//template<> struct fmt::formatter<Vect3D> final
//{
//    template<typename ParseContext>
//    constexpr auto parse(ParseContext& ctx) noexcept { return ctx.begin(); }
//
//    template<typename FormatContext>
//    auto format(const Vect3D& v, FormatContext& ctx) const
//       {
//        return fmt::format_to(ctx.out(), "{},{},{}", v.x, v.y, v.z);
//       }
//};


//---- end unit --------------------------------------------------------------
#endif
