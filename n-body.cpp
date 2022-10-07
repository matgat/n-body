#include <array>
#include <algorithm> // std::clamp
#include <chrono> // std::chrono::*
#include <fmt/core.h> // fmt::*
#include <fmt/color.h> // fmt::color::*
#include <SFML/Graphics.hpp>

#include "sfml-addons.hpp" // sfadd::*
#include "universe.hpp" // Universe


//----------------------------------------------------------------------
void draw(sf::RenderWindow& window, const Universe::Vect& point)
{
    sf::CircleShape p(2);
    p.setOrigin(1, 1);
    p.setFillColor(sf::Color::White);
    p.setPosition(static_cast<float>(point.x),
                  static_cast<float>(point.y));
    window.draw(p);
}

//----------------------------------------------------------------------
void draw(sf::RenderWindow& window, const Universe::Body& body)
{
    auto get_body_color = [](const double mass) -> sf::Color
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
       };

    const float min_radius = 4;
    const float max_radius = 10;
    const float radius = std::clamp(static_cast<float>(body.mass()/100.0), min_radius, max_radius);

    sf::CircleShape shape(radius);

    shape.setOrigin(radius/2, radius/2);
    shape.setFillColor( get_body_color(body.mass()) );
    shape.setPosition( static_cast<float>(body.position().x),
                       static_cast<float>(body.position().y) );
    window.draw(shape);
}

//----------------------------------------------------------------------
void draw(sf::RenderWindow& window, const Universe& universe)
{
    // Center of mass
    draw(window, universe.center_of_mass());

    // Bodies
    for( const auto& body : universe.bodies() )
       {
        draw(window, body);
       }
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
    sfadd::View view{ window };

    sf::Text text;
    sf::Font font;
    if( !font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansCondensed.ttf") )
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Cannot load font");
    text.setFont(font);
    text.setCharacterSize(14); // [px]
    text.setFillColor(sf::Color::White);

    //sf::Text dbg_text;
    //dbg_text.setFont(font);
    //dbg_text.setFillColor(sf::Color::Yellow);

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
                    view.resize(event.size.width, event.size.height);
                    break;

                case sf::Event::MouseButtonPressed:
                    if( event.mouseButton.button==sf::Mouse::Left )
                       {
                        view.pan_init({event.mouseButton.x, event.mouseButton.y});
                       }
                    break;

                case sf::Event::MouseButtonReleased:
                    view.pan_end();
                    break;

                case sf::Event::MouseMoved:
                    if( view.panning() )
                       {
                        view.pan({event.mouseMove.x, event.mouseMove.y});
                       }
                    //{
                    //const sf::Vector2f p{ window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}) };
                    //const sf::FloatRect r = view.rect();
                    //dbg_text.setString(fmt::format("{};{} = {:.3f};{:.3f}\n"
                    //                               "view {:.3f};{:.3f} {:.3f}x{:.3f}",
                    //                   event.mouseMove.x, event.mouseMove.y, p.x, p.y,
                    //                   r.x, r.y, r.x, r.y));
                    //}
                    break;

                case sf::Event::MouseWheelScrolled:
                    view.zoom({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, event.mouseWheelScroll.delta>0);
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
        //dbg_text.setPosition( window.mapPixelToCoords({0,20}) );
        //window.draw(dbg_text);

        draw(window, universe);

        window.draw(text);
        window.display();
       }

    return 0;
}

