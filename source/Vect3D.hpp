#pragma once
//  ---------------------------------------------
//  A 3D physical vector
//  ---------------------------------------------
#include <cassert>
#include <ostream>
#include <format>

#include "math-utilities.hpp" // math::*


////////////////////////////////////////////////////////////////////////
struct Vect3D final
{
    double x, y, z;

    Vect3D() noexcept : x{0.0}, y{0.0}, z{0.0} {}
    Vect3D(const double X, const double Y, const double Z) noexcept : x(X), y(Y), z(Z) {}

    //-----------------------------------------------------------------------
    [[nodiscard]] bool is_null() const noexcept
       {
        return math::is_zero(norm2());
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] bool operator==(const Vect3D& other) const noexcept
       {
        return math::is_zero(x-other.x) and math::is_zero(y-other.y) and math::is_zero(z-other.z);
       }

    //-----------------------------------------------------------------------
    void operator+=(const Vect3D& other) noexcept
       {
        x+=other.x; y+=other.y; z+=other.z;
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] Vect3D operator-() const noexcept
       {
        return Vect3D{-x, -y, -z};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect3D operator+(const Vect3D& v1, const Vect3D& v2) noexcept
       {
        return Vect3D{v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect3D operator-(const Vect3D& v1, const Vect3D& v2) noexcept
       {
        return Vect3D{v1.x-v2.x, v1.y-v2.y, v1.z-v2.z};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect3D operator*(const double k, const Vect3D& v) noexcept
       {
        return Vect3D{k*v.x, k*v.y, k*v.z};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect2D operator/(const Vect3D& v, const double k) noexcept
       {
        assert(k!=0.0);
        return Vect3D{v.x/k, v.y/k, v.z/k};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] double norm2() const noexcept { return x*x + y*y + z*z; }
    [[nodiscard]] double norm() const noexcept { return std::sqrt(norm2()); }
    [[nodiscard]] Vect3D& normalize() noexcept
       {
        const double len{ norm() };
        assert( not math::is_zero(len) );
        x/=len;
        y/=len;
        z/=len;
        return *this;
       }
    [[nodiscard]] Vect3D get_versor() const noexcept
       {
        const double len{ norm() };
        assert( not math::is_zero(len) );
        return Vect3D{x/len, y/len, z/len};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] double dot_prod_with(const Vect3D& other) const noexcept
       {
        return (x*other.x) + (y*other.y) + (z*other.z);
       }
    [[nodiscard]] friend double dot_prod(const Vect3D& v1, const Vect3D& v2) noexcept
       {
        return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect3D cross_prod(const Vect3D& v1, const Vect3D& v2) noexcept
       {
        return Vect3D{ (v1.y*v2.z)-(v2.y*v1.z),
                       (v2.x*v1.z)-(v1.x*v2.z),
                       (v1.x*v2.y)-(v2.x*v1.y) };
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] Vect3D component_on(const Vect3D& other) const noexcept
       {
        assert( not other.is_null() );
        return (dot_prod_with(other) / other.norm2()) * other;
       }

   //-----------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Vect3D& v)
       {
        os << v.x << ',' << v.y << ',' << v.z;
        return os;
       }
};


////////////////////////////////////////////////////////////////////////
template <> struct std::formatter<Vect2D> : std::formatter<std::string>
{
    auto format(const Vect2D v, std::format_context& ctx) const
       {
        return std::formatter<std::string>::format( std::format("{:.6g},{:.6g},{:.6g}", v.x, v.y, v.z), ctx);
       }
};