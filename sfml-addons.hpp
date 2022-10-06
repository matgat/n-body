#ifndef GUARD_sfml_addons_hpp
#define GUARD_sfml_addons_hpp
//  ---------------------------------------------
//  Some SFML facilities
//  ---------------------------------------------
#include <SFML/Graphics.hpp>


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace sfadd //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

/////////////////////////////////////////////////////////////////////////////
class Pan
{
 private:
    sf::Vector2i prev_mouse_pix;
    bool active=false;

 public:
    void init(const sf::Vector2i& mouse_pix) noexcept
        {
        prev_mouse_pix = mouse_pix;
        active = true;
        }

    void end() noexcept
        {
        active = false;
        }

    [[nodiscard]] bool is_active() const noexcept { return active; }

    void operator()(sf::RenderWindow& window, sf::View& view, const sf::Vector2i& mouse_pix)
        {
        view.move(sf::Vector2f(window.mapPixelToCoords(prev_mouse_pix) - window.mapPixelToCoords(mouse_pix)));
        prev_mouse_pix = mouse_pix;
        window.setView(view);
        }
};



/////////////////////////////////////////////////////////////////////////////
class Zoom
{
 private:
    const float k{ 1.1f }; // 10%
    float kx = 1.0f;
    float ky = 1.0f;

 public:
    void operator()(sf::RenderWindow& window, sf::View& view, const sf::Vector2i& mouse_pix, const bool out)
       {
        const sf::Vector2f center{ window.mapPixelToCoords(mouse_pix) };
        view.zoom( out ? (1.0f/k) : k );
        window.setView(view);
        view.move(center - window.mapPixelToCoords(mouse_pix));
        window.setView(view);
        // Remember the scaling factors in order to keep them on resize
        const sf::IntRect Rpix = window.getViewport(view);
        const sf::Vector2f view_size = view.getSize();
        kx = Rpix.width!=0 ? view_size.x/static_cast<float>(Rpix.width) : 1.0f;
        ky = Rpix.height!=0 ? view_size.y/static_cast<float>(Rpix.height) : 1.0f;
       }

    [[nodiscard]] float width_of(const unsigned int pix) { return kx * static_cast<float>(pix); }
    [[nodiscard]] float height_of(const unsigned int pix) { return ky * static_cast<float>(pix); }
};

}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
