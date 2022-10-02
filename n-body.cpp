#include <chrono> // std::chrono::*
#include <fmt/core.h> // fmt::*
#include <SFML/Graphics.hpp>

#include "universe.hpp" // Universe

//----------------------------------------------------------------------
int main()
{
    // Creating a universe with a certain gravitational constant
    Universe universe(1); // Our universe: 6.67408E-11 m³/kg s²
    // Adding bodies (mass, initial position and speed)
    const auto& b1 = universe.add_body(500, {500,400}, {0,0});
    const auto& b2 = universe.add_body(100, {540,400}, {0,3});
    const auto& b3 = universe.add_body(1500, {0,300}, {2,0});

    sf::RenderWindow window(sf::VideoMode(800, 800), "n-body");

    sf::Text text;
    sf::Font font;
    if( !font.loadFromFile("/usr/share/fonts/TTF/DejaVuSansCondensed.ttf") )
        fmt::print("Cannot load font");
    text.setFont(font);
    text.setCharacterSize(12); // [px]
    text.setFillColor(sf::Color::White);

    sf::CircleShape c(2);
    c.setOrigin(1, 1);
    c.setFillColor(sf::Color::White);

    sf::CircleShape body1(7);
    body1.setOrigin(3.5, 3.5);
    body1.setFillColor(sf::Color::Green);

    sf::CircleShape body2(5);
    body2.setOrigin(2.5, 2.5);
    body2.setFillColor(sf::Color::Red);

    sf::CircleShape body3(7);
    body3.setOrigin(3.5, 3.5);
    body3.setFillColor(sf::Color::Blue);


    using clock = std::chrono::high_resolution_clock;
    using sec = std::chrono::duration<double>;
    //using ms = std::chrono::duration<double, std::milli>;
    auto before = clock::now();
    const double k_time = 10.0;

    while(window.isOpen())
       {
        sf::Event event;
        while(window.pollEvent(event))
           {
            if(event.type == sf::Event::Closed) window.close();
           }

        const sec duration = clock::now() - before;
        before = clock::now();
        universe.evolve_verlet( k_time * duration.count() );

      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
        window.clear();
        text.setString(fmt::format("E={:.1f}  dt={:.1f}ms  t={:.0f}s", universe.total_energy(), 1E3*duration.count(), universe.time()));
        const auto C = universe.center_of_mass();
        c.setPosition(C.x, C.y);
        body1.setPosition(b1.position().x, b1.position().y);
        body2.setPosition(b2.position().x, b2.position().y);
        body3.setPosition(b3.position().x, b3.position().y);
        window.draw(c);
        window.draw(body1);
        window.draw(body2);
        window.draw(body3);
        window.draw(text);
        window.display();
      #pragma clang diagnostic pop
       }

    return 0;
}

