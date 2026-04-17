#include <SFML/Graphics.hpp>
#include <optional>

struct Vec2 {
  float x = 0, y = 0;
  Vec2 operator+(const Vec2 &other) const { return {x + other.x, y + other.y}; }
  Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
};

int main() {
  
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Physics Simulation");

  sf::CircleShape ball(20.0f);
  ball.setFillColor(sf::Color::Blue);
  ball.setOrigin({20.0f, 20.0f}); 

  Vec2 position{400, 100};
  Vec2 velocity{100, 0};
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

    
    if (position.y > 580) {
      position.y = 580;
      velocity.y *= -0.7f; 
    }

    
    if (position.x > 800)
      position.x = 0;
    if (position.x < 0)
      position.x = 800;

    ball.setPosition({position.x, position.y});

    window.clear();
    window.draw(ball);
    window.display();
  }

  return 0;
}
