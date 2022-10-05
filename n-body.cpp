#include <array>
#include <algorithm> // std::clamp
#include <chrono> // std::chrono::*
#include <fmt/core.h> // fmt::*
#include <fmt/color.h> // fmt::color::*
#include <SFML/Graphics.hpp>

#include "universe.hpp" // Universe

//----------------------------------------------------------------------
sf::Color get_body_color(const double mass)
{
    const auto colors = std::array
       {
        sf::Color::Blue,
        sf::Color::Green,
        sf::Color::Red,
        sf::Color::Yellow
       };
    const std::size_t idx = std::clamp(static_cast<std::size_t>(mass/200.0),
                                       static_cast<std::size_t>(0u),
                                       static_cast<std::size_t>(colors.size()-1));
    return colors[idx];
}

//----------------------------------------------------------------------
int main()
{
    // Creating a universe with a certain gravitational constant
    Universe universe(1); // Our universe: 6.67408E-11 m³/kg s²
    // Adding bodies (mass, initial position and speed)
    universe.add_body(10000, {400,400}, {0,0})
            .add_body(100, {200,400}, {0,4})
            .add_body(200, {700,400}, {0,-2})
            .add_body(100, {400,500}, {10,0})
            .add_body(200, {400,300}, {-5,0});

    sf::RenderWindow window(sf::VideoMode(800, 800), "n-body");
    sf::View view = window.getDefaultView();
    window.setView(view);

    struct{ sf::Vector2i clicked,center; bool active=false; } pan;


    sf::Text text;
    sf::Font font;
    if( !font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansCondensed.ttf") )
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Cannot load font");
    text.setFont(font);
    text.setCharacterSize(12); // [px]
    text.setFillColor(sf::Color::White);

    // Center of mass
    sf::CircleShape cdm(2);
    cdm.setOrigin(1, 1);
    cdm.setFillColor(sf::Color::White);

    struct body_t final
       {
        body_t() noexcept : data(nullptr) {}
        body_t(const Universe::Body* const p, sf::CircleShape&& o) noexcept : data(p), shape(o) {}

        const Universe::Body* data;
        sf::CircleShape shape;
       };
    std::vector<body_t> bodies;
    bodies.reserve( universe.bodies().size() );
    for( const auto& body : universe.bodies() )
       {
        const float min_radius = 4;
        const float max_radius = 10;
        const float radius = std::clamp(static_cast<float>(body.mass()/100.0), min_radius, max_radius);
        auto& new_body = bodies.emplace_back( body_t{&body, sf::CircleShape(radius)} );
        new_body.shape.setOrigin(radius/2, radius/2);
        new_body.shape.setFillColor( get_body_color(body.mass()) );
       }

    using clock = std::chrono::high_resolution_clock;
    using sec = std::chrono::duration<double>;
    //using ms = std::chrono::duration<double, std::milli>;
    auto before = clock::now();
    const double k_time = 10.0;

    while( window.isOpen() )
       {
        sf::Event event;
        while( window.pollEvent(event) )
           {

            //switch( event.type )
            //   {
            //    case sf::Event::KeyPressed:
            //        break;
            //    case sf::Event::Closed:
            //        window.close();
            //        break;
            //   }


            if( event.type==sf::Event::Resized )
               {// Update the view to the new size of the window
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                view.reset(visibleArea);
                window.setView(view);
               }
            else if( event.type==sf::Event::MouseButtonPressed )
               {
                if( event.mouseButton.button==sf::Mouse::Left )
                   {
                    pan.clicked = {event.mouseButton.x, event.mouseButton.y};
                    pan.center = window.mapCoordsToPixel(view.getCenter());
                    pan.active = true;
                   }
               }
            else if( event.type==sf::Event::MouseButtonReleased )
               {
                pan.active = false;
               }
            else if( event.type==sf::Event::MouseMoved && pan.active )
               {
                auto do_pan = [&window, &view, &pan](const sf::Vector2i pixel) -> void
                   {
                    //const sf::Vector2f p{ window.mapPixelToCoords(pixel) };
                    //view.setCenter( sf::Vector2f(pixel) );
                    //view.setCenter(sf::Vector2f(pan.center + pan.clicked - pixel));
                    view.move(sf::Vector2f(pan.clicked - pixel));
                    pan.clicked = pixel;
                    window.setView(view);
                   };
                do_pan({event.mouseMove.x, event.mouseMove.y});
               }
            else if( event.type==sf::Event::MouseWheelScrolled )
               {
                auto zoom_at = [&window, &view](const sf::Vector2i pixel, const float zoom) -> void
                   {
                    const sf::Vector2f c_before{ window.mapPixelToCoords(pixel) };
                    view.zoom(zoom);
                    window.setView(view);
                    const sf::Vector2f c_after{ window.mapPixelToCoords(pixel) };
                    view.move(c_before - c_after);
                    window.setView(view);
                   };
                const float zoomAmount{ 1.1f }; // 10%
                zoom_at({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, event.mouseWheelScroll.delta>0 ? (1.f / zoomAmount) : zoomAmount);
               }
            else if( event.type==sf::Event::KeyPressed )
               {
                if( event.key.code==sf::Keyboard::Escape )
                   {
                    window.close();
                   }
               }
            else if( event.type==sf::Event::Closed )
               {
                window.close();
               }

            // get the current mouse position in the window
            //sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

            // convert it to world coordinates
            //sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            //view.zoom(0.5f);
            //view.move(100.f, 100.f);
            //view.setCenter(200.f, 200.f);
            //    view.zoom(0.5f);

           }

        const sec duration = clock::now() - before;
        before = clock::now();
        universe.evolve_verlet( k_time * duration.count() );

        window.clear();
        text.setString(fmt::format("E={:.1f}  dt={:.1f}ms  t={:.0f}s", universe.total_energy(), 1E3*duration.count(), universe.time()));

        const auto C = universe.center_of_mass();
        cdm.setPosition(static_cast<float>(C.x),
                        static_cast<float>(C.y));
        window.draw(cdm);

        for( auto& body : bodies )
           {
            body.shape.setPosition(static_cast<float>(body.data->position().x),
                                   static_cast<float>(body.data->position().y));
            window.draw(body.shape);
           }

        window.draw(text);
        window.display();
       }

    return 0;
}

