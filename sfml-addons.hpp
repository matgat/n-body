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

    void operator()(sf::RenderWindow& window, sf::View& view, const sf::Vector2i& mouse_pix) noexcept
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
    void operator()(sf::RenderWindow& window, sf::View& view, const sf::Vector2i& mouse_pix, const bool out) noexcept
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

    [[nodiscard]] float width_of(const unsigned int pix) noexcept { return kx * static_cast<float>(pix); }
    [[nodiscard]] float height_of(const unsigned int pix) noexcept { return ky * static_cast<float>(pix); }
};


/////////////////////////////////////////////////////////////////////////////
class View
{
 private:
    sf::RenderWindow& i_window;
    sf::View i_view;
    Pan i_pan;
    Zoom i_zoom;

 public:
    explicit View(sf::RenderWindow& w) noexcept
      : i_window(w)
      , i_view(w.getDefaultView())
      {}

    //[[nodiscard]] sf::View& view() noexcept { return i_view; }

    //const sf::FloatRect rect() const noexcept
    //   {
    //    i_view.getCenter()
    //    i_view.getSize()
    //   }

    void resize(const unsigned int x_pix, const unsigned int y_pix) noexcept
       {
        i_view.setSize(i_zoom.width_of(x_pix), i_zoom.height_of(y_pix));
        i_window.setView(i_view);
       }

    void pan_init(const sf::Vector2i& mouse_pix) noexcept { i_pan.init(mouse_pix); }
    void pan_end() noexcept { i_pan.end(); }
    [[nodiscard]] bool panning() const noexcept { return i_pan.is_active(); }
    void pan(const sf::Vector2i& mouse_pix) noexcept { i_pan(i_window,i_view,mouse_pix); }

    void zoom(const sf::Vector2i& mouse_pix, const bool out) noexcept { i_zoom(i_window,i_view,mouse_pix,out); }
};


}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit --------------------------------------------------------------
#endif
