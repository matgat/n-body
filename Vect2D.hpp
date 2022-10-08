#ifndef GUARD_vect2d_hpp
#define GUARD_vect2d_hpp
//  ---------------------------------------------
//  A 2D physical vector
//  ---------------------------------------------
#include <cassert> // assert
#include <ostream> // std::ostream
#include <fmt/format.h> // fmt::formatter
#include "math-utilities.hpp" // math::*


////////////////////////////////////////////////////////////////////////
struct Vect2D final
{
    double x, y;

    Vect2D() noexcept : x(0.0), y(0.0) {}
    Vect2D(const double X, const double Y) noexcept : x(X), y(Y) {}

    //-----------------------------------------------------------------------
    [[nodiscard]] bool is_null() const noexcept
       {
        return math::is_zero(norm2());
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] bool operator==(const Vect2D& other) const noexcept
       {
        return math::is_zero(x-other.x) && math::is_zero(y-other.y);
       }

    //-----------------------------------------------------------------------
    void operator+=(const Vect2D& other) noexcept
       {
        x+=other.x; y+=other.y;
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] Vect2D operator-() const noexcept
       {
        return Vect2D{-x, -y};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect2D operator+(const Vect2D& v1, const Vect2D& v2) noexcept
       {
        return Vect2D{v1.x+v2.x, v1.y+v2.y};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect2D operator-(const Vect2D& v1, const Vect2D& v2) noexcept
       {
        return Vect2D{v1.x-v2.x, v1.y-v2.y};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect2D operator*(const double k, const Vect2D& v) noexcept
       {
        return Vect2D{k*v.x, k*v.y};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] friend Vect2D operator/(const Vect2D& v, const double k) noexcept
       {
        assert(k!=0.0);
        return Vect2D{v.x/k, v.y/k};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] double norm2() const noexcept { return x*x + y*y; }
    [[nodiscard]] double norm() const noexcept { return std::sqrt(norm2()); }
    [[nodiscard]] Vect2D& normalize() noexcept
       {
        const double len{ norm() };
        assert( !math::is_zero(len) );
        x/=len;
        y/=len;
        return *this;
       }
    [[nodiscard]] Vect2D get_versor() const noexcept
       {
        const double len{ norm() };
        assert( !math::is_zero(len) );
        return Vect2D{x/len, y/len};
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] double dot_prod_with(const Vect2D& other) const noexcept
       {
        return (x*other.x) + (y*other.y);
       }
    [[nodiscard]] friend double dot_prod(const Vect2D& v1, const Vect2D& v2) noexcept
       {
        return (v1.x*v2.x) + (v1.y*v2.y);
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] Vect2D component_on(const Vect2D& other) const noexcept
       {
        assert( !other.is_null() );
        return (dot_prod_with(other) / other.norm2()) * other;
       }

    //-----------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Vect2D& v)
       {
        os << v.x << ',' << v.y;
        return os;
       }
};


////////////////////////////////////////////////////////////////////////
template<> struct fmt::formatter<Vect2D> final
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) noexcept { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const Vect2D& v, FormatContext& ctx) const
       {
        return fmt::format_to(ctx.out(), "{:.6g},{:.6g}", v.x, v.y);
       }
};


//---- end unit --------------------------------------------------------------
#endif
