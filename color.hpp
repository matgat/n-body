#ifndef GUARD_sfml_addons_color_hpp
#define GUARD_sfml_addons_color_hpp
//  ---------------------------------------------
//  An enhanced color class for SFML
//  ---------------------------------------------
#include <cstdint> // std::uint32_t, std::uint8_t
#include <limits> // std::numeric_limits
#include <cmath> // std::fmod


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace sfadd //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

/////////////////////////////////////////////////////////////////////////////
class Color
{
 private:
    using hsl_type = float; // Single HSL channel
    std::uint8_t r,g,b,a; // Components

    static constexpr std::uint8_t channel_max
    static constexpr std::uint8_t channel_incr(const std::uint8_t v,const std::uint8_t d) noexcept
       {
        return d<(std::numeric_limits<uint8_t>::max()-v) ? v+d : std::numeric_limits<uint8_t>::max();
       }
    static constexpr std::uint8_t channel_decr(const std::uint8_t v,const std::uint8_t d) noexcept
       {
        return d<v ? v-d : std::numeric_limits<uint8_t>::min();
       }

 public:
    constexpr Color::Color() noexcept
      : r(0)
      , g(0)
      , b(0)
      , a(255) {}

    constexpr Color::Color(const std::uint8_t red,
                           const std::uint8_t green,
                           const std::uint8_t blue,
                           const std::uint8_t alpha =255) noexcept
      : r(red)
      , g(green)
      , b(blue)
      , a(alpha) {}

    constexpr explicit Color::Color(const std::uint32_t rgba) noexcept
      : r(static_cast<std::uint8_t>((rgba & 0xff000000) >> 24))
      , g(static_cast<std::uint8_t>((rgba & 0x00ff0000) >> 16))
      , b(static_cast<std::uint8_t>((rgba & 0x0000ff00) >> 8))
      , a(static_cast<std::uint8_t>(rgba & 0x000000ff)) {}

    constexpr Color(const hsl_type h, const hsl_type s, const hsl_type l) noexcept
    {}

    //------------------------------------------------------------------------
    [[nodiscard]] constexpr std::uint32_t rgba_value() const noexcept
       {
        return static_cast<std::uint32_t>((r << 24) | (g << 16) | (b << 8) | a);
       }

    //------------------------------------------------------------------------
    [[nodiscard]] friend constexpr bool operator==(const Color& left, const Color& right) noexcept
       {
        return (left.r == right.r) && (left.g == right.g) && (left.b == right.b) && (left.a == right.a);
       }
    [[nodiscard]] friend constexpr bool operator!=(const Color& left, const Color& right) noexcept
       {
        return !(left == right);
       }

    //-----------------------------------------------------------------------
    Color& invert() noexcept
       {
        r = 255-r;
        g = 255-g;
        b = 255-b;
        return *this;
       }


    Color& red_incr(const std::uint8_t d) noexcept { return red( channel_incr(red(),d) ); }
    Color& green_incr(const std::uint8_t d) noexcept { return green( channel_incr(green(),d) ); }
    Color& blue_incr(const std::uint8_t d) noexcept { return blue( channel_incr(blue(),d) ); }

    Color& redder(const std::uint8_t d) noexcept
       {
        const std::uint8_t r=red(), g=green(), b=blue();
        std::uint8_t r_incr = channel_incr(r, d/2);
        const std::uint8_t decr = d - (r_incr - r); // Voglio assicurare una differenza d
        const std::uint8_t g_decr = rgb_decr(g, decr),
                       b_decr = rgb_decr(b, decr);
        // Potrei non essere riuscito a decrementare:
             if(g_decr==0) r_incr = channel_incr(r, d - g); // channel_incr(r, d - (g - g_decr[==0]));
        else if(b_decr==0) r_incr = channel_incr(r, d - b); // channel_incr(r, d - (b - b_decr[==0]));
        return set_rgb(r_incr, g_decr, b_decr);
       }
    Color& greener(const std::uint8_t d) noexcept
       {
        const std::uint8_t r=red(), g=green(), b=blue();
        std::uint8_t g_incr = channel_incr(g, d/2);
        const std::uint8_t decr = d - (g_incr - g); // Voglio assicurare una differenza d
        const std::uint8_t r_decr = rgb_decr(r, decr),
                       b_decr = rgb_decr(b, decr);
        // Potrei non essere riuscito a decrementare:
             if(r_decr==0) g_incr = channel_incr(g, d - r); // channel_incr(g, d - (r - r_decr[==0]));
        else if(b_decr==0) g_incr = channel_incr(g, d - b); // channel_incr(g, d - (b - b_decr[==0]));
        return set_rgb(r_decr, g_incr, b_decr);
       }
    Color& bluer(const std::uint8_t d) noexcept
       {
        const std::uint8_t r=red(), g=green(), b=blue();
        std::uint8_t b_incr = channel_incr(b, d/2);
        const std::uint8_t decr = d - (b_incr - b); // Voglio assicurare una differenza d
        const std::uint8_t r_decr = rgb_decr(r, decr),
                       g_decr = rgb_decr(g, decr);
        // Potrei non essere riuscito a decrementare:
             if(r_decr==0) b_incr = channel_incr(b, d - r); // channel_incr(b, d - (r - r_decr[==0]));
        else if(g_decr==0) b_incr = channel_incr(b, d - g); // channel_incr(b, d - (g - g_decr[==0]));
        return set_rgb(r_decr, g_decr, b_incr);
       }
    Color& yellower(const std::uint8_t d) noexcept
       {
        const std::uint8_t r=red(), g=green(), b=blue();
        std::uint8_t b_decr = rgb_decr(b, d/2);
        const std::uint8_t incr = d - (b - b_decr); // Voglio assicurare una differenza d
        const std::uint8_t r_incr = channel_incr(r, incr),
                       g_incr = channel_incr(g, incr);
        // Potrei non essere riuscito ad incrementare:
             if(r_incr==255) b_decr = rgb_decr(b, d - (r_incr - r));
        else if(g_incr==255) b_decr = rgb_decr(b, d - (g_incr - g));
        return set_rgb(r_incr, g_incr, b_decr);
       }
    Color& purpler(const std::uint8_t d) noexcept
       {
        const std::uint8_t r=red(), g=green(), b=blue();
        std::uint8_t g_decr = rgb_decr(g, d/2);
        const std::uint8_t incr = d - (g - g_decr); // Voglio assicurare una differenza d
        const std::uint8_t r_incr = channel_incr(r, incr),
                       b_incr = channel_incr(b, incr);
        // Potrei non essere riuscito ad incrementare:
             if(r_incr==255) g_decr = rgb_decr(g, d - (r_incr - r));
        else if(b_incr==255) g_decr = rgb_decr(g, d - (b_incr - b));
        return set_rgb(r_incr, g_decr, b_incr);
       }
    Color& cyaner(const std::uint8_t d) noexcept
       {
        const std::uint8_t r=red(), g=green(), b=blue();
        std::uint8_t r_decr = rgb_decr(r, d/2);
        const std::uint8_t incr = d - (r - r_decr); // Voglio assicurare una differenza d
        const std::uint8_t g_incr = channel_incr(g, incr),
                       b_incr = channel_incr(b, incr);
        // Potrei non essere riuscito ad incrementare:
             if(g_incr==255) r_decr = rgb_decr(r, d - (g_incr - g));
        else if(b_incr==255) r_decr = rgb_decr(r, d - (b_incr - b));
        return set_rgb(r_decr, g_incr, b_incr);
       }

    Color& grayer(const hsl_type d) noexcept { return sat_incr(-d); }


    // [Hue Saturation Luminance]
    // Not efficient (Better use once: Color::hsl_type h,s,l; Color::col2hsl(col,h,s,l);)
    hsl_type hue() const noexcept { hsl_type h,s,l; col2hsl(i_rgba,h,s,l); return h; } // [0.0÷360.0] (Hue)
    hsl_type sat() const noexcept { hsl_type h,s,l; col2hsl(i_rgba,h,s,l); return s; } // [0.0÷1.0] (Saturation)
    hsl_type lum() const noexcept { hsl_type h,s,l; col2hsl(i_rgba,h,s,l); return l; } // [0.0÷1.0] (Brightness)

    Color& hue(const hsl_type& h_new) noexcept { hsl_type h,s,l; col2hsl(i_rgba,h,s,l); return operator=(hsl2col(h_new,s,l)); }
    Color& sat(const hsl_type& s_new) noexcept { hsl_type h,s,l; col2hsl(i_rgba,h,s,l); return operator=(hsl2col(h,s_new,l)); }
    Color& lum(const hsl_type& l_new) noexcept { hsl_type h,s,l; col2hsl(i_rgba,h,s,l); return operator=(hsl2col(h,s,l_new)); }
    Color& set_hsl(const hsl_type h,const hsl_type s,const hsl_type l) noexcept { return operator=(hsl2col(h,s,l)); }

    bool is_dark() const noexcept { return lum()<0.25; }

    //-----------------------------------------------------------------------
    // Adjust Hue [0.0÷360.0]
    static inline std::uint32_t hue_incr(const std::uint32_t C, const hsl_type d) noexcept
       {
        hsl_type h,s,l;
        col2hsl(C,h,s,l);
        h += d;
        return hsl2col(h,s,l);
       }
    Color& hue_incr(const hsl_type d) noexcept { return operator=(hue_incr(i_rgba,d)); }

    //-----------------------------------------------------------------------
    // Adjust brightness [0.0÷1.0]
    static inline std::uint32_t lum_incr(const std::uint32_t C, const hsl_type d) noexcept
       {
        hsl_type h,s,l;
        col2hsl(C,h,s,l);
        l += d;
        if(l<0.0) l=0.0; else if(l>1.0) l=1.0;
        return hsl2col(h,s,l);
       }
    Color& lum_incr(const hsl_type d) noexcept { return operator=(lum_incr(i_rgba,d)); }
    Color& invert_brightness() noexcept
       {
        hsl_type h,s,l; col2hsl(i_rgba,h,s,l);
        l = 1.0 - l;
        return operator=(hsl2col(h,s,l));
       }
    Color& diversify_lum_from(const Color& other, const hsl_type lum_diff) noexcept
       {// Ensure a brightness difference respect a background
        hsl_type other_lum = other.lum();
        hsl_type h,s,l; col2hsl(i_rgba,h,s,l);
        if( other_lum < 0.25 ) // other.is_dark()
           {// Dark background: ensure brighter
            if( (l-other_lum) < lum_diff ) l = other_lum+lum_diff;
           }
        else
           {// Light background: ensure darker
            if( (other_lum-l) < lum_diff ) l = other_lum-lum_diff;
           }
        if(l<0.0) l=0.0; else if(l>1.0) l=1.0;
        return operator=(hsl2col(h,s,l));
       }

    //-----------------------------------------------------------------------
    // Adjust saturation [0.0÷1.0]
    static inline std::uint32_t sat_incr(const std::uint32_t C, const hsl_type d) noexcept
       {
        hsl_type h,s,l; col2hsl(C,h,s,l);
        s += d;
        if(s<0.0) s=0.0; else if(s>1.0) s=1.0;
        return hsl2col(h,s,l);
       }
    Color& sat_incr(const hsl_type d) noexcept { return operator=(sat_incr(i_rgba,d)); }


    //-----------------------------------------------------------------------
    // Convert color map RGB to HSL
    static inline void rgb2hsl(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, hsl_type& h, hsl_type& s, hsl_type& l) noexcept
       {
        // Retrieve minimum and maximum intensity of channels
        std::uint8_t rgbmin, rgbmax;
        if( r < g )
           {
            if( g < b ) { rgbmin = r;  rgbmax = b; }
            else        { rgbmax = g;  rgbmin = r<b ? r : b; }
           }
        else
           {
            if( r < b ) { rgbmin = g;  rgbmax = b; }
            else        { rgbmax = r;  rgbmin = g<b ? g : b; }
           }

        hsl_type rgbsum = static_cast<hsl_type>(rgbmax) + static_cast<hsl_type>(rgbmin);
        l = rgbsum / (2.0*255); // Luminance

        if( rgbmax == rgbmin ) h = s = 0.0; // gray color
        else
           {
            hsl_type rgbdiff = static_cast<hsl_type>(rgbmax) - static_cast<hsl_type>(rgbmin); // chroma
            s = rgbdiff / (l<=0.5 ? rgbsum : (2.0*255)-rgbsum);
            //s = rgbdiff / (rgbsum<=255 ? rgbsum : (2.0*255)-rgbsum);
            // One of the three:
                 if(r == rgbmax)     h = std::fmod( 60.0 * (6.0 + (static_cast<hsl_type>(g)-static_cast<hsl_type>(b))/rgbdiff), 360.0 );
            else if(g == rgbmax)     h = std::fmod( 60.0 * (2.0 + (static_cast<hsl_type>(b)-static_cast<hsl_type>(r))/rgbdiff), 360.0 );
            else /*if(b == rgbmax)*/ h = std::fmod( 60.0 * (4.0 + (static_cast<hsl_type>(r)-static_cast<hsl_type>(g))/rgbdiff), 360.0 );
           }
       } // 'rgb2hsl'
    static inline void col2hsl(const std::uint32_t c, hsl_type& h, hsl_type& s, hsl_type& l) noexcept
       {
        rgb2hsl(red(c),green(c),blue(c), h,s,l);
       }
    void get_hsl(hsl_type& h, hsl_type& s, hsl_type& l) const noexcept
       {
        rgb2hsl(red(),green(),blue(), h,s,l);
       }

    //-----------------------------------------------------------------------
    // Convert color map HSL -> RGB
    static void hsl2rgb(const hsl_type h, const hsl_type s, const hsl_type l, std::uint8_t& r, std::uint8_t& g, std::uint8_t& b) noexcept
       {
        struct local   {// Auxiliary local function
                        static inline hsl_type hue2rgb(hsl_type h, const hsl_type k, hsl_type ch) noexcept
                           {
                            //while(h > 360.0) h -= 360.0; while(h < 0.0) h += 360.0;
                            h = std::fmod(h, 360.0);
                            if     (h <  60.0) ch += (k - ch) * h / 60.0;
                            else if(h < 180.0) ch = k;
                            else if(h < 240.0) ch += (k - ch) * (240.0 - h) / 60.0;
                            return ch;
                           }
                       };
        if(s == 0.0) // Grayscale
           {
            r = g = b = static_cast<std::uint8_t>(255 * l);
           }
        else
           {
            hsl_type k = (l <= 0.5) ? l * (1.0+s) : l+s - l*s;
            hsl_type ch = 2.0*l - k;
            r = static_cast<std::uint8_t>(255 * local::hue2rgb(h + 120.0, k, ch) );
            g = static_cast<std::uint8_t>(255 * local::hue2rgb(h        , k, ch) );
            b = static_cast<std::uint8_t>(255 * local::hue2rgb(h - 120.0, k, ch) );
           }
       } // 'hsl2rgb'
    static inline std::uint32_t hsl2col(const hsl_type h, const hsl_type s, const hsl_type l) noexcept
       {
        std::uint8_t r,g,b;
        hsl2rgb(h,s,l, r,g,b);
        return compose_rgba(r,g,b);
       }

    // String representation
    //[[nodiscard]] constexpr std::string rgb_string() const { return fmt::format("({},{},{},{})",red(),green(),blue(),alpha()); }
    //[[nodiscard]] std::string hex_string() const { return fmt::format("{:#08x}", rgba_value()); }

    // Interaction with sf::Color
    [[nodiscard]] constexpr Color(const sf::Color& other) noexcept : i_rgba(other.toInteger()) {}
    [[nodiscard]] constexpr operator sf::Color() const noexcept { return sf::Color{rgba_value()}; }
    [[nodiscard]] constexpr bool operator==(const sf::Color& other)
       {
        return rgba_value() == other.toInteger();
       }
};








    constexpr Color operator+(const Color& left, const Color& right)
    {
        const auto clampedAdd = [](std::uint8_t lhs, std::uint8_t rhs) -> std::uint8_t
        {
            const int intResult = static_cast<int>(lhs) + static_cast<int>(rhs);
            return static_cast<std::uint8_t>(intResult < 255 ? intResult : 255);
        };

        return Color(clampedAdd(left.r, right.r),
                     clampedAdd(left.g, right.g),
                     clampedAdd(left.b, right.b),
                     clampedAdd(left.a, right.a));
    }



    constexpr Color operator-(const Color& left, const Color& right)
    {
        const auto clampedSub = [](std::uint8_t lhs, std::uint8_t rhs) -> std::uint8_t
        {
            const int intResult = static_cast<int>(lhs) - static_cast<int>(rhs);
            return static_cast<std::uint8_t>(intResult > 0 ? intResult : 0);
        };

        return Color(clampedSub(left.r, right.r),
                     clampedSub(left.g, right.g),
                     clampedSub(left.b, right.b),
                     clampedSub(left.a, right.a));
    }



    constexpr Color operator*(const Color& left, const Color& right)
    {
        const auto scaledMul = [](std::uint8_t lhs, std::uint8_t rhs) -> std::uint8_t
        {
            const auto uint16Result = static_cast<std::uint16_t>(
                static_cast<std::uint16_t>(lhs) * static_cast<std::uint16_t>(rhs));
            return static_cast<std::uint8_t>(uint16Result / 255u);
        };

        return Color(scaledMul(left.r, right.r),
                     scaledMul(left.g, right.g),
                     scaledMul(left.b, right.b),
                     scaledMul(left.a, right.a));
    }



    constexpr Color& operator+=(Color& left, const Color& right)
    {
        return left = left + right;
    }



    constexpr Color& operator-=(Color& left, const Color& right)
    {
        return left = left - right;
    }



    constexpr Color& operator*=(Color& left, const Color& right)
    {
        return left = left * right;
    }


// Note: the 'inline' keyword here is technically not required, but VS2019 fails
// to compile with a bogus "multiple definition" error if not explicitly used.
//inline constexpr Color Color::Black(0, 0, 0);
//inline constexpr Color Color::White(255, 255, 255);
//inline constexpr Color Color::Red(255, 0, 0);
//inline constexpr Color Color::Green(0, 255, 0);
//inline constexpr Color Color::Blue(0, 0, 255);
//inline constexpr Color Color::Yellow(255, 255, 0);
//inline constexpr Color Color::Magenta(255, 0, 255);
//inline constexpr Color Color::Cyan(0, 255, 255);
//inline constexpr Color Color::Transparent(0, 0, 0, 0);



}//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
