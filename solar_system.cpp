#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int main () {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "solar system",
                          sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);



    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            }

        window.clear(sf::Color::Black);
        window.display();
    }
    return 0;
}