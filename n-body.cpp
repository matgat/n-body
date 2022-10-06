#include <array>
#include <algorithm> // std::clamp
#include <chrono> // std::chrono::*
#include <fmt/core.h> // fmt::*
#include <fmt/color.h> // fmt::color::*
#include <SFML/Graphics.hpp>

#include "sfml-addons.hpp" // sfadd::*
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

    sfadd::Pan pan;
    sfadd::Zoom zoom;

    sf::Text text;
    sf::Font font;
    if( !font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansCondensed.ttf") )
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Cannot load font");
    text.setFont(font);
    text.setCharacterSize(14); // [px]
    text.setFillColor(sf::Color::White);

    sf::Text dbg_text;
    dbg_text.setFont(font);
    dbg_text.setFillColor(sf::Color::Yellow);

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
            switch( event.type )
               {
                case sf::Event::Resized:
                    view.setSize(zoom.width_of(event.size.width), zoom.height_of(event.size.height));
                    window.setView(view);
                    break;

                case sf::Event::MouseButtonPressed:
                    if( event.mouseButton.button==sf::Mouse::Left )
                       {
                        pan.init({event.mouseButton.x, event.mouseButton.y});
                       }
                    break;

                case sf::Event::MouseButtonReleased:
                    pan.end();
                    break;

                case sf::Event::MouseMoved:
                    if( pan.is_active() )
                       {
                        pan(window, view, {event.mouseMove.x, event.mouseMove.y});
                       }
                    else
                       {
                        const sf::Vector2f p{ window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}) };
                        const sf::Vector2f vc{view.getCenter()};
                        const sf::Vector2f vsiz{view.getSize()};

                        dbg_text.setString(fmt::format("{};{} = {:.3f};{:.3f}\n"
                                                       "view {:.3f};{:.3f} {:.3f}x{:.3f}",
                                           event.mouseMove.x, event.mouseMove.y, p.x, p.y,
                                           vc.x, vc.y, vsiz.x, vsiz.y));
                       }
                    break;

                case sf::Event::MouseWheelScrolled:
                    zoom(window, view, {event.mouseWheelScroll.x, event.mouseWheelScroll.y}, event.mouseWheelScroll.delta>0);
                    break;

                case sf::Event::KeyPressed:
                    if( event.key.code==sf::Keyboard::Escape )
                       {
                        window.close();
                       }
                    break;

                case sf::Event::Closed:
                    window.close();
                    break;

                default:
                    break;
               }
           }

        const sec duration = clock::now() - before;
        before = clock::now();
        universe.evolve_verlet( k_time * duration.count() );

        window.clear();

        text.setString(fmt::format("E={:.1f}  dt={:.1f}ms  t={:.0f}s", universe.total_energy(), 1E3*duration.count(), universe.time()));
        text.setPosition( window.mapPixelToCoords({0,0}) );

        //sf::Vector2i mouse_pix = sf::Mouse::getPosition(window);
        dbg_text.setPosition( window.mapPixelToCoords({0,20}) );

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
        window.draw(dbg_text);
        window.display();
       }

    return 0;
}

