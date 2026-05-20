#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

struct Plannet {
  std::string name;
  float mass;
  sf::Vector2f position;
  sf::Vector2f velocity;

  sf::CircleShape Circle;

  Plannet(std::string n, float m, float radius, sf::Vector2f pos,
          sf::Vector2f vel, sf::Color color)
      : name(n), mass(m), position(pos), velocity(vel) {
    Circle.setRadius(radius);
    Circle.setFillColor(color);
    Circle.setPosition(position);
    Circle.setOrigin(position);
  }
};

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "solar system",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);
  Plannet sun("Sun", 1.989e30f, 40.0f, sf::Vector2f(640, 360),
              sf::Vector2f(0, 0), sf::Color::Yellow);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    window.clear();
    window.draw(sun.Circle);
    window.display();
  }

  return 0;
}
