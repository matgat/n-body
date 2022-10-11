#ifndef GUARD_sfml_addons_hpp
#define GUARD_sfml_addons_hpp
//  ---------------------------------------------
//  Some SFML facilities
//  ---------------------------------------------
#include <cmath> // std::ceil
#include <SFML/Graphics.hpp> // sf::*


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace sfadd //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
{

/////////////////////////////////////////////////////////////////////////////
class Pan
{
 private:
    sf::Vector2i prev_mouse_pix;
    bool active = false;

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
        const sf::Vector2f& view_size = view.getSize();
        kx = Rpix.width!=0 ? view_size.x/static_cast<float>(Rpix.width) : 1.0f;
        ky = Rpix.height!=0 ? view_size.y/static_cast<float>(Rpix.height) : 1.0f;
       }

    [[nodiscard]] float width_of(const unsigned int pix) const noexcept { return kx * static_cast<float>(pix); }
    [[nodiscard]] float height_of(const unsigned int pix) const noexcept { return ky * static_cast<float>(pix); }
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

    [[nodiscard]] sf::FloatRect rect() const noexcept
       {
        const sf::Vector2f& c = i_view.getCenter();
        const sf::Vector2f& siz = i_view.getSize();
        return sf::FloatRect(c.x-siz.x/2.0f, c.y-siz.y/2.0f, siz.x, siz.y);
       }

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

    void draw_grid(const float dx, const float dy, sf::Color color) noexcept
       {
        const sf::FloatRect r = rect();
        const float r_right = r.left + r.width;
        const float r_bottom = r.top + r.height;

        sf::Vertex vertices[2];
        vertices[0].color = color;
        vertices[1].color = color;

        // Vertical lines
        if( dx>0.0f )
           {
            float x = dx * std::ceil(r.left/dx);
            while( x < r_right )
               {
                vertices[0].position = {x,r.top};
                vertices[1].position = {x,r_bottom};
                i_window.draw(vertices, 2, sf::Lines);
                x += dx;
               }
           }

        // Horizontal lines
        if( dy>0.0f )
           {
            float y = dy * std::ceil(r.top/dy);
            while( y < r_bottom )
               {
                vertices[0].position = {r.left,y};
                vertices[1].position = {r_right,y};
                i_window.draw(vertices, 2, sf::Lines);
                y += dy;
               }
           }
       }
};


/////////////////////////////////////////////////////////////////////////////
class Cross : public sf::Drawable
{
 public:
    sf::Color color{0,200,200};
    sf::Vector2f position{};
    float dim{10.0f};

 private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
       {
        sf::Vertex vertices[2];
        vertices[0].color = color;
        vertices[1].color = color;
        vertices[0].position = {position.x+dim,position.y};
        vertices[1].position = {position.x-dim,position.y};
        target.draw(vertices, 2, sf::Lines, states);
        vertices[0].position = {position.x,position.y+dim};
        vertices[1].position = {position.x,position.y-dim};
        target.draw(vertices, 2, sf::Lines, states);
       }
};


}//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//---- end unit -------------------------------------------------------------
#endif
