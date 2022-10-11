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
#include <fmt/color.h> // fmt::color::*

#include <SFML/Graphics/Color.hpp> // sf::Color


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace sfadd //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

/////////////////////////////////////////////////////////////////////////////
class Color : public sf::Color
{
 private:
    struct hsl_t
       {
        float h, s, l;
        [[nodiscard]] static constexpr bool is_zero(const float v) noexcept { return v>(-0.01f) && v<0.01f; }
        [[nodiscard]] constexpr friend bool operator==(const hsl_t& l, const hsl_t& r) noexcept { return is_zero(l.h-r.h) && is_zero(l.s-r.s) && is_zero(l.l-r.l); }
        [[nodiscard]] constexpr bool is_equal_to(const std::initializer_list<float>& r) const noexcept { return r.size()==3 && is_zero(h-*(r.begin())) && is_zero(s-*(r.begin()+1)) && is_zero(l-*(r.begin()+2)); }
        [[nodiscard]] static constexpr float clamp(const float val) noexcept
           {
            if(val>=1.0f) return 1.0f;
            else if(val<=0.0f) return 0.0f;
            return val;
           }
        [[nodiscard]] std::string string() const { return fmt::format("({:.2f},{:.2f},{:.2f})",h,s,l); }
       };

    struct rgb_t
       {
        std::uint8_t r, g, b;

        static constexpr std::uint8_t max = std::numeric_limits<uint8_t>::max();

        [[nodiscard]] static constexpr std::uint8_t incr(const std::uint8_t v,const std::uint8_t d) noexcept
           {
            return d<(rgb_t::max-v) ? v+d : rgb_t::max;
           }
        [[nodiscard]] static constexpr std::uint8_t decr(const std::uint8_t v,const std::uint8_t d) noexcept
           {
            return d<v ? v-d : std::numeric_limits<uint8_t>::min();
           }

        [[nodiscard]] constexpr friend bool operator==(const rgb_t& l, const rgb_t& r) noexcept { return l.r==r.r && l.g==r.g && l.b==r.b; }
        [[nodiscard]] constexpr bool is_equal_to(const std::initializer_list<std::uint8_t>& lst) const noexcept { return lst.size()==3 && r==*(lst.begin()) && g==*(lst.begin()+1) && b==*(lst.begin()+2); }
       };

 public:
    using sf::Color::Color; // Inherit all constructors
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
    [[maybe_unused]] constexpr Color& invert() noexcept
       {
        r = rgb_t::max - r;
        g = rgb_t::max - g;
        b = rgb_t::max - b;
        return *this;
       }


    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& redder(const std::uint8_t d) noexcept
       {
        std::uint8_t r_incr = rgb_t::incr(r, d/2);
        const std::uint8_t decr = rgb_t::decr(d, rgb_t::decr(r_incr, r)); // Ensure a difference d
        const std::uint8_t g_decr = rgb_t::decr(g, decr);
        const std::uint8_t b_decr = rgb_t::decr(b, decr);
        // Ensure the difference:
             if(g_decr==0) r_incr = rgb_t::incr(r, d - g);
        else if(b_decr==0) r_incr = rgb_t::incr(r, d - b);
        return set_rgb(r_incr, g_decr, b_decr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& greener(const std::uint8_t d) noexcept
       {
        std::uint8_t g_incr = rgb_t::incr(g, d/2);
        const std::uint8_t decr = rgb_t::decr(d, rgb_t::decr(g_incr,g)); // Ensure a difference d
        const std::uint8_t r_decr = rgb_t::decr(r, decr);
        const std::uint8_t b_decr = rgb_t::decr(b, decr);
        // Ensure the difference:
             if(r_decr==0) g_incr = rgb_t::incr(g, d - r);
        else if(b_decr==0) g_incr = rgb_t::incr(g, d - b);
        return set_rgb(r_decr, g_incr, b_decr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& bluer(const std::uint8_t d) noexcept
       {
        std::uint8_t b_incr = rgb_t::incr(b, d/2);
        const std::uint8_t decr = rgb_t::decr(d, rgb_t::decr(b_incr,b)); // Ensure a difference d
        const std::uint8_t r_decr = rgb_t::decr(r, decr);
        const std::uint8_t g_decr = rgb_t::decr(g, decr);
        // Ensure the difference:
             if(r_decr==0) b_incr = rgb_t::incr(b, d - r);
        else if(g_decr==0) b_incr = rgb_t::incr(b, d - g);
        return set_rgb(r_decr, g_decr, b_incr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& yellower(const std::uint8_t d) noexcept
       {
        std::uint8_t b_decr = rgb_t::decr(b, d/2);
        const std::uint8_t incr = rgb_t::decr(d, rgb_t::decr(b,b_decr)); // Ensure a difference d
        const std::uint8_t r_incr = rgb_t::incr(r, incr);
        const std::uint8_t g_incr = rgb_t::incr(g, incr);
        // Ensure the difference:
             if(r_incr==rgb_t::max) b_decr = rgb_t::decr(b, rgb_t::decr(d,rgb_t::decr(r_incr,r)));
        else if(g_incr==rgb_t::max) b_decr = rgb_t::decr(b, rgb_t::decr(d,rgb_t::decr(g_incr,g)));
        return set_rgb(r_incr, g_incr, b_decr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& purpler(const std::uint8_t d) noexcept
       {
        std::uint8_t g_decr = rgb_t::decr(g, d/2);
        const std::uint8_t incr = rgb_t::decr(d, rgb_t::decr(g,g_decr)); // Ensure a difference d
        const std::uint8_t r_incr = rgb_t::incr(r, incr);
        const std::uint8_t b_incr = rgb_t::incr(b, incr);
        // Ensure the difference:
             if(r_incr==rgb_t::max) g_decr = rgb_t::decr(g, rgb_t::decr(d,rgb_t::decr(r_incr,r)));
        else if(b_incr==rgb_t::max) g_decr = rgb_t::decr(g, rgb_t::decr(d,rgb_t::decr(b_incr,b)));
        return set_rgb(r_incr, g_decr, b_incr);
       }

    //-----------------------------------------------------------------------
    [[maybe_unused]] constexpr Color& cyaner(const std::uint8_t d) noexcept
       {
        std::uint8_t r_decr = rgb_t::decr(r, d/2);
        const std::uint8_t incr = rgb_t::decr(d, rgb_t::decr(r,r_decr)); // Ensure a difference d
        const std::uint8_t g_incr = rgb_t::incr(g, incr);
        const std::uint8_t b_incr = rgb_t::incr(b, incr);
        // Ensure the difference:
             if(g_incr==rgb_t::max) r_decr = rgb_t::decr(r, rgb_t::decr(d,rgb_t::decr(g_incr,g)));
        else if(b_incr==rgb_t::max) r_decr = rgb_t::decr(r, rgb_t::decr(d,rgb_t::decr(b_incr,b)));
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
        return set_hsl({h, hsl_t::clamp(s+d), l});
       }
    [[maybe_unused]] constexpr Color& grayer(const float d) noexcept { return sat_incr(-d); }


    //-----------------------------------------------------------------------
    // Adjust luminance [0.0÷1.0]
    [[maybe_unused]] constexpr Color& lum_incr(const float d) noexcept
       {
        const auto [h,s,l] = get_hsl();
        return set_hsl({h, s, hsl_t::clamp(l+d)});
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

        return set_hsl({h, s, hsl_t::clamp(l)});
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

        // [luminance]
        const float maxsum = 2.0f * static_cast<float>(rgb_t::max);
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
            out.s = rgbdiff / (out.l<=0.5f ? rgbsum : maxsum-rgbsum); // Equiv to rgbsum<=rgb_t::max

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
        const auto hue2rgb = [](float h, const float k, float ch) noexcept -> float
           {
            h = std::fmod(h, 360.0f);
            if     (h <  60.0f) ch += (k - ch) * h / 60.0f;
            else if(h < 180.0f) ch = k;
            else if(h < 240.0f) ch += (k - ch) * (240.0f - h) / 60.0f;
            return ch;
           };
        const float chn_max = static_cast<float>(rgb_t::max);
        if( hsl_t::is_zero(in.s) )
           {// Grayscale
            r = g = b = static_cast<std::uint8_t>(chn_max * in.l);
           }
        else
           {
            const float k = (in.l <= 0.5f) ? in.l * (1.0f+in.s) : in.l+in.s - in.l*in.s;
            const float ch0 = 2.0f*in.l - k;
            r = static_cast<std::uint8_t>(chn_max * hue2rgb(in.h + 120.0f, k, ch0));
            g = static_cast<std::uint8_t>(chn_max * hue2rgb(in.h         , k, ch0));
            b = static_cast<std::uint8_t>(chn_max * hue2rgb(in.h - 120.0f, k, ch0));
           }
       }


    // String representation
    [[nodiscard]] /*constexpr*/ std::string rgba_string() const { return fmt::format("({},{},{},{})",r,g,b,a); }
    [[nodiscard]] /*constexpr*/ std::string hex_string() const { return fmt::format("{:#08x}", toInteger()); }
};


}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



/////////////////////////////////////////////////////////////////////////////
#ifdef TEST /////////////////////////////////////////////////////////////////
int main()
{
    int errors = 0;

    Color c(254,58,58); // Reddish

    fmt::print("hsl components... ");
    if( const auto hsl = c.get_hsl(); hsl.is_equal_to({0.00f,0.99f,0.61f}) )
       {
        fmt::print(fg(fmt::color::green), "ok hsl={}\n",hsl.string());
       }
    else
       {
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Wrong hsl={}\n",hsl.string());
        ++errors;
       }

    fmt::print("hue setting... ");
    if( const auto hsl = c.set_hue(31.5f).get_hsl();
        hsl.is_equal_to({31.5f,0.99f,0.61f}) && c.has_same_rgb_of({254,163,58}) )
       {
        fmt::print(fg(fmt::color::green), "ok hsl={}, rgba={}\n",hsl.string(),c.rgba_string());
       }
    else
       {
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Wrong hsl={}, rgba={}\n",hsl.string(),c.rgba_string());
        ++errors;
       }

    return errors;
}
#endif //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



//---- end unit -------------------------------------------------------------
#endif
