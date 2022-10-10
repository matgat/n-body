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

#include <SFML/Graphics/Color.hpp> // sf::Color


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace sfadd //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

/////////////////////////////////////////////////////////////////////////////
class Color : public sf::Color
{
 private:
    struct hsl_t {float h, s, l;};
    struct rgb_t {std::uint8_t r, g, b;};

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

 private:
    static constexpr std::uint8_t channel_max = std::numeric_limits<uint8_t>::max();

    [[nodiscard]] static constexpr float clamp_01(const float val) noexcept
       {
        if(val>=1.0f) return 1.0f;
        else if(val<=0.0f) return 0.0f;
        return val;
       }
};


}//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
