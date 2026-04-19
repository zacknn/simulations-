#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <optional>
struct Vec2 {
  float x = 0, y = 0;
  Vec2 operator+(const Vec2 &other) const { return {x + other.x, y + other.y}; }
  Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
};

int main() {

  sf::RenderWindow window(sf::VideoMode({800, 600}), "Physics Simulation",
                          sf::Style::Titlebar | sf::Style::Close);

  sf::CircleShape ball(20.0f);
  ball.setFillColor(sf::Color::Blue);
  ball.setOrigin({20.0f, 20.0f});
  ball.setRadius(20.0f);
  ball.setOrigin({20.0f, 20.0f});
  sf::RectangleShape floor(sf::Vector2f(800.0f, 20.0f));
  floor.setFillColor(sf::Color::Cyan);
  ball.setPosition({0.0f, 580.0f});
  Vec2 position{400, 100};
  Vec2 velocity{0, 0};
  Vec2 acceleration{0, 980.0f};

  sf::Clock clock;

  while (window.isOpen()) {

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    float dt = clock.restart().asSeconds();

    velocity = velocity + (acceleration * dt);
    position = position + (velocity * dt);

    // Ground
    if (position.y >= 580) {
      position.y = 580;
      if (std::abs(velocity.y) > 30.0) {
        velocity.y *= -0.7f;
        std::cout << "Bounce ! velocity Y : " << velocity.y << std::endl;
      } else {
        velocity.y = 0;
      }
    }
    // side walls
    if (position.x > 780) {
      position.x = 780;
      velocity.x *= -0.999f;
    }
    if (position.x < 20) {
      position.x = 20;
      velocity.x *= -0.999f;
    }

    if (position.x > 800)
      position.x = 0;
    if (position.x < 0)
      position.x = 800;

    ball.setPosition({position.x, position.y});

    window.clear();
    // window.draw(floor);
    window.draw(ball);
    window.display();
  }

  return 0;
}
