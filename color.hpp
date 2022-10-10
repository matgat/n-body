#ifndef GUARD_sfml_addons_color_hpp
#define GUARD_sfml_addons_color_hpp
//  ---------------------------------------------
//  An enhanced color class for SFML
//  ---------------------------------------------
#include <cassert>
#include <cstdint> // std::uint32_t, std::uint8_t
#include <limits> // std::numeric_limits
#include <cmath> // std::fmod
#include <algorithm> // std::minmax
#include <fmt/core.h> // fmt::format



/////////////////////////////////////////////////////////////////////////////
class Color
{
 private:
    std::uint8_t r,g,b,a; // Components

    struct hsl_t {float h, s, l;};
    struct rgb_t {std::uint8_t r, g, b;};

 public:
    constexpr Color() noexcept
      : r(0)
      , g(0)
      , b(0)
      , a(channel_max) {}

    constexpr Color(const std::uint8_t red,
                    const std::uint8_t green,
                    const std::uint8_t blue,
                    const std::uint8_t alpha =channel_max) noexcept
      : r(red)
      , g(green)
      , b(blue)
      , a(alpha) {}

    constexpr explicit Color(const std::uint32_t rgba) noexcept
      : r(static_cast<std::uint8_t>((rgba & 0xff000000) >> 24))
      , g(static_cast<std::uint8_t>((rgba & 0x00ff0000) >> 16))
      , b(static_cast<std::uint8_t>((rgba & 0x0000ff00) >> 8))
      , a(static_cast<std::uint8_t>(rgba & 0x000000ff)) {}

    constexpr Color(const float h,
                    const float s,
                    const float l,
                    const float opacity =1.0f) noexcept
      : a(static_cast<std::uint8_t>(opacity * static_cast<float>(channel_max)))
       {
        hsl2rgb({h,s,l}, r,g,b);
       }

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
    [[maybe_unused]] constexpr Color& set_rgb(const std::uint8_t red,
                                              const std::uint8_t green,
                                              const std::uint8_t blue) noexcept
       {
        r = red;
        g = green;
        b = blue;
        return *this;
       }
    //[[maybe_unused]] constexpr Color& set_alpha(const std::uint8_t alpha) noexcept
    //   {
    //    a = alpha;
    //    return *this;
    //   }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& operator+=(const Color other) noexcept
       {
        r = channel_incr(r, other.r);
        g = channel_incr(g, other.g);
        b = channel_incr(b, other.b);
        a = channel_incr(a, other.a);
        return *this;
       }
    [[nodiscard]] friend constexpr Color operator+(Color left, const Color right) noexcept
       {
        return left += right;
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& operator-=(const Color other) noexcept
       {
        r = channel_decr(r, other.r);
        g = channel_decr(g, other.g);
        b = channel_decr(b, other.b);
        a = channel_decr(a, other.a);
        return *this;
       }
    [[nodiscard]] friend constexpr Color operator-(Color left, const Color right) noexcept
       {
        return left -= right;
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& operator*=(const Color other) noexcept
       {
        const auto channel_mul = [](const std::uint8_t lhs, const std::uint8_t rhs) -> std::uint8_t
           {
            const std::uint16_t prod = static_cast<std::uint16_t>(lhs) * static_cast<std::uint16_t>(rhs);
            return static_cast<std::uint8_t>(prod / channel_max);
           };
        r = channel_mul(r, other.r),
        g = channel_mul(g, other.g),
        b = channel_mul(b, other.b),
        a = channel_mul(a, other.a);
        return *this;
       }
    [[nodiscard]] friend constexpr Color operator*(Color left, const Color right) noexcept
       {
        return left *= right;
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& invert() noexcept
       {
        r = channel_max - r;
        g = channel_max - g;
        b = channel_max - b;
        return *this;
       }


    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& redder(const std::uint8_t d) noexcept
       {
        std::uint8_t r_incr = channel_incr(r, d/2);
        const std::uint8_t decr = d - (r_incr - r); // Ensure a difference d
        const std::uint8_t g_decr = channel_decr(g, decr);
        const std::uint8_t b_decr = channel_decr(b, decr);
        // Ensure the difference:
             if(g_decr==0) r_incr = channel_incr(r, d - g);
        else if(b_decr==0) r_incr = channel_incr(r, d - b);
        return set_rgb(r_incr, g_decr, b_decr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& greener(const std::uint8_t d) noexcept
       {
        std::uint8_t g_incr = channel_incr(g, d/2);
        const std::uint8_t decr = d - (g_incr - g); // Ensure a difference d
        const std::uint8_t r_decr = channel_decr(r, decr);
        const std::uint8_t b_decr = channel_decr(b, decr);
        // Ensure the difference:
             if(r_decr==0) g_incr = channel_incr(g, d - r);
        else if(b_decr==0) g_incr = channel_incr(g, d - b);
        return set_rgb(r_decr, g_incr, b_decr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& bluer(const std::uint8_t d) noexcept
       {
        std::uint8_t b_incr = channel_incr(b, d/2);
        const std::uint8_t decr = d - (b_incr - b); // Ensure a difference d
        const std::uint8_t r_decr = channel_decr(r, decr);
        const std::uint8_t g_decr = channel_decr(g, decr);
        // Ensure the difference:
             if(r_decr==0) b_incr = channel_incr(b, d - r);
        else if(g_decr==0) b_incr = channel_incr(b, d - g);
        return set_rgb(r_decr, g_decr, b_incr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& yellower(const std::uint8_t d) noexcept
       {
        std::uint8_t b_decr = channel_decr(b, d/2);
        const std::uint8_t incr = d - (b - b_decr); // Ensure a difference d
        const std::uint8_t r_incr = channel_incr(r, incr);
        const std::uint8_t g_incr = channel_incr(g, incr);
        // Ensure the difference:
             if(r_incr==channel_max) b_decr = channel_decr(b, d - (r_incr - r));
        else if(g_incr==channel_max) b_decr = channel_decr(b, d - (g_incr - g));
        return set_rgb(r_incr, g_incr, b_decr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& purpler(const std::uint8_t d) noexcept
       {
        std::uint8_t g_decr = channel_decr(g, d/2);
        const std::uint8_t incr = d - (g - g_decr); // Ensure a difference d
        const std::uint8_t r_incr = channel_incr(r, incr);
        const std::uint8_t b_incr = channel_incr(b, incr);
        // Ensure the difference:
             if(r_incr==channel_max) g_decr = channel_decr(g, d - (r_incr - r));
        else if(b_incr==channel_max) g_decr = channel_decr(g, d - (b_incr - b));
        return set_rgb(r_incr, g_decr, b_incr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& cyaner(const std::uint8_t d) noexcept
       {
        std::uint8_t r_decr = channel_decr(r, d/2);
        const std::uint8_t incr = d - (r - r_decr); // Ensure a difference d
        const std::uint8_t g_incr = channel_incr(g, incr);
        const std::uint8_t b_incr = channel_incr(b, incr);
        // Ensure the difference:
             if(g_incr==channel_max) r_decr = channel_decr(r, d - (g_incr - g));
        else if(b_incr==channel_max) r_decr = channel_decr(r, d - (b_incr - b));
        return set_rgb(r_decr, g_incr, b_incr);
       }


    //-----------------------------------------------------------------------
    // [Hue Saturation Luminance]
    [[nodiscard]] constexpr float hue() const noexcept { const auto [h,s,l] = get_hsl(); return h; } // [0.0÷360.0] (Hue)
    [[nodiscard]] constexpr float sat() const noexcept { const auto [h,s,l] = get_hsl(); return s; } // [0.0÷1.0] (Saturation)
    [[nodiscard]] constexpr float lum() const noexcept { const auto [h,s,l] = get_hsl(); return l; } // [0.0÷1.0] (Luminance)

    [[maybe_unused]] constexpr Color& set_hue(const float hⁿᵉʷ) noexcept { const auto [h,s,l] = get_hsl(); return set_hsl({hⁿᵉʷ,s,l}); }
    [[maybe_unused]] constexpr Color& set_sat(const float sⁿᵉʷ) noexcept { const auto [h,s,l] = get_hsl(); return set_hsl({h,sⁿᵉʷ,l}); }
    [[maybe_unused]] constexpr Color& set_lum(const float lⁿᵉʷ) noexcept { const auto [h,s,l] = get_hsl(); return set_hsl({h,s,lⁿᵉʷ}); }


    //-----------------------------------------------------------------------
    // Adjust Hue [0.0÷360.0]
    [[maybe_unused]] constexpr Color& hue_incr(const float d) noexcept
       {
        const auto [h,s,l] = get_hsl();
        return set_hsl({h+d,s,l});
       }

    //-----------------------------------------------------------------------
    // Adjust saturation [0.0÷1.0]
    [[maybe_unused]] constexpr Color& sat_incr(const float d) noexcept
       {
        const auto [h,s,l] = get_hsl();
        return set_hsl({h, clamp_01(s+d), l});
       }
    [[maybe_unused]] constexpr Color& grayer(const float d) noexcept { return sat_incr(-d); }


    //-----------------------------------------------------------------------
    // Adjust luminance [0.0÷1.0]
    [[maybe_unused]] constexpr Color& lum_incr(const float d) noexcept
       {
        const auto [h,s,l] = get_hsl();
        return set_hsl({h, s, clamp_01(l+d)});
       }

    [[nodiscard]] constexpr bool is_dark() const noexcept { return lum()<0.25; }

    [[maybe_unused]] constexpr Color&  invert_lum() noexcept
       {
        const auto [h,s,l] = get_hsl();
        return set_hsl({h,s,1.0f-l});
       }

    [[maybe_unused]] constexpr Color& diversify_lum_from(const Color& other, const float min_lum_diff) noexcept
       {// Ensure a brightness difference respect a background
        const float other_lum = other.lum();
        auto [h,s,l] = get_hsl();
        if( other_lum<0.25 )
           {// Dark background: Ensure brighter
            const float min_lum = other_lum+min_lum_diff;
            if( l<min_lum ) l = min_lum;
           }
        else
           {// Light background: Ensure darker
            const float max_lum = other_lum-min_lum_diff;
            if( l>max_lum ) l = max_lum;
           }

        return set_hsl({h, s, clamp_01(l)});
       }

    //-----------------------------------------------------------------------
    [[nodiscard]] constexpr hsl_t get_hsl() const noexcept
       {
        return rgb2hsl({r,g,b});
       }
    [[maybe_unused]] constexpr Color& set_hsl(const hsl_t hsl) noexcept
       {
        hsl2rgb(hsl, r,g,b);
        return *this;
       }

    //-----------------------------------------------------------------------
    // Convert color map RGB to HSL
    [[nodiscard]] static constexpr hsl_t rgb2hsl(const rgb_t in) noexcept
       {
        hsl_t out;

        // Retrieve minimum and maximum intensity of channels
        const auto [rgbmin, rgbmax] = std::minmax({in.r, in.g, in.b});

                                std::uint8_t rmin, rmax;
                                if( in.r < in.g )
                                   {
                                    if( in.g < in.b ) { rmin = in.r;  rmax = in.b; }
                                    else              { rmax = in.g;  rmin = in.r<in.b ? in.r : in.b; }
                                   }
                                else
                                   {
                                    if( in.r < in.b ) { rmin = in.g;  rmax = in.b; }
                                    else              { rmax = in.r;  rmin = in.g<in.b ? in.g : in.b; }
                                   }
                                if(rmin!=rgbmin || rmax!=rgbmax) fmt::print("minmax error: {}/{} not {}/{}\n",rgbmin,rgbmax,rmin,rmax);

        // [luminance]
        const float maxsum = 2.0f * static_cast<float>(channel_max);
        const float rgbsum = static_cast<float>(rgbmax) + static_cast<float>(rgbmin);
        out.l = rgbsum / maxsum; // Luminance

        if( rgbmax == rgbmin )
           { // It's a gray
            out.h = out.s = 0.0f;
           }
        else
           {
            // [saturation]
            const float rgbdiff = static_cast<float>(rgbmax) - static_cast<float>(rgbmin); // chroma
            out.s = rgbdiff / (out.l<=0.5f ? rgbsum : maxsum-rgbsum); // Equiv to rgbsum<=channel_max

            // [hue]
                 if(in.r == rgbmax)     out.h = std::fmod( 60.0f * (6.0f + (static_cast<float>(in.g)-static_cast<float>(in.b))/rgbdiff), 360.0f );
            else if(in.g == rgbmax)     out.h = std::fmod( 60.0f * (2.0f + (static_cast<float>(in.b)-static_cast<float>(in.r))/rgbdiff), 360.0f );
            else /*if(in.b == rgbmax)*/ out.h = std::fmod( 60.0f * (4.0f + (static_cast<float>(in.r)-static_cast<float>(in.g))/rgbdiff), 360.0f );
           }

        return out;
       }

    //-----------------------------------------------------------------------
    // Convert color map HSL -> RGB
    static constexpr void hsl2rgb(const hsl_t in, std::uint8_t& r, std::uint8_t& g, std::uint8_t& b) noexcept
       {
        assert(in.s>=0.0f && in.s<=1.0f && in.l>=0.0f && in.l<=1.0f);
        const auto hue2rgb = [](float h, const float k, float ch) noexcept -> std::uint8_t
           {
            //while(h > 360.0f) h -= 360.0f; while(h < 0.0f) h += 360.0f;
            h = std::fmod(h, 360.0f);
            if     (h <  60.0f) ch += (k - ch) * h / 60.0f;
            else if(h < 180.0f) ch = k;
            else if(h < 240.0f) ch += (k - ch) * (240.0f - h) / 60.0f;
            return static_cast<std::uint8_t>(ch);
           };

        if( std::abs(in.s)<std::numeric_limits<float>::epsilon() )
           {// Grayscale
            r = g = b = channel_max * static_cast<std::uint8_t>(in.l);
           }
        else
           {
            const float k = (in.l <= 0.5f) ? in.l * (1.0f+in.s) : in.l+in.s - in.l*in.s;
            const float ch = 2.0f*in.l - k;
            r = channel_max * hue2rgb(in.h + 120.0f, k, ch);
            g = channel_max * hue2rgb(in.h         , k, ch);
            b = channel_max * hue2rgb(in.h - 120.0f, k, ch);
           }
       }


    // String representation
    [[nodiscard]] constexpr std::string rgba_string() const { return fmt::format("({},{},{},{})",r,g,b,a); }
    [[nodiscard]] constexpr std::string hex_string() const { return fmt::format("{:#08x}", rgba_value()); }


 private:
    static constexpr std::uint8_t channel_max = std::numeric_limits<uint8_t>::max();

    static constexpr std::uint8_t channel_incr(const std::uint8_t v,const std::uint8_t d) noexcept
       {
        return d<(channel_max-v) ? v+d : channel_max;
       }

    static constexpr std::uint8_t channel_decr(const std::uint8_t v,const std::uint8_t d) noexcept
       {
        return d<v ? v-d : std::numeric_limits<uint8_t>::min();
       }

    [[nodiscard]] static constexpr float clamp_01(const float val) noexcept
       {
        if(val>=1.0f) return 1.0f;
        else if(val<=0.0f) return 0.0f;
        return val;
       }
};




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



//---- end unit --------------------------------------------------------------
#endif