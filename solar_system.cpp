#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
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
    Circle.setOrigin({radius, radius});
    Circle.setPosition(position);
  }
  void update(float dt, const Plannet &other) {
    sf::Vector2f dir = other.position - position;

    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (dist < 1.f)
      return;

    sf::Vector2f dirNorm = dir / dist;

    float G = 0.1f;
    float accel = G * other.mass / (dist * dist);

    velocity += dirNorm * accel * dt;
    position += velocity * dt;
    Circle.setPosition(position);
  }
};
int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "solar system",
                          sf::Style::Titlebar | sf::Style::Close);
  sf::Vector2f center = sf::Vector2f(window.getSize()) / 2.f;
  window.setFramerateLimit(60);
  Plannet sun("Sun", 1.0e6f, 40.0f, sf::Vector2f(center), sf::Vector2f(0, 0),
              sf::Color::Yellow);
  Plannet earth("Earth", 1.0f, 10.0f, sf::Vector2f(center.x + 250.f, center.y),
                sf::Vector2f(0.0f, 20.0f), sf::Color::Blue);

  Plannet saturn("Saturn", 2.0f, 20.f, sf::Vector2f(center.x + 350.f, center.y),
                 sf::Vector2f(0.0f, 20.0f), sf::Color::Yellow);
  sf::Clock clock;
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
    }
    float dt = clock.restart().asSeconds();

    earth.update(dt, sun);
    saturn.update(dt, sun);

    window.clear();
    window.draw(sun.Circle);
    window.draw(earth.Circle);
    window.draw(saturn.Circle);
    window.display();
  }

  return 0;
}
