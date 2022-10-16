#include <array>
#include <algorithm> // std::clamp
#include <chrono> // std::chrono::*
#include <fmt/core.h> // fmt::*
#include <fmt/color.h> // fmt::color::*
#include <SFML/Graphics.hpp>

#include "sfml-addons.hpp" // sfadd::*
#include "sfml-addons-color.hpp" // sfadd::Color
#include "universe.hpp" // Universe


//----------------------------------------------------------------------
void draw(sf::RenderWindow& window, const Universe::Vect& point)
{
    sfadd::Cross c;
    c.position.x = static_cast<float>(point.x);
    c.position.y = static_cast<float>(point.y);
    window.draw(c);
}

//----------------------------------------------------------------------
void draw(sf::RenderWindow& window, const Universe::Body& body)
{
    auto get_body_color = [](const double mass) noexcept -> sf::Color
       {
        sfadd::Color col{78,72,8};
        col.lum_incr( static_cast<float>(mass)/10000.0f );
        return col;
       };

    sf::CircleShape shape( static_cast<float>(body.radius()) );
    const float r_half = shape.getRadius()/2.0f;
    shape.setOrigin(r_half, r_half);
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
Universe setup_small_around_big()
{
    Universe universe(1.0); // Our universe: 6.67408E-11 m³/kg s²
    // Adding bodies (mass, initial position and speed)
    universe.add_body(5000, {400,400}, {0,0})
            .add_body(200, {200,400}, {0,4})
            .add_body(300, {700,400}, {0,-2})
            .add_body(200, {400,500}, {10,0})
            .add_body(500, {400,300}, {-5,0});
    return universe;
}

//----------------------------------------------------------------------
Universe setup_coll_test()
{
    Universe universe(0.0);
    universe.add_body(40000, {300,400}, {2,1})
            .add_body(20000, {500,396}, {-2,1});
    return universe;
}


//----------------------------------------------------------------------
int main()
{
    // Creating a universe with a certain gravitational constant
    Universe universe = setup_small_around_big();
    //Universe universe = setup_coll_test();

    sf::RenderWindow window(sf::VideoMode(800, 800), "n-body");
    sfadd::View view{ window };

    sf::Text text;
    sf::Font font;

  #if defined(_WIN32) || defined(_WIN64)
    if( !font.loadFromFile("c:/windows/fonts/DejaVuSansCondensed.ttf") )
  #elif defined(__unix__) || defined(__linux__)
    if( !font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansCondensed.ttf") )
  #endif
       {
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Cannot load font");
       }
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(14);

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
                        const sf::Vector2f p = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                        universe.add_body(200, {p.x+20, p.y}, {8.0f,0});
                        universe.add_body(200, {p.x-20, p.y}, {-8.0f,0});
                       }
                    else if( event.mouseButton.button==sf::Mouse::Middle )
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
        universe.handle_collisions();

        window.clear();
        view.draw_grid(100,100,sf::Color{50,50,50});

        text.setString(fmt::format("E={:.1f}  dt={:.1f}ms  t={:.0f}s", universe.total_energy(), 1E3*duration.count(), universe.time()));
        text.setPosition( window.mapPixelToCoords({0,0}) );
        //text.setCharacterSize(14);

        //sf::Vector2i mouse_pix = sf::Mouse::getPosition(window);
        //dbg_text.setPosition( window.mapPixelToCoords({0,20}) );
        //window.draw(dbg_text);

        draw(window, universe);

        window.draw(text);
        window.display();
       }

    return 0;
}

