#include <fmt/core.h> // fmt::*
#include <SFML/Graphics.hpp>

#include "universe.hpp" // Universe

//----------------------------------------------------------------------
int main()
{
    // Creating a universe with a certain gravitational constant
    Universe universe(1); // Our universe: 6.67408E-11 m³/kg s²
    // Adding bodies (mass, initial position and speed)
    const auto& b1 = universe.add_body(500, Universe::Vect{500,400}, Universe::Vect{0,0});
    const auto& b2 = universe.add_body(100, Universe::Vect{540,400}, Universe::Vect{0,3});
    const auto& b3 = universe.add_body(1500, Universe::Vect{0,300}, Universe::Vect{2,0});

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

    const double dt = 1E-3;
    while(window.isOpen())
       {
        sf::Event event;
        while(window.pollEvent(event))
           {
            if(event.type == sf::Event::Closed) window.close();
           }
        universe.evolve_verlet(dt);

      #pragma clang diagnostic push
      #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
        window.clear();
        text.setString(fmt::format("{:.1f}", universe.total_energy()));
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

