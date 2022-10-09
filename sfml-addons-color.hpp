#ifndef GUARD_sfml_addons_color_hpp
#define GUARD_sfml_addons_color_hpp
//  ---------------------------------------------
//  An enhanced color class for SFML
//  ---------------------------------------------
#include <cstdint> // std::uint32_t, std::uint8_t
#include <limits> // std::numeric_limits
#include <cmath> // std::fmod
//#include <fmt/core.h> // fmt::*
#include <SFML/Graphics/Color.hpp> // sf::Color


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace sfadd //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

/////////////////////////////////////////////////////////////////////////////
class Color : public sf::Color
{
 public:
    using sf::Color::Color; // Inherit all constructors

    //-----------------------------------------------------------------------
    Color& invert() noexcept
       {
        r = std::numeric_limits<uint8_t>::max() - r;
        g = std::numeric_limits<uint8_t>::max() - g;
        b = std::numeric_limits<uint8_t>::max() - b;
        return *this;
       }

    // String representation
    //[[nodiscard]] constexpr std::string rgb_string() const { return fmt::format("({},{},{},{})",red(),green(),blue(),alpha()); }
    //[[nodiscard]] std::string hex_string() const { return fmt::format("{:#08x}", rgba_value()); }
};


}//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
